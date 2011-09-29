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

#include "coveragescannerpdef.h"
#include "source.h"
#include "compiler.h"
#include "filename.h"
#include "tmpfile.h"
#include "parsemisc.h"
#include "compilerinterface.h"
#include "c_cpp_libgen.h"
#include "system.h"
#include "fileinfo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

CppLibGen::CppLibGen()
{
  FUNCTION_TRACE;
}

CppLibGen::~CppLibGen()
{
  FUNCTION_TRACE;
}


void CppLibGen::save_source(const char *filename, const CompilerInterface &compiler_wrapper,const char *default_csexe,bool lock_csexe)
{
  FUNCTION_TRACE;
  int i;
  FILE *f;
  char signature_str[CHAINE_LEN];
  char tmp[CHAINE_LEN];
  char indexstr[CHAINE_LEN];
  char *default_csexe_escaped=NULL;
  if (default_csexe)
  {
    default_csexe_escaped = (char*)MALLOC(strlen(default_csexe)*2+1) ;
    escape(default_csexe,default_csexe_escaped);
  }
  DEBUG1("==== begin generating __cs_libgen.c source code ====\n");


  if (filename==NULL)
    f=stdout;
  else
    f=fopen(filename,"w");

  if (f==NULL)
  {
    FATAL2("Could not open %s with write access\n",filename);
  }
  if (nb_data==0)
  {
    WARNING1("No object files compiled with code coverage support\n");
  }
#ifdef OS_WIN32
  if (compiler_wrapper.setupMS() || compiler_wrapper.setupMSCE())
    fputs_trace("#pragma warning(disable : 4996)\n",f);
#endif
  if ( (!compiler_wrapper.customSetup()) || compiler_wrapper.pluginRegistrationFeature() )
  {
    fputs_trace("#include <stdlib.h>\n",f);
    fputs_trace("#include <stdio.h>\n",f);
    fputs_trace("#include <string.h>\n",f);
  }
  else
    fputs_trace("#define NULL 0\n",f);
  if (lock_csexe)
  {
    fputs_trace("#include <time.h>\n",f);
    fputs_trace("#define  CS_TIMEOUT 300\n",f);
  }
  if (lock_csexe)
  {
    if ( (compiler_wrapper.setupMS()) || (compiler_wrapper.setupMSCE()) )
    {
       if (!compiler_wrapper.pluginRegistrationFeature())
          fputs_trace("#include <windows.h>\n",f);
      fputs_trace("#include <process.h>\n",f);
    }
  }
  if (compiler_wrapper.setupGNU() || compiler_wrapper.setupPOSIX())
  {
    fputs_trace("#include <sys/types.h>\n",f);
    fputs_trace("#include <unistd.h>\n",f);
  }
  if (compiler_wrapper.pluginRegistrationFeature())
  {
    if (compiler_wrapper.setupMS())
      fputs_trace("#include <windows.h>\n",f);
    else
      fputs_trace("#include <dlfcn.h>\n",f);
  }
  fputs_trace("#define  CHAINE_LEN 1024\n",f);
  if (compiler_wrapper.pluginRegistrationFeature())
  {
    fputs_trace("typedef struct {\n",f);
    fputs_trace("  char *name;\n",f);
    fputs_trace("  int use_count;\n",f);
    fputs_trace("  void (*__coveragescanner_clear)();\n",f);
    fputs_trace("  void (*__coveragescanner_savecov)(void*);\n",f);
    fputs_trace("  void (*__coveragescanner_set_custom_io)\n",f);
    fputs_trace("       (",f);
    fputs_trace("         char *(*cs_fgets)(char *s, int size, void *stream),",f);
    fputs_trace("         int (*cs_fputs)(const char *s, void *stream),",f);
    fputs_trace("         void *(*cs_fopenappend)(const char *path),",f);
    fputs_trace("         void *(*cs_fopenread)(const char *path),",f);
    fputs_trace("         void *(*cs_fopenwrite)(const char *path),",f);
    fputs_trace("         int (*cs_fclose)(void *fp),",f);
    fputs_trace("         int (*cs_remove)(const char *name)",f);
    fputs_trace("       );\n",f);
    fputs_trace("  int (*__coveragescanner_emptycov)();\n",f);
    if (compiler_wrapper.setupMS())
      fputs_trace("  HMODULE handle;\n",f);
    else
      fputs_trace("  void * handle;\n",f);
    fputs_trace("} __cs_plugins_t;\n",f);
    fputs_trace("static __cs_plugins_t *__cs_plugins=NULL;\n",f);
    fputs_trace("static int __cs_nb_plugins=0;\n",f);
  }

  fputs_trace("static char __cs_appname[CHAINE_LEN+10]=\"",f);
  fputs_trace(default_csexe_escaped,f);
  fputs_trace(".csexe\";\n",f);
  fputs_trace("static char __cs_testname[CHAINE_LEN]=\"\";\n",f);
  fputs_trace("static char __cs_teststate[CHAINE_LEN]=\"\";\n",f);
  fputs_trace("static char *cs_fgets(char *s, int size, void *stream);\n",f);
  fputs_trace("static int cs_remove_defined();\n",f);
  fputs_trace("static int cs_fgets_defined();\n",f);
  fputs_trace("static int cs_fputs(const char *s, void *stream);\n",f);
  fputs_trace("static void *cs_fopenappend(const char *path);\n",f);
  fputs_trace("static void *cs_fopenread(const char *path);\n",f);
  fputs_trace("static void *cs_fopenwrite(const char *path);\n",f);
  fputs_trace("static int cs_fclose(void *fp);\n",f);
  fputs_trace("static void cs_fflush(void *fp);\n",f);
  fputs_trace("static int cs_remove(const char *n);\n",f);

  fputs_trace("#ifdef __cplusplus\n",f);
  fputs_trace("extern \"C\" {\n",f);
  fputs_trace("#endif\n",f);
  for (i=0;i<nb_data;i++)
  {
    char table_name[INSTRUMENTATION_CODE_MAX_LENGTH] ;
    char filename_abs[MAX_PATH];
    realPath(datas[i].filename,filename_abs);
    Source::instrumentation_table(filename_abs,table_name);


    if (datas[i].import_symbols)
      fputs_trace(compiler_wrapper.dll_import(),f);
    else
      fputs_trace(compiler_wrapper.dll_export(),f);
    fputs_trace("void ",f);
    fputs_trace(" ",f);
    fputs_trace(table_name,f);
    fputs_trace("_(int *,volatile int **) ",f);
    fputs_trace(compiler_wrapper.function_attribute(),f);
    fputs_trace(";",f);
    fputs_trace("/* ",f);
    fputs_trace(filename_abs,f);
    fputs_trace(" */\n",f);
  }
  fputs_trace("#ifdef __cplusplus\n",f);
  fputs_trace("}\n",f);
  fputs_trace("#endif\n",f);

  if (compiler_wrapper.customSetup())
  { /* Custom IO */
    fputs_trace("static char *(*__cs_fgets)(char *s, int size, void *stream)=NULL;\n",f);
    fputs_trace("static int (*__cs_fputs)(const char *s, void *stream)=NULL;\n",f);
    fputs_trace("static void *(*__cs_fopenappend)(const char *path)=NULL;\n",f);
    fputs_trace("static void *(*__cs_fopenread)(const char *path)=NULL;\n",f);
    fputs_trace("static void *(*__cs_fopenwrite)(const char *path)=NULL;\n",f);
    fputs_trace("static int (*__cs_fclose)(void *fp)=NULL;\n",f);
    fputs_trace("static int (*__cs_remove)(const char *n)=NULL;\n",f);
  }
  else
  { /* Custom IO */
    fputs_trace("static void * __fopenread(const char *name) { return (void *)fopen(name,\"r\"); }\n",f);
    fputs_trace("static void * __fopenappend(const char *name) { return (void *)fopen(name,\"a+\"); }\n",f);
    fputs_trace("static void * __fopenwrite(const char *name) { return (void *)fopen(name,\"w\"); }\n",f);
    fputs_trace("static char *(*__cs_fgets)(char *s, int size, void *stream)=(char *(*)(char *s, int size, void *stream))fgets;\n",f);
    fputs_trace("static int (*__cs_fputs)(const char *s, void *stream)=(int (*)(const char *s, void *stream))fputs;\n",f);
    fputs_trace("static void *(*__cs_fopenappend)(const char *path)=__fopenappend;\n",f);
    fputs_trace("static void *(*__cs_fopenread)(const char *path)=__fopenread;\n",f);
    fputs_trace("static void *(*__cs_fopenwrite)(const char *path)=__fopenwrite;\n",f);
    fputs_trace("static int (*__cs_fclose)(void *fp)=(int (*)(void *fp))fclose;\n",f);
    if (lock_csexe)
      fputs_trace("static int (*__cs_remove)(const char *n)=remove;\n",f);
    else
      fputs_trace("static int (*__cs_remove)(const char *n)=NULL;\n",f);
  }

  fputs_trace("typedef struct { int size; int signature; const char *name;volatile int *values;} __cs_exec_t ;\n",f);
  fputs_trace("static __cs_exec_t *__cs_exec= NULL;\n",f);

  /* Checksum for lockfile */
  fputs_trace("static int __checksum_over_coverage()\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  int i,item;\n",f);
  fputs_trace("  int chksum=0;\n",f);
  fputs_trace("  for (item=0;__cs_exec[item].name!=NULL;item++)\n",f);
  fputs_trace("  {\n",f);
  fputs_trace("    for (i=0;i<__cs_exec[item].size;i++)\n",f);
  fputs_trace("      {\n",f);
  fputs_trace("          chksum += __cs_exec[item].values[i];\n",f);
  fputs_trace("      }\n",f);
  fputs_trace("  }\n",f);
  fputs_trace("  return chksum;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("\n",f);

  fputs_trace("static char * __cs_strncpy( char *dst, const char *src, int n)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("    if (n != 0) {\n",f);
  fputs_trace("        char *d = dst;\n",f);
  fputs_trace("        const char *s = src;\n",f);
  fputs_trace("        do {\n",f);
  fputs_trace("            if ((*d++ = *s++) == 0) {\n",f);
  fputs_trace("                while (--n != 0)\n",f);
  fputs_trace("                    *d++ = 0;\n",f);
  fputs_trace("                break;\n",f);
  fputs_trace("            }\n",f);
  fputs_trace("        } while (--n != 0);\n",f);
  fputs_trace("    }\n",f);
  fputs_trace("    return (dst);\n",f);
  fputs_trace("}\n",f);

  fputs_trace("static char * __cs_strcpy( char *to, const char *from)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("    char *save = to;\n",f);
  fputs_trace("    for (; (*to = *from) != 0; ++from, ++to);\n",f);
  fputs_trace("    return(save);\n",f);
  fputs_trace("}\n",f);

  fputs_trace("static char * __cs_strcat( char *s, const char *append)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("    char *save = s;\n",f);
  fputs_trace("    for (; *s; ++s);\n",f);
  fputs_trace("    while ((*s++ = *append++) != 0);\n",f);
  fputs_trace("    return(save);\n",f);
  fputs_trace("}\n",f);

  fputs_trace("static int __cs_strlen(const char *str)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  const char *s;\n",f);
  fputs_trace("  for (s = str; *s; ++s);\n",f);
  fputs_trace("  return(s - str);\n",f);
  fputs_trace("}\n",f);

  fputs_trace("static void __cs_int2hex(int v,char h[9])\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  int i;\n",f);
  fputs_trace("  for (i=0;i<8;i++)\n",f);
  fputs_trace("  {\n",f);
  fputs_trace("    char c;\n",f);
  fputs_trace("    c=(v>>(4*(7-i))) & 0xF;\n",f);
  fputs_trace("    if (c<=9)\n",f);
  fputs_trace("      c+='0';\n",f);
  fputs_trace("    else\n",f);
  fputs_trace("      c+='A'-10;\n",f);
  fputs_trace("    h[i]=c;\n",f);
  fputs_trace("  }\n",f);
  fputs_trace("  h[8]='\\0';\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static void __cs_int2asc(unsigned int valeur,char resultat[35])\n",f);
  fputs_trace("{\n",f);
  fputs_trace("   int i,j;\n",f);
  fputs_trace("   char chaine[35];\n",f);
  fputs_trace("   unsigned short int reste;\n",f);
  fputs_trace("   unsigned int count=0;\n",f);
  fputs_trace("   int quotient=valeur;\n",f);
  fputs_trace("   if(quotient==0){\n",f);
  fputs_trace("      chaine[0]='0';\n",f);
  fputs_trace("      count++;\n",f);
  fputs_trace("   }\n",f);
  fputs_trace("   while (quotient!=0){\n",f);
  fputs_trace("     reste = quotient % 10 ;\n",f);
  fputs_trace("     quotient = (int) quotient/10;\n",f);
  fputs_trace("     chaine[count]=(char) (reste+'0');\n",f);
  fputs_trace("     ++count;\n",f);
  fputs_trace("   }\n",f);
  fputs_trace("   for(i = 0, j=count-1 ; i< count ;i++, j--){\n",f);
  fputs_trace("     resultat[j]=chaine[i];\n",f);
  fputs_trace("   }\n",f);
  fputs_trace("   resultat[count]='\\0';\n",f);
  fputs_trace("} \n",f);
  /* init function of the table */
  fputs_trace("static void __cs_sem_init(void)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  static int initialized=0;\n",f);
  fputs_trace("  if (initialized) return;\n",f);
  fputs_trace("  initialized=1;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static int __cs_sem_lock(void)\n",f);
  fputs_trace("{\n",f);
  if (lock_csexe)
  {
    fputs_trace("  time_t start_time;\n",f);
    fputs_trace("  time_t end_time;\n",f);
    fputs_trace("  char lockfile[CHAINE_LEN+20];\n",f);
    fputs_trace("  char str[40];\n",f);
    fputs_trace("  char str2[40];\n",f);
    fputs_trace("  void *f;\n",f);
    fputs_trace("  if (!cs_fgets_defined()) return 1;\n",f);
    fputs_trace("  __cs_strcpy(str,\"cs:\");\n",f);
    fputs_trace("  char tmp9[9];\n",f);
    if (compiler_wrapper.setupMS() || compiler_wrapper.setupMSCE())
    {
      fputs_trace("  __cs_int2hex(GetCurrentProcessId(),tmp9);\n",f);
      fputs_trace("  strcat(str,tmp9);\n",f);
    }
    else if (compiler_wrapper.setupGNU() || compiler_wrapper.setupPOSIX())
    {
      fputs_trace("  __cs_int2hex(getpid(),tmp9);\n",f);
      fputs_trace("  strcat(str,tmp9);\n",f);
    }
    fputs_trace("  strcat(str,\":\");\n",f);
    fputs_trace("  start_time=time(NULL);\n",f);
    fputs_trace("  __cs_int2hex(start_time,tmp9);\n",f);
    fputs_trace("  strcat(str,tmp9);\n",f);
    fputs_trace("  strcat(str,\":\");\n",f);
    fputs_trace("  __cs_int2hex(__checksum_over_coverage(),tmp9);\n",f);
    fputs_trace("  strcat(str,tmp9);\n",f);
    fputs_trace("  __cs_strcpy(lockfile,__cs_appname);\n",f);
    fputs_trace("  strcat(lockfile,\".lck\");\n",f);
    fputs_trace("  for (end_time=time(NULL);(end_time-start_time)<CS_TIMEOUT;end_time=time(NULL))\n",f);
    fputs_trace("  {\n",f);
    fputs_trace("    int canlock=0;\n",f);
    fputs_trace("    if ( (f=cs_fopenread(lockfile)) )\n",f);
    fputs_trace("    {\n",f);
    fputs_trace("      if (cs_fgets(str2,sizeof(str2),f))  \n",f);
    fputs_trace("      {\n",f);
    fputs_trace("        cs_fclose(f);\n",f);
    fputs_trace("        if (strcmp(str2,\"\")==0)\n",f);
    fputs_trace("          canlock=1;\n",f);
    fputs_trace("      }\n",f);
    fputs_trace("    }\n",f);
    fputs_trace("    else\n",f);
    fputs_trace("      canlock=1;\n",f);
    fputs_trace("    if (canlock==1)\n",f);
    fputs_trace("    {\n",f);
    fputs_trace("      if ( (f=cs_fopenwrite(lockfile)) )  \n",f);
    fputs_trace("      {\n",f);
    fputs_trace("        cs_fputs(str,f);  \n",f);
    fputs_trace("        cs_fclose(f);\n",f);
    fputs_trace("        if ( (f=cs_fopenread(lockfile)) )  \n",f);
    fputs_trace("        {\n",f);
    fputs_trace("          if (cs_fgets(str2,sizeof(str2),f))  \n",f);
    fputs_trace("          {\n",f);
    fputs_trace("            cs_fclose(f);\n",f);
    fputs_trace("            if (strcmp(str2,str)==0)\n",f);
    fputs_trace("              return 1;\n",f);
    fputs_trace("          }\n",f);
    fputs_trace("        }\n",f);
    fputs_trace("      }\n",f);
    fputs_trace("    }\n",f);
    fputs_trace("  }\n",f);
    fputs_trace("  return 0;\n",f);
  }
  else
    fputs_trace("  return 1;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static void __cs_sem_unlock(void)\n",f);
  fputs_trace("{\n",f);
  if (lock_csexe)
  {
    if (    compiler_wrapper.setupMS() || compiler_wrapper.setupMSCE()
        || compiler_wrapper.setupGNU() || compiler_wrapper.setupPOSIX())
    {
      fputs_trace("  char lockfile[CHAINE_LEN+20];\n",f);
      fputs_trace("  if (!cs_fgets_defined()) return ;\n",f);
      fputs_trace("  __cs_strcpy(lockfile,__cs_appname);\n",f);
      fputs_trace("  __cs_strcat(lockfile,\".lck\");\n",f);
      fputs_trace("  if (cs_remove_defined())\n",f);
      fputs_trace("    cs_remove(lockfile);\n",f);
      fputs_trace("  else\n",f);
      fputs_trace("  {\n",f);
      fputs_trace("    void *f;\n",f);
      fputs_trace("    if ( (f=cs_fopenwrite(lockfile)) )  \n",f);
      fputs_trace("      cs_fclose(f);\n",f);
      fputs_trace("  }\n",f);
    }
  }
  fputs_trace("}\n",f);
  fputs_trace("static void __cs_custon_io_init(void)\n",f);
  fputs_trace("{\n",f);
  if ( compiler_wrapper.pluginRegistrationFeature() )
  {
    fputs_trace("    int iplg;\n",f);
    fputs_trace("    for (iplg=0;iplg<__cs_nb_plugins;iplg++)\n",f);
    fputs_trace("    {\n",f);
    fputs_trace("      if (__cs_plugins[iplg].__coveragescanner_set_custom_io)\n",f);
    fputs_trace("        __cs_plugins[iplg].__coveragescanner_set_custom_io(__cs_fgets,__cs_fputs,__cs_fopenappend,__cs_fopenread,__cs_fopenwrite,__cs_fclose,__cs_remove);",f);
    fputs_trace("    }\n",f);
  }
  fputs_trace("}\n",f);
  fputs_trace("static void __cs_exec_init(void)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  static const char *filenames[] = {",f);
  for (i=0;i<nb_data;i++)
  {
    char table_name[INSTRUMENTATION_CODE_MAX_LENGTH] ;
    char filename_abs_escape[MAX_PATH*2];
    char filename_abs[MAX_PATH];
    realPath(datas[i].filename,filename_abs);
    Source::instrumentation_table(filename_abs,table_name);
    escape(filename_abs,filename_abs_escape);

    fputs_trace("      \n\"",f);
    fputs_trace(filename_abs_escape,f);
    fputs_trace("\"",f);
    fputs_trace(",",f);
  }
  fputs_trace("\n      NULL\n    };\n",f);
  fputs_trace("  static __cs_exec_t exectab[",f);
  sprintf(tmp,"%lu",nb_data+1);
  fputs_trace(tmp,f);
  fputs_trace("  ];\n\n",f);

  fputs_trace("  if (__cs_exec!=NULL) return ;\n\n",f);
  fputs_trace("  __cs_exec=exectab;\n\n",f);
  for (i=0;i<nb_data;i++)
  {
    char table_name[INSTRUMENTATION_CODE_MAX_LENGTH] ;
    char filename_abs[MAX_PATH];
    realPath(datas[i].filename,filename_abs);
    Source::instrumentation_table(filename_abs,table_name);

    fputs_trace("  /* ",f);
    fputs_trace(filename_abs,f);
    fputs_trace(" */\n",f);
    sprintf(indexstr,"%i",i);
    sprintf(tmp,"  __cs_exec[%i]",i);

    fputs_trace(tmp,f);
    fputs_trace(".signature=",f);
    sprintf(signature_str,"%lu",datas[i].signature);
    fputs_trace(signature_str,f);
    fputs_trace(";\n",f);

    fputs_trace(tmp,f);
    fputs_trace(".name=filenames[",f);
    fputs_trace(indexstr,f);
    fputs_trace("];\n",f);

    fputs_trace("  if ((void*)",f);
    fputs_trace(table_name,f);
    fputs_trace("_)\n",f);
    fputs_trace("     ",f);
    fputs_trace(table_name,f);
    fputs_trace("_(&",f);
    fputs_trace(tmp,f);
    fputs_trace(".size,&",f);
    fputs_trace(tmp,f);
    fputs_trace(".values);\n",f);
    fputs_trace("  else",f);
    fputs_trace("     {",f);
    fputs_trace(tmp,f);
    fputs_trace(".size=0;",f);
    fputs_trace("}\n\n",f);
  }
  sprintf(indexstr,"%i",i);
  sprintf(tmp,"  __cs_exec[%i]",i);
  fputs_trace(tmp,f); fputs_trace(".size=0;\n",f);
  fputs_trace(tmp,f); fputs_trace(".signature=0;\n",f);
  fputs_trace(tmp,f); fputs_trace(".name=NULL;\n",f);
  fputs_trace(tmp,f); fputs_trace(".values=NULL;\n\n",f);
  fputs_trace("}\n",f);

  if (compiler_wrapper.pluginRegistrationFeature())
  {
    /* __coveragescanner_unregister_library */
    fputs_trace("#ifdef __cplusplus\n",f);
    fputs_trace("extern \"C\"\n",f);
    fputs_trace("#endif\n",f);
    fputs_trace(compiler_wrapper.dll_export(),f);
    fputs_trace(" int ",f);
    fputs_trace(" __coveragescanner_unregister_library(const char*)  ",f);
    fputs_trace(compiler_wrapper.function_attribute(),f);
    fputs_trace(" ;\n",f);
    fputs_trace("int ",f);
    fputs_trace(" __coveragescanner_unregister_library(const char*p)\n",f);
    fputs_trace("{\n",f);
    fputs_trace("  int i;\n",f);
    fputs_trace("  int found=0;\n",f);
    fputs_trace("  if (p==NULL) return -2;\n",f);
    fputs_trace("  for (i=0;i<__cs_nb_plugins;i++)\n",f);
    fputs_trace("  {\n",f);
    fputs_trace("    if (strcmp(p,__cs_plugins[i].name)==0)\n",f);
    fputs_trace("    {\n",f);
    fputs_trace("      __cs_plugins[i].use_count--;\n",f);
    fputs_trace("      if (__cs_plugins[i].use_count==0)\n",f);
    fputs_trace("      {\n",f);
    fputs_trace("        free(__cs_plugins[i].name);\n",f);
    if (compiler_wrapper.setupMS())
      fputs_trace("        FreeLibrary(__cs_plugins[i].handle);\n",f);
    else
      fputs_trace("        dlclose(__cs_plugins[i].handle);\n",f);
    fputs_trace("        __cs_nb_plugins--;\n",f);
    fputs_trace("        for (;i<__cs_nb_plugins;i++)\n",f);
    fputs_trace("           __cs_plugins[i]=__cs_plugins[i+1];\n",f);
    fputs_trace("        if (__cs_nb_plugins==0)\n",f);
    fputs_trace("        {\n",f);
    fputs_trace("          free(__cs_plugins);\n",f);
    fputs_trace("          __cs_plugins=NULL;\n",f);
    fputs_trace("        }\n",f);
    fputs_trace("        return 1;\n",f);
    fputs_trace("      }\n",f);
    fputs_trace("      return 0;\n",f);
    fputs_trace("    }\n",f);
    fputs_trace("  }\n",f);
    fputs_trace("  return -1;\n",f);
    fputs_trace("}\n",f);
    fputs_trace("\n",f);

    /* __coveragescanner_register_library */
    fputs_trace("#ifdef __cplusplus\n",f);
    fputs_trace("extern \"C\"\n",f);
    fputs_trace("#endif\n",f);
    fputs_trace(compiler_wrapper.dll_export(),f);
    fputs_trace(" int ",f);
    fputs_trace(" __coveragescanner_register_library(const char*)  ",f);
    fputs_trace(compiler_wrapper.function_attribute(),f);
    fputs_trace(" ;\n",f);
    fputs_trace("int ",f);
    fputs_trace(" __coveragescanner_register_library(const char*p)\n",f);
    fputs_trace("{\n",f);
    fputs_trace("  int i;\n",f);
    fputs_trace("  int found=0;\n",f);
    fputs_trace("  if (p==NULL) return -2;\n",f);
    fputs_trace("  for (i=0;i<__cs_nb_plugins;i++)\n",f);
    fputs_trace("  {\n",f);
    fputs_trace("    if (strcmp(p,__cs_plugins[i].name)==0)\n",f);
    fputs_trace("    {\n",f);
    fputs_trace("      __cs_plugins[i].use_count++;\n",f);
    fputs_trace("      return 1;\n",f);
    fputs_trace("    }\n",f);
    fputs_trace("  }\n",f);
    fputs_trace("  {\n",f);
    if (compiler_wrapper.setupMS())
    {
      fputs_trace("  HMODULE mod;\n",f);
      fputs_trace("  mod=LoadLibrary(p);\n",f);
      fputs_trace("  if (!mod)\n",f);
      fputs_trace("    return -1;\n",f);
    }
    else
    {
      fputs_trace("  void *mod;\n",f);
      fputs_trace("  mod=dlopen(p,RTLD_LAZY);\n",f);
      fputs_trace("  if (!mod)\n",f);
      fputs_trace("    return -1;\n",f);
    }
    fputs_trace("  __cs_nb_plugins++;\n",f);
    fputs_trace("  __cs_plugins=(__cs_plugins_t *)realloc(__cs_plugins,__cs_nb_plugins*sizeof(__cs_plugins_t));\n",f);
    fputs_trace("  __cs_plugins[__cs_nb_plugins-1].name=strdup(p);\n",f);
    fputs_trace("  __cs_plugins[__cs_nb_plugins-1].use_count=1;\n",f);
    fputs_trace("  __cs_plugins[__cs_nb_plugins-1].handle=mod;\n",f);
    if (compiler_wrapper.setupMS())
    {
      fputs_trace("    __cs_plugins[__cs_nb_plugins-1].__coveragescanner_emptycov=(int (*)())GetProcAddress(mod,\"__coveragescanner_emptycov\");\n",f);
      fputs_trace("    __cs_plugins[__cs_nb_plugins-1].__coveragescanner_savecov=(void (*)(void*))GetProcAddress(mod,\"__coveragescanner_savecov\");\n",f);
      fputs_trace("    __cs_plugins[__cs_nb_plugins-1].__coveragescanner_set_custom_io=(void (*)(char* (*)(char*, int, void*), int (*)(const char*, void*), void* (*)(const char*), void* (*)(const char*), void* (*)(const char*), int (*)(void*), int (*)(const char*)))GetProcAddress(mod,\"__coveragescanner_set_custom_io\");\n",f);
      fputs_trace("    __cs_plugins[__cs_nb_plugins-1].__coveragescanner_clear=(void (*)())GetProcAddress(mod,\"__coveragescanner_clear\");\n",f);
    }
    else
    {
      fputs_trace("    __cs_plugins[__cs_nb_plugins-1].__coveragescanner_emptycov=(int (*)()) dlsym(mod,\"__coveragescanner_emptycov\");\n",f);
      fputs_trace("    __cs_plugins[__cs_nb_plugins-1].__coveragescanner_savecov=(void (*)(void*))dlsym(mod,\"__coveragescanner_savecov\");\n",f);
      fputs_trace("    __cs_plugins[__cs_nb_plugins-1].__coveragescanner_set_custom_io=(void (*)(char* (*)(char*, int, void*), int (*)(const char*, void*), void* (*)(const char*), void* (*)(const char*), void* (*)(const char*), int (*)(void*), int (*)(const char*)))dlsym(mod,\"__coveragescanner_set_custom_io\");\n",f);
      fputs_trace("    __cs_plugins[__cs_nb_plugins-1].__coveragescanner_clear=(void (*)())dlsym(mod,\"__coveragescanner_clear\");\n",f);
    }
    fputs_trace("  __cs_custon_io_init();\n",f);
    fputs_trace("  if (!__cs_plugins[__cs_nb_plugins-1].__coveragescanner_emptycov) return 2;\n",f);
    fputs_trace("  if (!__cs_plugins[__cs_nb_plugins-1].__coveragescanner_savecov) return 2;\n",f);
    fputs_trace("  if (!__cs_plugins[__cs_nb_plugins-1].__coveragescanner_clear) return 2;\n",f);
    fputs_trace("  return 0;\n",f);
    fputs_trace("  }\n",f);
    fputs_trace("}\n",f);
    fputs_trace("\n",f);
  }

  /* __coveragescanner_emptycov */
  fputs_trace("#ifdef __cplusplus\n",f);
  fputs_trace("extern \"C\"\n",f);
  fputs_trace("#endif\n",f);
  fputs_trace(compiler_wrapper.dll_export(),f);
  fputs_trace(" int ",f);
  fputs_trace(" __coveragescanner_emptycov(void)  ",f);
  fputs_trace(compiler_wrapper.function_attribute(),f);
  fputs_trace(" ;\n",f);
  fputs_trace("int ",f);
  fputs_trace(" __coveragescanner_emptycov(void)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  int i,item;\n",f);
  fputs_trace("  __cs_exec_init();\n",f);

  if ( compiler_wrapper.pluginRegistrationFeature() )
  {
    fputs_trace("  {\n",f);
    fputs_trace("    int iplg;\n",f);
    fputs_trace("    for (iplg=0;iplg<__cs_nb_plugins;iplg++)\n",f);
    fputs_trace("    {\n",f);
    fputs_trace("      if (__cs_plugins[iplg].__coveragescanner_emptycov)\n",f);
    fputs_trace("      {\n",f);
    fputs_trace("        if ( __cs_plugins[iplg].__coveragescanner_emptycov() == 0 )\n",f);
    fputs_trace("          return 0;\n",f);
    fputs_trace("      }\n",f);
    fputs_trace("    }\n",f);
    fputs_trace("  }\n",f);
  }
  fputs_trace("  for (item=0;__cs_exec[item].name!=NULL;item++)\n",f);
  fputs_trace("  {\n",f);
  fputs_trace("    for (i=0;i<__cs_exec[item].size;i++)\n",f);
  fputs_trace("      {\n",f);
  fputs_trace("          if (__cs_exec[item].values[i]!=0)\n",f);
  fputs_trace("             return 0;\n",f);
  fputs_trace("      }\n",f);
  fputs_trace("  }\n",f);
  fputs_trace("  return 1;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("\n",f);


  /* __coveragescanner_savecov */
  fputs_trace("#ifdef __cplusplus\n",f);
  fputs_trace("extern \"C\"\n",f);
  fputs_trace("#endif\n",f);
  fputs_trace(compiler_wrapper.dll_export(),f);
  fputs_trace(" void ",f);
  fputs_trace(" __coveragescanner_savecov(void *f)  ",f);
  fputs_trace(compiler_wrapper.function_attribute(),f);
  fputs_trace(" ;\n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_savecov(void *f)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  int i,item;\n",f);
  fputs_trace("  char tmp[35];\n",f);

  fputs_trace("  __cs_exec_init();\n",f);
  if ( compiler_wrapper.pluginRegistrationFeature() )
  {
    fputs_trace("  {\n",f);
    fputs_trace("    int iplg;\n",f);
    fputs_trace("    for (iplg=0;iplg<__cs_nb_plugins;iplg++)\n",f);
    fputs_trace("    {\n",f);
    fputs_trace("      if (__cs_plugins[iplg].__coveragescanner_savecov)\n",f);
    fputs_trace("        __cs_plugins[iplg].__coveragescanner_savecov(f);\n",f);
    fputs_trace("    }\n",f);
    fputs_trace("  }\n",f);
  }
  fputs_trace("  for (item=0;__cs_exec[item].name!=NULL;item++)\n",f);
  fputs_trace("  {\n",f);
  fputs_trace("    int empty=1;\n",f);
  fputs_trace("    if (__cs_exec[item].size==0) continue;\n",f);
  fputs_trace("    for (i=0;i<__cs_exec[item].size;i++)\n",f);
  fputs_trace("      {\n",f);
  fputs_trace("          if (__cs_exec[item].values[i]!=0)\n",f);
  fputs_trace("             empty=0;\n",f);
  fputs_trace("      }\n",f);
  fputs_trace("    if (empty) continue;\n",f);
  fputs_trace("    cs_fputs(\"/\",f);\n",f);
  fputs_trace("    __cs_int2asc(__cs_exec[item].size,tmp);\n",f);
  fputs_trace("    cs_fputs(tmp,f);\n",f);
  fputs_trace("    cs_fputs(\":\",f);\n",f);
  fputs_trace("    __cs_int2asc(__cs_exec[item].signature,tmp);\n",f);
  fputs_trace("    cs_fputs(tmp,f);\n",f);
  fputs_trace("    cs_fputs(\":\",f);\n",f);
  fputs_trace("    cs_fputs(__cs_exec[item].name,f);\n",f);
  fputs_trace("    cs_fputs(\"\\n\",f);\n",f);

  fputs_trace("    cs_fputs(\"\\\\\",f);\n",f);

  fputs_trace("    for (i=0;i<__cs_exec[item].size;i++)\n",f);

  fputs_trace("      {\n",f);
  fputs_trace("        {\n",f);
  fputs_trace("          switch (__cs_exec[item].values[i])\n",f);
  fputs_trace("          {\n",f);
  fputs_trace("            case 1:\n",f);
  fputs_trace("              cs_fputs(\"+\",f);\n",f);
  fputs_trace("              break;\n",f);
  fputs_trace("            case 0:\n",f);
  fputs_trace("              cs_fputs(\"-\",f);\n",f);
  fputs_trace("              break;\n",f);
  fputs_trace("            default:\n",f);
  fputs_trace("              {\n",f);
  fputs_trace("                 char tmp9[9];\n",f);
  fputs_trace("                 __cs_int2hex(__cs_exec[item].values[i],tmp9);\n",f);
  fputs_trace("                 cs_fputs(tmp9,f);\n",f);
  fputs_trace("              }\n",f);
  fputs_trace("              break;\n",f);
  fputs_trace("          }\n",f);
  fputs_trace("        }\n",f);
  fputs_trace("        __cs_exec[item].values[i]=0;\n",f);
  fputs_trace("      }\n",f);
  fputs_trace("    cs_fputs(\"\\n\",f);\n",f);
  fputs_trace("  }\n",f);
  fputs_trace("  cs_fflush(f);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("\n",f);

  /* __coveragescanner_save */
  fputs_trace("#ifdef __cplusplus\n",f);
  fputs_trace("extern \"C\"\n",f);
  fputs_trace("#endif\n",f);
  fputs_trace(compiler_wrapper.dll_export(),f);
  fputs_trace(" void ",f);
  fputs_trace(" __coveragescanner_save(void)  ",f);
  fputs_trace(compiler_wrapper.function_attribute(),f);
  fputs_trace(" ;\n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_save(void)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  void *f;\n\n",f);

  fputs_trace("  __cs_sem_init();\n",f);
  fputs_trace("  __cs_exec_init();\n",f);
  fputs_trace("  __cs_custon_io_init();\n",f);
  fputs_trace("  if (__coveragescanner_emptycov()) return ;\n",f);
  fputs_trace("  if (__cs_sem_lock()==0) return ;\n",f);
  fputs_trace("  f=cs_fopenappend(__cs_appname);\n",f);
  fputs_trace("  if (f==NULL) return ;\n",f);

  fputs_trace("  if (__cs_testname[0]!='\\0') {\n",f);
  fputs_trace("    cs_fputs(\"*\",f);\n",f);
  fputs_trace("    cs_fputs(__cs_testname,f);\n",f);
  fputs_trace("    cs_fputs(\"\\n\",f);\n",f);
  fputs_trace("  }\n",f);

  fputs_trace("  cs_fputs(\"# Measurements\\n\",f);\n",f);
  fputs_trace("  cs_fflush(f);\n",f);
  fputs_trace("  __coveragescanner_savecov(f);\n",f);

  /* saving the execution status */
  fputs_trace("  if (__cs_teststate[0]!='\\0') {\n",f);
  fputs_trace("    cs_fputs(\"!\",f);\n",f);
  fputs_trace("    cs_fputs(__cs_teststate,f);\n",f);
  fputs_trace("    cs_fputs(\"\\n\",f);\n",f);
  fputs_trace("    __cs_teststate[0]='\\0';\n",f);
  fputs_trace("  }\n",f);

  fputs_trace("  cs_fclose(f);\n",f);
  fputs_trace("  __cs_sem_unlock();\n",f);
  fputs_trace("}\n",f);
  fputs_trace("\n",f);

  /* __coveragescanner_testname */
  fputs_trace("#ifdef __cplusplus\n",f);
  fputs_trace("extern \"C\"\n",f);
  fputs_trace("#endif\n",f);
  fputs_trace(compiler_wrapper.dll_export(),f);
  fputs_trace(" void ",f);
  fputs_trace(" __coveragescanner_testname(const char *name)  ",f);
  fputs_trace(compiler_wrapper.function_attribute(),f);
  fputs_trace(" ;\n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_testname(const char *name)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __cs_strncpy(__cs_testname,name,CHAINE_LEN);\n",f);
  fputs_trace("  __cs_testname[CHAINE_LEN-1]='\\0';\n",f);
  fputs_trace("}\n",f);

  /* __coveragescanner_filename */
  fputs_trace("#ifdef __cplusplus\n",f);
  fputs_trace("extern \"C\"\n",f);
  fputs_trace("#endif\n",f);
  fputs_trace(compiler_wrapper.dll_export(),f);
  fputs_trace(" void ",f);
  fputs_trace(" __coveragescanner_filename(const char *name)  ",f);
  fputs_trace(compiler_wrapper.function_attribute(),f);
  fputs_trace(" ;\n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_filename(const char *name)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __cs_strncpy(__cs_appname,name,CHAINE_LEN);\n",f);
  fputs_trace("  __cs_appname[CHAINE_LEN-1]='\\0';\n",f);
  fputs_trace("  __cs_strcat(__cs_appname,\".csexe\");\n",f);
  fputs_trace("}\n",f);

  /* __coveragescanner_clear */
  fputs_trace("#ifdef __cplusplus\n",f);
  fputs_trace("extern \"C\"\n",f);
  fputs_trace("#endif\n",f);
  fputs_trace(compiler_wrapper.dll_export(),f);
  fputs_trace(" void ",f);
  fputs_trace(" __coveragescanner_clear()  ",f);
  fputs_trace(compiler_wrapper.function_attribute(),f);
  fputs_trace(" ;\n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_clear()\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  int i,item;\n",f);
  fputs_trace("  __cs_sem_init();\n",f);
  fputs_trace("  __cs_exec_init();\n",f);
  fputs_trace("  __cs_custon_io_init();\n",f);
  if ( compiler_wrapper.pluginRegistrationFeature() )
  {
    fputs_trace("  {\n",f);
    fputs_trace("    int iplg;\n",f);
    fputs_trace("    for (iplg=0;iplg<__cs_nb_plugins;iplg++)\n",f);
    fputs_trace("    {\n",f);
    fputs_trace("      if (__cs_plugins[iplg].__coveragescanner_clear)\n",f);
    fputs_trace("        __cs_plugins[iplg].__coveragescanner_clear();\n",f);
    fputs_trace("    }\n",f);
    fputs_trace("  }\n",f);
  }
  fputs_trace("  for (item=0;__cs_exec[item].values!=NULL;item++)\n",f);
  fputs_trace("    for (i=0;i<__cs_exec[item].size;i++)\n",f);
  fputs_trace("        __cs_exec[item].values[i]=0;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("\n",f);

  /* __coveragescanner_teststate */
  fputs_trace("#ifdef __cplusplus\n",f);
  fputs_trace("extern \"C\"\n",f);
  fputs_trace("#endif\n",f);
  fputs_trace(compiler_wrapper.dll_export(),f);
  fputs_trace(" void ",f);
  fputs_trace(" __coveragescanner_teststate(const char *state)  ",f);
  fputs_trace(compiler_wrapper.function_attribute(),f);
  fputs_trace(" ;\n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_teststate(const char *state)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __cs_strncpy(__cs_teststate,state,CHAINE_LEN);\n",f);
  fputs_trace("  __cs_teststate[CHAINE_LEN-1]='\\0';\n",f);
  fputs_trace("}\n",f);

  if (!compiler_wrapper.customSetup())
  {
    if (compiler_wrapper.setupGNU())
    {
      fputs_trace("static void __cs_init(void) __attribute__ ((constructor));\n",f);
      fputs_trace("static void __cs_exit_default(void) __attribute__ ((destructor));\n",f);
    }
    fputs_trace("static int __cs_default_exit=1;\n",f);
    fputs_trace("static void __cs_exit(void)\n",f);
    fputs_trace("{\n",f);
    fputs_trace("  __coveragescanner_save();\n",f);
    fputs_trace("}\n",f);
    fputs_trace("static void __cs_exit_default(void)\n",f);
    fputs_trace("{\n",f);
    fputs_trace("  if (__cs_default_exit)\n",f);
    fputs_trace("    __cs_exit();\n",f);
    fputs_trace("}\n",f);
    fputs_trace("static void __cs_init(void)\n",f);
    fputs_trace("{\n",f);
    fputs_trace("  __coveragescanner_filename(\"",f);
    fputs_trace(default_csexe_escaped,f);
    fputs_trace("\");\n",f);
    fputs_trace("}\n",f);

    if (compiler_wrapper.setupCpp() || compiler_wrapper.setupMS() || compiler_wrapper.setupMSCE())
    {
      fputs_trace("#ifdef __cplusplus\n",f);
      fputs_trace("class __cs_lib_t {\n",f);
      fputs_trace("  public:\n",f);
      fputs_trace("  __cs_lib_t()\n",f);
      fputs_trace("    {\n",f);
      fputs_trace("       __cs_init();\n",f);
      fputs_trace("    }\n",f);
      fputs_trace("  ~__cs_lib_t()\n",f);
      fputs_trace("    {\n",f);
      fputs_trace("       __cs_exit_default();\n",f);
      fputs_trace("    }\n",f);
      fputs_trace("};\n",f);
      fputs_trace("static __cs_lib_t __cs_lib;\n",f);
      fputs_trace("#endif\n",f);
    }
    if (compiler_wrapper.setupMSCE())
    {
      fputs_trace("#ifdef __cplusplus\n",f);
      fputs_trace("extern \"C\"\n",f);
      fputs_trace("#endif\n",f);
      fputs_trace(compiler_wrapper.dll_export(),f);
      fputs_trace(" void ",f);
      fputs_trace(" __coveragescanner_install(const char *name)  ",f);
      fputs_trace(compiler_wrapper.function_attribute(),f);
      fputs_trace(" ;\n",f);
      fputs_trace("void ",f);
      fputs_trace(" __coveragescanner_install(const char *name)\n",f);
      fputs_trace("{\n",f);
      fputs_trace("  __coveragescanner_filename(name);\n",f);
      fputs_trace("}\n",f);
    }
    else
    {
      fputs_trace("#include <signal.h>\n",f);

      fputs_trace("static void sighandler(int sig)\n",f);
      fputs_trace("{\n",f);
      fputs_trace("volatile static int fatal_error_in_progress = 0;\n",f);
      fputs_trace("if (fatal_error_in_progress)\n",f);
      fputs_trace("raise (sig);\n",f);
      fputs_trace("fatal_error_in_progress = 1;\n",f);
      fputs_trace("__coveragescanner_save();\n",f);
      fputs_trace("signal (sig, SIG_DFL);\n",f);
      fputs_trace("fatal_error_in_progress = 0;\n",f);
      fputs_trace("raise (sig);\n",f);
      fputs_trace("}\n",f);

      fputs_trace("#ifdef __cplusplus\n",f);
      fputs_trace("extern \"C\"\n",f);
      fputs_trace("#endif\n",f);
      fputs_trace(compiler_wrapper.dll_export(),f);
      fputs_trace(" void ",f);
      fputs_trace(" __coveragescanner_install(const char *name)  ",f);
      fputs_trace(compiler_wrapper.function_attribute(),f);
      fputs_trace(" ;\n",f);
      fputs_trace("void ",f);
      fputs_trace(" __coveragescanner_install(const char *name)\n",f);
      fputs_trace("{\n",f);
      fputs_trace("  __coveragescanner_filename(name);\n",f);
      fputs_trace("  __cs_default_exit=0;\n",f);
      fputs_trace("  atexit(__cs_exit);\n",f);
      fputs_trace("  signal(SIGABRT,sighandler);\n",f);
      fputs_trace("  signal(SIGTERM,sighandler);\n",f);
      fputs_trace("  signal(SIGFPE,sighandler);\n",f);
      fputs_trace("  signal(SIGILL,sighandler);\n",f);
      fputs_trace("  signal(SIGINT,sighandler);\n",f);
      fputs_trace("  signal(SIGSEGV,sighandler);\n",f);
      fputs_trace("  signal(SIGTERM,sighandler);\n",f);
      fputs_trace("}\n",f);
    }
  }
  fputs_trace("static char __out_buffer[1024]=\"\";\n",f);
  fputs_trace("static int cs_remove_defined()\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  return __cs_remove!=NULL;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static int cs_fgets_defined()\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  return __cs_fgets!=NULL;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static char *cs_fgets(char *s, int size, void *stream)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  return __cs_fgets(s,size,stream);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static int cs_fputs(const char *s, void *stream)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  int lg=__cs_strlen(s);\n",f);
  fputs_trace("  int __out_buffer_lg=__cs_strlen(__out_buffer);\n",f);
  fputs_trace("  if  (lg+__out_buffer_lg>=(int)sizeof(__out_buffer))\n",f);
  fputs_trace("  {\n",f);
  fputs_trace("    if (__out_buffer[0]!='\\0') __cs_fputs(__out_buffer,stream);\n",f);
  fputs_trace("    __out_buffer[0]='\\0';\n",f);
  fputs_trace("  }\n",f);
  fputs_trace("  if  (lg>=(int)sizeof(__out_buffer))\n",f);
  fputs_trace("  {\n",f);
  fputs_trace("    return __cs_fputs(s,stream);\n",f);
  fputs_trace("  }\n",f);
  fputs_trace("  __cs_strcat(__out_buffer,s);\n",f);
  fputs_trace("  return lg;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static void *cs_fopenappend(const char *path)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __out_buffer[0]='\\0';\n",f);
  fputs_trace("  return __cs_fopenappend(path);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static int cs_remove(const char *path)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  return __cs_remove(path);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static void *cs_fopenread(const char *path)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __out_buffer[0]='\\0';\n",f);
  fputs_trace("  return __cs_fopenread(path);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static void *cs_fopenwrite(const char *path)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __out_buffer[0]='\\0';\n",f);
  fputs_trace("  return __cs_fopenwrite(path);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static void cs_fflush(void *fp)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  if (__out_buffer[0]!='\\0') __cs_fputs(__out_buffer,fp);\n",f);
  fputs_trace("  __out_buffer[0]='\\0';\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static int cs_fclose(void *fp)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  cs_fflush(fp);\n",f);
  fputs_trace("  return __cs_fclose(fp);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("#ifdef __cplusplus\n",f);
  fputs_trace("extern \"C\" \n",f);
  fputs_trace("#endif\n",f);
  fputs_trace(compiler_wrapper.dll_export(),f);
  fputs_trace(" void ",f);
  fputs_trace(" __coveragescanner_set_custom_io(",f);
  fputs_trace("char *(*cs_fgets)(char *s, int size, void *stream),",f);
  fputs_trace("int (*cs_fputs)(const char *s, void *stream),",f);
  fputs_trace("void *(*cs_fopenappend)(const char *path),",f);
  fputs_trace("void *(*cs_fopenread)(const char *path),",f);
  fputs_trace("void *(*cs_fopenwrite)(const char *path),",f);
  fputs_trace("int (*cs_fclose)(void *fp),",f);
  fputs_trace("int (*cs_remove)(const char *name)",f);
  fputs_trace(") ",f);
  fputs_trace(compiler_wrapper.function_attribute(),f);
  fputs_trace(" ;\n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_set_custom_io(",f);
  fputs_trace("char *(*cs_fgets)(char *s, int size, void *stream),",f);
  fputs_trace("int (*cs_fputs)(const char *s, void *stream),",f);
  fputs_trace("void *(*cs_fopenappend)(const char *path),",f);
  fputs_trace("void *(*cs_fopenread)(const char *path),",f);
  fputs_trace("void *(*cs_fopenwrite)(const char *path),",f);
  fputs_trace("int (*cs_fclose)(void *fp),",f);
  fputs_trace("int (*cs_remove)(const char *name)",f);
  fputs_trace(")\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __cs_fgets=cs_fgets;\n",f);
  fputs_trace("  __cs_fputs=cs_fputs;\n",f);
  fputs_trace("  __cs_fopenappend=cs_fopenappend;\n",f);
  fputs_trace("  __cs_fopenread=cs_fopenread;\n",f);
  fputs_trace("  __cs_fopenwrite=cs_fopenwrite;\n",f);
  fputs_trace("  __cs_fclose=cs_fclose;\n",f);
  fputs_trace("  __cs_remove=cs_remove;\n",f);
  fputs_trace("  __cs_custon_io_init();\n",f);
  fputs_trace("}\n",f);


  if (filename!=NULL)
    fclose(f);
  DEBUG1("==== end generating __cs_libgen.c source code ====\n");
  FREE(default_csexe_escaped);
}

#ifdef LOG
void CppLibGen::PDEBUG() const
{
  FUNCTION_TRACE;
}
#endif


std::string CppLibGen::csmes_library_source_name() const
{
  FUNCTION_TRACE;
  return TmpFile::object().generate("__cs_library_$ID$.c",true);
}

