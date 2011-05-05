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

#ifndef DEBUG_H__
#define DEBUG_H__

#include "../commoncode/commonglobal.h"
#include "libcsmespdef.h"
#include <stdlib.h>
#include <string>
#include "stringtool.h"
#if LOG
#include <stdio.h>
#include <assert.h>

  void record_debug_open_log ();
  void record_debug_close_log ();
  void record_debug_log (const char *);

  const char* log_folder() ;
  int record_debug_printf( const char* format, ... );
  int record_debug_fprintf( FILE *f,const char* format, ... );
  void record_set_position(const char*filename,int line);
#define DEBUG_PRINTF record_set_position(__FILE__,__LINE__),record_debug_printf
#define DEBUG_FPRINTF record_set_position(__FILE__,__LINE__),record_debug_fprintf
#define DEBUG_PRINTF1(X1)  record_set_position(__FILE__,__LINE__),record_debug_log(X1)
#define DEBUG_PRINTF2(FORMAT,X1)  DEBUG_PRINTF(FORMAT,X1)
#define DEBUG_PRINTF3(FORMAT,X1,X2)  DEBUG_PRINTF(FORMAT,X1,X2)
#define DEBUG_PRINTF4(FORMAT,X1,X2,X3)  DEBUG_PRINTF(FORMAT,X1,X2,X3)
#define DEBUG_PRINTF5(FORMAT,X1,X2,X3,X4)  DEBUG_PRINTF(FORMAT,X1,X2,X3,X4)
#define DEBUG_PRINTF6(FORMAT,X1,X2,X3,X4,X5)  DEBUG_PRINTF(FORMAT,X1,X2,X3,X4,X5)
#define DEBUG_PRINTF7(FORMAT,X1,X2,X3,X4,X5,X6)  DEBUG_PRINTF(FORMAT,X1,X2,X3,X4,X5,X6)
#define DEBUG_PRINTF8(FORMAT,X1,X2,X3,X4,X5,X6,X7)  DEBUG_PRINTF(FORMAT,X1,X2,X3,X4,X5,X6,X7)
#define DEBUG_PRINTF9(FORMAT,X1,X2,X3,X4,X5,X6,X7,X8)  DEBUG_PRINTF(FORMAT,X1,X2,X3,X4,X5,X6,X7,X8)
#define DEBUG_PRINTF10(FORMAT,X1,X2,X3,X4,X5,X6,X7,X8,X9)  DEBUG_PRINTF(FORMAT,X1,X2,X3,X4,X5,X6,X7,X8,X9)
#define DEBUG_PRINTF11(FORMAT,X1,X2,X3,X4,X5,X6,X7,X8,X9,X10)  DEBUG_PRINTF(FORMAT,X1,X2,X3,X4,X5,X6,X7,X8,X9,X10)

void *debug_malloc (const char*file,int line,size_t __size) ;
void *debug_realloc (const char*file,int line,void *__ptr, size_t __size) ;
void debug_free (const char*file,int line,void *__ptr);
char* debug_strdup (const char*file,int line,const char *__ptr);
void debug_memory_report();

void debug_print_callstack();
#define CALLSTACK debug_print_callstack();
#define STRDUP(ADDR) debug_strdup(__FILE__,__LINE__,ADDR)
#define FREE(ADDR) debug_free(__FILE__,__LINE__,ADDR)
#define MALLOC(SIZE) debug_malloc(__FILE__,__LINE__,SIZE)
#define REALLOC(ADDR,SIZE) debug_realloc(__FILE__,__LINE__,ADDR,SIZE)
#define MEMORY_REPORT debug_memory_report()
#  define OPEN_LOG record_debug_open_log
#  define CLOSE_LOG   record_debug_close_log
#  define EXIT(X)     { DEBUG_PRINTF4("exit:%s:%i=%i\n",__FILE__,__LINE__,X); CLOSE_LOG(); exit(X); }
#  define FATAL1(X1) { DEBUG_PRINTF3("FATAL:%s:%i:",__FILE__,__LINE__); DEBUG_PRINTF1(X1);  DEBUG_PRINTF1("\n"); throw string_printf ("%s",X1) ;}
#  define FATAL2(X1,X2) { DEBUG_PRINTF3("FATAL:%s:%i:",__FILE__,__LINE__); DEBUG_PRINTF2(X1,X2);  DEBUG_PRINTF1("\n"); throw string_printf(X1,X2);  }
#  define FATAL3(X1,X2,X3) { DEBUG_PRINTF3("FATAL:%s:%i:",__FILE__,__LINE__); DEBUG_PRINTF3(X1,X2,X3);  DEBUG_PRINTF1("\n"); throw string_printf(X1,X2,X3);  }
#  define FATAL4(X1,X2,X3,X4) { DEBUG_PRINTF3("FATAL:%s:%i:",__FILE__,__LINE__); DEBUG_PRINTF4(X1,X2,X3,X4);  DEBUG_PRINTF1("\n"); throw string_printf(X1,X2,X3,X4);  }
#  define FATAL5(X1,X2,X3,X4,X5) { DEBUG_PRINTF3("FATAL:%s:%i:",__FILE__,__LINE__); DEBUG_PRINTF5(X1,X2,X3,X4,X5);  DEBUG_PRINTF1("\n"); throw string_printf(X1,X2,X3,X4,X5);  }
#if defined(OS_MACX) || defined(OS_LINUX)
#  define DEBUG(X...) DEBUG_PRINTF(X)
#endif
#  define DEBUG1(X1) { DEBUG_PRINTF1(X1);   }
#  define DEBUG2(X1,X2) { DEBUG_PRINTF2(X1,X2);   }
#  define DEBUG3(X1,X2,X3) { DEBUG_PRINTF3(X1,X2,X3);  }
#  define DEBUG4(X1,X2,X3,X4) { DEBUG_PRINTF4(X1,X2,X3,X4);  }
#  define DEBUG5(X1,X2,X3,X4,X5) { DEBUG_PRINTF5(X1,X2,X3,X4,X5);  }
#  define DEBUG6(X1,X2,X3,X4,X5,X6) { DEBUG_PRINTF6(X1,X2,X3,X4,X5,X6);  }
#  define DEBUG7(X1,X2,X3,X4,X5,X6,X7) { DEBUG_PRINTF7(X1,X2,X3,X4,X5,X6,X7);  }
#  define DEBUG8(X1,X2,X3,X4,X5,X6,X7,X8) { DEBUG_PRINTF8(X1,X2,X3,X4,X5,X6,X7,X8);  }
#  define DEBUG10(X1,X2,X3,X4,X5,X6,X7,X8,X9,X10) { DEBUG_PRINTF10(X1,X2,X3,X4,X5,X6,X7,X8,X9,X10);  }
#  define DEBUG11(X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11) { DEBUG_PRINTF11(X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11);  }
#  define ERROR1(X1) { fprintf(stderr,"Error:%s:%i:",__FILE__,__LINE__); fprintf(stderr,X1); DEBUG_PRINTF1("Error:");DEBUG_PRINTF1(X1); }
#  define ERROR2(X1,X2) { fprintf(stderr,"Error:%s:%i:",__FILE__,__LINE__); fprintf(stderr,X1,X2); DEBUG_PRINTF1("Error:");DEBUG_PRINTF2(X1,X2); }
#  define ERROR3(X1,X2,X3) { fprintf(stderr,"Error:%s:%i:",__FILE__,__LINE__); fprintf(stderr,X1,X2,X3); DEBUG_PRINTF1("Error:");DEBUG_PRINTF3(X1,X2,X3); }
#  define ERROR4(X1,X2,X3,X4) { fprintf(stderr,"Error:%s:%i:",__FILE__,__LINE__); fprintf(stderr,X1,X2,X3,X4); DEBUG_PRINTF1("Error:");DEBUG_PRINTF4(X1,X2,X3,X4); }
#  define ERROR5(X1,X2,X3,X4,X5) { fprintf(stderr,"Error:%s:%i:",__FILE__,__LINE__); fprintf(stderr,X1,X2,X3,X4,X5); DEBUG_PRINTF1("Error:");DEBUG_PRINTF5(X1,X2,X3,X4,X5); }
#  define WARNING1(X1) {  fprintf(stderr,"Warning:%s:%i:",__FILE__,__LINE__); fprintf(stderr,X1); DEBUG_PRINTF1("Warning:");DEBUG_PRINTF1(X1); }
#  define WARNING2(X1,X2) {  fprintf(stderr,"Warning:%s:%i:",__FILE__,__LINE__); fprintf(stderr,X1,X2); DEBUG_PRINTF1("Warning:");DEBUG_PRINTF2(X1,X2); }
#  define WARNING3(X1,X2,X3) {  fprintf(stderr,"Warning:%s:%i:",__FILE__,__LINE__); fprintf(stderr,X1,X2,X3); DEBUG_PRINTF1("Warning:");DEBUG_PRINTF3(X1,X2,X3); }
#  define INFO1(X1) {  fprintf(stderr,"Info:%s:%i:",__FILE__,__LINE__); fprintf(stderr,X1); DEBUG_PRINTF1("Warning:");DEBUG_PRINTF1(X1); }
#  define INFO2(X1,X2) {  fprintf(stderr,"Info:%s:%i:",__FILE__,__LINE__); fprintf(stderr,X1,X2); DEBUG_PRINTF1("Warning:");DEBUG_PRINTF2(X1,X2); }
#  define INFO3(X1,X2,X3) {  fprintf(stderr,"Info:%s:%i:",__FILE__,__LINE__); fprintf(stderr,X1,X2,X3); DEBUG_PRINTF1("Warning:");DEBUG_PRINTF3(X1,X2,X3); }

class DebugFuntionTracer
{
  const char *_f;
  const char *_file;
  int _line;
  public:
    DebugFuntionTracer(const char *f,const char *file, int line);
    ~DebugFuntionTracer();
};
#define FUNCTION_TRACE DebugFuntionTracer __function__tracer(__FUNCTION__,__FILE__,__LINE__);
#else
#if defined(OS_LINUX) || defined(OS_MACX)
#  define DEBUG(X...) { }
#endif
#  define PDEBUG()
#  define EXIT(X)     {  exit(X); }
#  define FATAL1(X1) { throw string_printf ("%s",X1) ; }
#  define FATAL2(X1,X2) { throw string_printf(X1,X2);  }
#  define FATAL3(X1,X2,X3) { throw string_printf(X1,X2,X3);  }
#  define FATAL4(X1,X2,X3,X4) { throw string_printf(X1,X2,X3,X4);  }
#  define FATAL5(X1,X2,X3,X4,X5) {throw string_printf(X1,X2,X3,X4,X5);   }
#  define DEBUG1(X1) 
#  define DEBUG2(X1,X2) 
#  define DEBUG3(X1,X2,X3) 
#  define DEBUG4(X1,X2,X3,X4) 
#  define DEBUG5(X1,X2,X3,X4,X5) 
#  define DEBUG6(X1,X2,X3,X4,X5,X6) 
#  define DEBUG7(X1,X2,X3,X4,X5,X6,X7)
#  define DEBUG8(X1,X2,X3,X4,X5,X6,X7,X8)
#  define DEBUG10(X1,X2,X3,X4,X5,X6,X7,X8,X9,X10) 
#  define DEBUG11(X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11) 
#  define ERROR1(X1) { fprintf(stderr,"Error:"); fprintf(stderr,X1); }
#  define ERROR2(X1,X2) { fprintf(stderr,"Error:"); fprintf(stderr,X1,X2); }
#  define ERROR3(X1,X2,X3) { fprintf(stderr,"Error:"); fprintf(stderr,X1,X2,X3); }
#  define ERROR4(X1,X2,X3,X4) { fprintf(stderr,"Error:"); fprintf(stderr,X1,X2,X3,X4); }
#  define ERROR5(X1,X2,X3,X4,X5) { fprintf(stderr,"Error:"); fprintf(stderr,X1,X2,X3,X4,X5); }
#  define WARNING1(X1) {  fprintf(stderr,"Warning:"); fprintf(stderr,X1); }
#  define WARNING2(X1,X2) {  fprintf(stderr,"Warning:"); fprintf(stderr,X1,X2); }
#  define WARNING3(X1,X2,X3) {  fprintf(stderr,"Warning:"); fprintf(stderr,X1,X2,X3); }
#  define INFO1(X1) {  fprintf(stderr,"Info:"); fprintf(stderr,X1); }
#  define INFO2(X1,X2) {  fprintf(stderr,"Info:"); fprintf(stderr,X1,X2); }
#  define INFO3(X1,X2,X3) {  fprintf(stderr,"Info:"); fprintf(stderr,X1,X2,X3); }
#  define OPEN_LOG(X)    
#  define CLOSE_LOG()
#define STRDUP ::strdup
#define FREE ::free
#define MALLOC ::malloc
#define REALLOC ::realloc
#define FUNCTION_TRACE 
#define MEMORY_REPORT
#define CALLSTACK
#endif

#endif
