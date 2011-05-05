/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 3 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.TXT included in the
 ** packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@testcocoon.org if any conditions of this licensing are
 ** not clear to you.
 **
 ****************************************************************************/

#define PERFORMANCE_TEST 1
#define DUMP_ALLOCATIONS 0
#define DUMP_CALLS 0

#if LOG
#include "debug.h"
#include "tmpfile.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#if defined(OS_LINUX) || defined(OS_MACX)
#include <fnmatch.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#elif defined(OS_WIN32)
#include <windows.h>
#include <direct.h>
#endif
#include <time.h>

#define _CHAINE_LEN 1024
static FILE *logfile=NULL;
static char log_filename[_CHAINE_LEN]="";
static int  log_line=0;

#if PERFORMANCE_TEST
#if defined(OS_WIN32)
static LARGE_INTEGER perfInit;
static LARGE_INTEGER perffreq;
#elif  defined(OS_LINUX) ||  defined(OS_MACX)
static clock_t perfInit;
#endif
#endif
inline void TimeStampInit()
{
#if PERFORMANCE_TEST
#if defined(OS_WIN32)
  QueryPerformanceCounter(&perfInit);
  QueryPerformanceFrequency(&perffreq);
#elif  defined(OS_LINUX) || defined(OS_MACX)
  perfInit=clock();
#endif
#endif
}

inline double TimeStamp()
{
#if PERFORMANCE_TEST
#if defined(OS_WIN32)
  LARGE_INTEGER perfnow;
  QueryPerformanceCounter(&perfnow);
  double diff=static_cast<double>(perfnow.QuadPart-perfInit.QuadPart)/static_cast<double>(perffreq.QuadPart);
  return diff;
#elif  defined(OS_LINUX) || defined(OS_MACX)
  clock_t perfnow=clock();
  double diff=static_cast<double>(perfnow-perfInit)/static_cast<double>(CLOCKS_PER_SEC);
  return diff;
#endif
#endif
}

void record_set_position(const char*filename,int line)
{
  strncpy(log_filename,filename,_CHAINE_LEN);
  log_line=line;
}

static bool log_start_of_line=true;
void record_debug_log (const char *s)
{
  if (logfile)
  {
    double timestamp=TimeStamp();
    if (log_start_of_line)
      fprintf(logfile,"%s:%i:%9.6f:",log_filename,log_line,timestamp);
    int s_lg=strlen(s);
    if (s_lg>0)
      log_start_of_line = (s[s_lg-1]=='\n');

    fputs(s,logfile);
  }
}

const char *log_folder()
{
#if defined(OS_LINUX) || defined(OS_MACX)
  static const char*logfolder="/tmp/testcocoon/";
  umask(0);
  mkdir(logfolder,0777);
  return logfolder;
#elif defined(OS_WIN32)
  static char logfolder[_CHAINE_LEN];
  if (!getenv("TESTCOCOON"))
  {
    FATAL1("System variable TESTCOCOON not defined");
  }
  
  strcpy(logfolder,getenv("TESTCOCOON"));
  strcat(logfolder,"\\logfiles\\");
  _mkdir(logfolder);
  return logfolder;
#endif
}

void record_debug_open_log ()
{
  std::string logname=TmpFile::object().generate("coveragescanner_$ID$.cslog",true);

  if (logfile!=NULL)
    record_debug_close_log ();  
  logfile=fopen(logname.c_str(),"w");
  TimeStampInit();
}

void record_debug_close_log ()
{
    record_debug_log("\nClosing log file\n");
    if (logfile!=NULL)
        fclose(logfile);
    logfile=NULL;
}

int record_debug_printf( const char* format, ... )
{
   va_list arg;
   if (logfile)
   {
     double timestamp=TimeStamp();
     if (log_start_of_line)
       fprintf(logfile,"%s:%i:%9.6f:",log_filename,log_line,timestamp);
     int format_lg=strlen(format);
     if (format_lg>0)
       log_start_of_line = (format[format_lg-1]=='\n');
     
     va_start( arg, format );     /* Initialize variable arguments. */
     int ret=vfprintf(logfile,format,arg);
     va_end( arg );              /* Reset variable arguments.      */
     return ret;
   }
   else
     return 0;
}

int record_debug_fprintf( FILE* ,const char* format, ... )
{
   va_list arg;
   if (logfile)
   {
     va_start( arg, format );     /* Initialize variable arguments. */
     int ret=vfprintf(logfile,format,arg);
     va_end( arg );              /* Reset variable arguments.      */
     return ret;
   }
   else
     return 0;
}

struct debug_allocation_t
{
  char *file;
  int line;
  void *ptr;
  int size;
  debug_allocation_t *next;
};
debug_allocation_t *debug_allocations=NULL;

static void malloc_debug_alloc(const char*file,int line,void *p,int size)
{
  debug_allocation_t *trc=(debug_allocation_t*)malloc(sizeof(debug_allocation_t));
  trc->file=strdup(file);
  trc->line=line;
  trc->ptr=p;
  trc->size=size;
  trc->next=debug_allocations;
  debug_allocations=trc;
}

static void free_debug_alloc(const char*file,int line,void *__ptr,bool clean)
{
  if (__ptr)
  {
    bool found=false;
    debug_allocation_t *plast=NULL;
    for (debug_allocation_t *p=debug_allocations; p!=NULL; p=p->next)
    {
      if (p->ptr==__ptr)
      {
        found=true;
        if (plast)
          plast->next=p->next;
        else
          debug_allocations=p->next;
        if (memcmp(&(((char*)p->ptr)[p->size]),"DEAD\0",5)!=0)
        {
          record_set_position(file,line);
          record_debug_printf("WARNING: corrupt pointer %p allocated here %s:%i\n",__ptr,p->file,p->line);
        }
        if (clean)
           memset(p->ptr,0xdc,p->size);
        free(p->file);
        memset(p,0,sizeof(debug_allocation_t));
        free(p);
        break;
      }
      plast=p;
    }
    if (!found)
    {
      record_set_position(file,line);
      record_debug_printf("WARNING: free(%p) from a non allocated pointer\n",__ptr);
    }
  }
}



void *debug_malloc (const char*file,int line,size_t __size) 
{
  void *p=malloc(__size+5);
  memset(p,0xCC,__size);
  memcpy(&(((char*)p)[__size]),"DEAD\0",5);
  malloc_debug_alloc(file,line,p,__size);

#if DUMP_ALLOCATIONS
  record_set_position(file,line);
  record_debug_printf("malloc(%i) -> %p\n",__size,p);
#endif
  return p;
}

void *debug_realloc (const char*file,int line,void *__ptr, size_t __size) 
{
  free_debug_alloc(file,line,__ptr,false);
  void *p=realloc(__ptr,__size+5);
  memcpy(&(((char*)p)[__size]),"DEAD\0",5);
  malloc_debug_alloc(file,line,p,__size);
#if DUMP_ALLOCATIONS
  record_set_position(file,line);
  record_debug_printf("realloc(%p,%i) -> %p\n",__ptr,__size,p);
#endif
  return p;
}

void debug_free (const char*file,int line,void *__ptr) 
{
  if (__ptr)
  {
    free_debug_alloc(file,line,__ptr,true);
#if DUMP_ALLOCATIONS
    record_set_position(file,line);
    record_debug_printf("free(%p)\n",__ptr);
#endif
  }
  free(__ptr);
}

char* debug_strdup (const char*file,int line,const char *__ptr)
{
  int __size=strlen(__ptr)+1;
  char *p=(char*)malloc(__size+5);
  strcpy(p,__ptr);
  memcpy(&(((char*)p)[__size]),"DEAD\0",5);
  malloc_debug_alloc(file,line,p,__size);
#if DUMP_ALLOCATIONS
  record_set_position(file,line);
  record_debug_printf("strdup(%p) -> %p\n",__ptr,p);
#endif
  return p;
}

void debug_memory_report()
{
  record_debug_printf("Memory items allocated:\n");
  for (debug_allocation_t *p=debug_allocations;p;p=p->next)
  {
    record_set_position(p->file,p->line); 
    record_debug_printf("allocated address:%p size:%i\n",p->ptr,p->size);
    if (memcmp(&(((char*)p->ptr)[p->size]),"DEAD\0",5)!=0)
    {
      record_debug_printf("WARNING: corrupt pointer %p allocated here %s:%i\n",p->ptr,p->file,p->line);
    }
  }
}

struct debug_call_t
{
  char *file;
  int line;
  void *name;
  debug_call_t *next;
};
debug_call_t *debug_calls=NULL;
#if DUMP_CALLS
int debug_call_level=0;
#endif

DebugFuntionTracer::DebugFuntionTracer(const char *f,const char *file, int line)
{
  _f=f;
  _line=line;
  _file=file;
  debug_call_t *call=(debug_call_t*)malloc(sizeof(debug_call_t));
  call->file=strdup(file);
  call->name=strdup(f);
  call->line=line;
  call->next=debug_calls;
  debug_calls=call;
#if DUMP_CALLS
  record_set_position(_file,_line);
  for (int i=0;i<debug_call_level;i++)
    record_debug_printf("-");
  record_debug_printf(">:%s\n",_f);
  debug_call_level++;
#endif
}
DebugFuntionTracer::~DebugFuntionTracer()
{
  if (debug_calls)
  {
    debug_call_t *call=debug_calls;
    debug_calls=debug_calls->next;
    free(call->file);
    free(call->name);
    free(call);
  }
#if DUMP_CALLS
  debug_call_level--;
  record_set_position(_file,_line);
  for (int i=0;i<debug_call_level;i++)
    record_debug_printf("-");
  record_debug_printf("<:%s\n",_f);
#endif
}

void debug_print_callstack()
{
  record_debug_printf("Call stack:\n");
  for (debug_call_t *call=debug_calls;call;call=call->next)
  {
    record_set_position(call->file,call->line); 
    record_debug_printf(" -> %s\n",call->name);
  }
}
#endif
