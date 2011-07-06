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
#include "tmpfile.h"
#include "compiler.h"
#include "filename.h"
#include "parsemisc.h"
#include "compilerinterface.h"
#include "cs_libgen.h"
#include "system.h"
#include "fileinfo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

CsLibGen::CsLibGen() :LibGen()
{
  FUNCTION_TRACE;
}

CsLibGen::~CsLibGen()
{
  FUNCTION_TRACE;
}


void CsLibGen::save_source(const char *filename, const CompilerInterface &compiler_wrapper,const char *default_csexe,bool lock_csexe)
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
  DEBUG1("==== begin generating __cs_libgen.cs source code ====\n");

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
  fputs_trace("#pragma warning disable\n",f);
  fputs_trace("using System;\n",f);
  fputs_trace("using System.IO;\n",f);
  fputs_trace("using System.Text;\n",f);
  if (compiler_wrapper.setupMS())
    fputs_trace("using System.Runtime.InteropServices;\n",f);
  fputs_trace("class CoverageScanner\n",f);
  fputs_trace("{\n",f);
  if (lock_csexe)
  {
    fputs_trace("const int  CS_TIMEOUT=3000;\n",f);
  }
  fputs_trace("static string __cs_appname=\"",f);
  fputs_trace(default_csexe_escaped,f);
  fputs_trace(".csexe\";\n",f);
  fputs_trace("static System.IO.Stream __fopenread(string name) { return new FileStream(name,FileMode.Create,FileAccess.Read); }\n",f);
  fputs_trace("static System.IO.Stream __fopenappend(string name) { return new FileStream(name,FileMode.Append,FileAccess.Write); }\n",f);
  fputs_trace("static System.IO.Stream __fopenwrite(string name) { return new FileStream(name,FileMode.Create,FileAccess.Write); }\n",f);
  fputs_trace("static void __remove(string name) { System.IO.File.Delete(name); }\n",f);
  fputs_trace("static void __fputs(string data,System.IO.Stream stream) { System.Text.ASCIIEncoding  encoding=new System.Text.ASCIIEncoding();stream.Write(encoding.GetBytes(data),0,data.Length); }\n",f);
  fputs_trace("static string __fgets(System.IO.Stream stream) \n",f);
  fputs_trace(" { byte[] bytes= new byte[1024];  int numBytesToRead=1024; int numBytesRead=0; int n=stream.Read(bytes,numBytesRead,numBytesToRead); return Encoding.ASCII.GetString(bytes,0,n);}\n",f);
  fputs_trace("static void __fclose(System.IO.Stream stream) { stream.Close(); }\n",f);
  fputs_trace("static string __cs_testname=\"\";\n",f);
  fputs_trace("static string __cs_teststate=\"\";\n",f);

  /* Custom IO delegates */
  fputs_trace("public delegate string __cs_fgets_delegate(System.IO.Stream stream);\n",f);
  fputs_trace("public delegate void __cs_fputs_delegate(string s, System.IO.Stream stream);\n",f);
  fputs_trace("public delegate System.IO.Stream __cs_fopenappend_delegate(string path);\n",f);
  fputs_trace("public delegate System.IO.Stream __cs_fopenread_delegate(string path);\n",f);
  fputs_trace("public delegate System.IO.Stream __cs_fopenwrite_delegate(string path);\n",f);
  fputs_trace("public delegate void __cs_fclose_delegate(System.IO.Stream fp);\n",f);
  fputs_trace("public delegate void __cs_remove_delegate(string n);\n",f);
  /* Custom IO */
  fputs_trace("static __cs_fgets_delegate __cs_fgets =  new __cs_fgets_delegate(__fgets);\n",f);
  fputs_trace("static __cs_fputs_delegate __cs_fputs =  new __cs_fputs_delegate(__fputs);\n",f);
  fputs_trace("static __cs_fopenappend_delegate __cs_fopenappend =  new __cs_fopenappend_delegate(__fopenappend);\n",f);
  fputs_trace("static __cs_fopenread_delegate __cs_fopenread =  new __cs_fopenread_delegate(__fopenread);\n",f);
  fputs_trace("static __cs_fopenwrite_delegate __cs_fopenwrite =  new __cs_fopenwrite_delegate(__fopenwrite);\n",f);
  fputs_trace("static __cs_fclose_delegate __cs_fclose =  new __cs_fclose_delegate(__fclose);\n",f);
  fputs_trace("static __cs_remove_delegate __cs_remove =  new __cs_remove_delegate(__remove);\n",f);

  fputs_trace("static string __out_buffer=\"\";\n",f);
  fputs_trace("static string cs_fgets(System.IO.Stream stream)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  return __cs_fgets(stream);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static void cs_fputs(string s, System.IO.Stream stream)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  const int max_lg=1024;\n",f);
  fputs_trace("  int lg=s.Length;\n",f);
  fputs_trace("  int __out_buffer_lg=__out_buffer.Length;\n",f);
  fputs_trace("  if  (lg+__out_buffer_lg>=max_lg)\n",f);
  fputs_trace("  {\n",f);
  fputs_trace("    if (__out_buffer_lg>0) __cs_fputs(__out_buffer,stream);\n",f);
  fputs_trace("    __out_buffer=\"\";\n",f);
  fputs_trace("  }\n",f);
  fputs_trace("  if  (lg>=max_lg)\n",f);
  fputs_trace("  {\n",f);
  fputs_trace("    __cs_fputs(s,stream);\n",f);
  fputs_trace("    return ;\n",f);
  fputs_trace("  }\n",f);
  fputs_trace("  __out_buffer+=s;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static System.IO.Stream cs_fopenappend(string path)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __out_buffer=\"\";\n",f);
  fputs_trace("  return __cs_fopenappend(path);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static System.IO.Stream cs_fopenread(string path)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __out_buffer=\"\";\n",f);
  fputs_trace("  return __cs_fopenread(path);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static System.IO.Stream cs_fopenwrite(string path)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __out_buffer=\"\";\n",f);
  fputs_trace("  return __cs_fopenwrite(path);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static void cs_fclose(System.IO.Stream fp)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  if (__out_buffer.Length!=0) __cs_fputs(__out_buffer,fp);\n",f);
  fputs_trace("  __out_buffer=\"\";\n",f);
  fputs_trace("  __cs_fclose(fp);\n",f);
  fputs_trace("}\n",f);
  fputs_trace("public static \n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_set_custom_io(",f);
  fputs_trace("__cs_fgets_delegate cs_fgets,\n",f);
  fputs_trace("__cs_fputs_delegate cs_fputs,\n",f);
  fputs_trace("__cs_fopenappend_delegate cs_fopenappend,\n",f);
  fputs_trace("__cs_fopenread_delegate cs_fopenread,\n",f);
  fputs_trace("__cs_fopenwrite_delegate cs_fopenwrite,\n",f);
  fputs_trace("__cs_fclose_delegate cs_fclose,\n",f);
  fputs_trace("__cs_remove_delegate cs_remove\n",f);
  fputs_trace(")\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __cs_fgets=cs_fgets;\n",f);
  fputs_trace("  __cs_fputs=cs_fputs;\n",f);
  fputs_trace("  __cs_fopenappend=cs_fopenappend;\n",f);
  fputs_trace("  __cs_fopenread=cs_fopenread;\n",f);
  fputs_trace("  __cs_fopenwrite=cs_fopenwrite;\n",f);
  fputs_trace("  __cs_fclose=cs_fclose;\n",f);
  fputs_trace("  __cs_remove=cs_remove;\n",f);
  fputs_trace("}\n",f);

  fputs_trace("struct __cs_exec_t { public int size; public int signature; public string name;public int []values;} ;\n",f);
  fputs_trace("static __cs_exec_t []__cs_exec= null;\n",f);

  /* Checksum for lockfile */
  fputs_trace("static int __checksum_over_coverage()\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  int i,item;\n",f);
  fputs_trace("  int chksum=0;\n",f);
  fputs_trace("  for (item=0;__cs_exec[item].name!=null;item++)\n",f);
  fputs_trace("  {\n",f);
  fputs_trace("    for (i=0;i<__cs_exec[item].size;i++)\n",f);
  fputs_trace("      {\n",f);
  fputs_trace("          chksum += __cs_exec[item].values[i];\n",f);
  fputs_trace("      }\n",f);
  fputs_trace("  }\n",f);
  fputs_trace("  return chksum;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("\n",f);

  /* init function of the table */
  fputs_trace("static string __cs_int2hex(int v)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  string h=\"\";\n",f);
  fputs_trace("  int i;\n",f);
  fputs_trace("  for (i=0;i<8;i++)\n",f);
  fputs_trace("  {\n",f);
  fputs_trace("    char c;\n",f);
  fputs_trace("    c=(char)((v>>(4*(7-i))) & 0xF);\n",f);
  fputs_trace("    if (c<=9)\n",f);
  fputs_trace("      c+='0';\n",f);
  fputs_trace("    else\n",f);
  fputs_trace("      c+=(char)('A'-10);\n",f);
  fputs_trace("    h+=c;\n",f);
  fputs_trace("  }\n",f);
  fputs_trace("  return h;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static void __cs_sem_init()\n",f);
  fputs_trace("{\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static int __cs_sem_lock()\n",f);
  fputs_trace("{\n",f);
  if (lock_csexe)
  {
    fputs_trace("  System.DateTime start_time=System.DateTime.Now;\n",f);
    fputs_trace("  System.DateTime end_time;\n",f);
    fputs_trace("  string lockfile;\n",f);
    fputs_trace("  string str;\n",f);
    fputs_trace("  string str2;\n",f);
    fputs_trace("  System.IO.Stream f=null;\n\n",f);
    fputs_trace("  if (__cs_fgets==null) return 1;\n",f);
    fputs_trace("  str=\"cs:\";\n",f);
    fputs_trace("  str+=__cs_int2hex(System.Diagnostics.Process.GetCurrentProcess().Id);\n",f);
    fputs_trace("  str+=\":\";\n",f);
    fputs_trace("  str+=__cs_int2hex(start_time.Second*1000+start_time.Minute*60000+start_time.Millisecond);\n",f);
    fputs_trace("  str+=\":\";\n",f);
    fputs_trace("  str+=__cs_int2hex(__checksum_over_coverage());\n",f);
    fputs_trace("  lockfile=__cs_appname;\n",f);
    fputs_trace("  lockfile+=\".lck\";\n",f);
    fputs_trace("  start_time=System.DateTime.Now;\n",f);
    fputs_trace("  for (end_time=start_time;(end_time-start_time).Seconds<CS_TIMEOUT;end_time=System.DateTime.Now)\n",f);
    fputs_trace("  {\n",f);
    fputs_trace("    int canlock=0;\n",f);
    fputs_trace("    try {\n",f);
    fputs_trace("      f=cs_fopenread(lockfile);\n",f);
    fputs_trace("    } \n",f);
    fputs_trace("    catch \n",f);
    fputs_trace("    {\n",f);
    fputs_trace("      canlock=1;\n",f);
    fputs_trace("      f=null;\n",f);
    fputs_trace("    }\n",f);
    fputs_trace("    if (f!=null)\n",f);
    fputs_trace("    {\n",f);
    fputs_trace("      try {\n",f);
    fputs_trace("        f=cs_fopenread(lockfile);\n",f);
    fputs_trace("        str2=cs_fgets(f);  \n",f);
    fputs_trace("        cs_fclose(f);\n",f);
    fputs_trace("        if (str2==\"\")\n",f);
    fputs_trace("          canlock=1;\n",f);
    fputs_trace("      } catch {}\n",f);
    fputs_trace("    } \n",f);
    fputs_trace("    if (canlock==1)\n",f);
    fputs_trace("    {\n",f);
    fputs_trace("      try \n",f);
    fputs_trace("      {\n",f);
    fputs_trace("        f=cs_fopenwrite(lockfile);  \n",f);
    fputs_trace("        cs_fputs(str,f);  \n",f);
    fputs_trace("        cs_fclose(f);\n",f);
    fputs_trace("        f=cs_fopenread(lockfile);  \n",f);
    fputs_trace("        str2=cs_fgets(f);  \n",f);
    fputs_trace("        cs_fclose(f);\n",f);
    fputs_trace("        if (str2==str)\n",f);
    fputs_trace("          return 1;\n",f);
    fputs_trace("      } catch {}\n",f);
    fputs_trace("    }\n",f);
    fputs_trace("  }\n",f);
    fputs_trace("  return 0;\n",f);
  }
  else
    fputs_trace("  return 1;\n",f);
  fputs_trace("}\n",f);
  fputs_trace("static void __cs_sem_unlock()\n",f);
  fputs_trace("{\n",f);
  if (lock_csexe)
  {
    fputs_trace("try \n",f);
    fputs_trace("{\n",f);
    fputs_trace("  string lockfile;\n",f);
    fputs_trace("  if (__cs_fgets==null) return ;\n",f);
    fputs_trace("  lockfile=__cs_appname;\n",f);
    fputs_trace("  lockfile+=\".lck\";\n",f);
    fputs_trace("  if (__cs_remove!=null)\n",f);
    fputs_trace("    __cs_remove(lockfile);\n",f);
    fputs_trace("  else\n",f);
    fputs_trace("  {\n",f);
    fputs_trace("    System.IO.Stream f;\n\n",f);
    fputs_trace("    f=cs_fopenwrite(lockfile);  \n",f);
    fputs_trace("    cs_fclose(f);\n",f);
    fputs_trace("  }\n",f);
    fputs_trace("} catch {}\n",f);
  }
  fputs_trace("}\n",f);
  fputs_trace("static void __cs_exec_init()\n",f);
  fputs_trace("{\n",f);
  fputs_trace("try \n",f);
  fputs_trace("{\n",f);

  fputs_trace("  if (__cs_exec!=null) return ;\n\n",f);
  fputs_trace("  __cs_exec_t []exectab = new __cs_exec_t[",f);
  sprintf(tmp,"%lu",nb_data+1);
  fputs_trace(tmp,f);
  fputs_trace("  ];\n\n",f);
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
    fputs_trace(".name=",f);
    fputs_trace("\"",f);
    {
      char table_name[INSTRUMENTATION_CODE_MAX_LENGTH] ;
      char filename_abs_escape[MAX_PATH*2];
      char filename_abs[MAX_PATH];
      realPath(datas[i].filename,filename_abs);
      Source::instrumentation_table(filename_abs,table_name);
      escape(filename_abs,filename_abs_escape);
      fputs_trace(filename_abs_escape,f);
    }
    fputs_trace("\";\n",f);

    fputs_trace(tmp,f);
    fputs_trace(".values=",f);
    fputs_trace(table_name,f);
    fputs_trace(".val;\n",f);

    fputs_trace(tmp,f);
    fputs_trace(".size=",f);
    fputs_trace(table_name,f);
    fputs_trace(".nb;\n",f);
  }
  sprintf(tmp,"  __cs_exec[%i]",i);
  fputs_trace(tmp,f);
  fputs_trace(".signature=0;\n",f);

  fputs_trace(tmp,f);
  fputs_trace(".name=null;\n",f);

  fputs_trace(tmp,f);
  fputs_trace(".values=null;\n",f);

  fputs_trace(tmp,f);
  fputs_trace(".size=0;\n",f);
  fputs_trace("} catch {}\n",f);
  fputs_trace("}\n",f);


  /* __coveragescanner_save */
  fputs_trace("public static \n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_save()\n",f);
  fputs_trace("{\n",f);
  fputs_trace("try \n",f);
  fputs_trace("{\n",f);
  fputs_trace("  int i,item;\n",f);
  fputs_trace("  System.IO.Stream f;\n\n",f);

  fputs_trace("  __cs_sem_init();\n",f);
  fputs_trace("  __cs_exec_init();\n",f);
  fputs_trace("  if (__cs_sem_lock()==0) return ;\n",f);
  fputs_trace("  f=cs_fopenappend(__cs_appname);\n",f);
  fputs_trace("  if (f==null) return ;\n",f);

  fputs_trace("  try {\n",f);
  fputs_trace("  if (__cs_testname!=\"\") {\n",f);
  fputs_trace("    cs_fputs(\"*\",f);\n",f);
  fputs_trace("    cs_fputs(__cs_testname,f);\n",f);
  fputs_trace("    cs_fputs(\"\\n\",f);\n",f);
  fputs_trace("  }\n",f);

  fputs_trace("  cs_fputs(\"# Measurements\\n\",f);\n",f);

  /* Recording code */
  fputs_trace("  for (item=0;__cs_exec[item].name!=null;item++)\n",f);
  fputs_trace("  {\n",f);
  fputs_trace("    if (__cs_exec[item].size==0) continue;\n",f);
  fputs_trace("    bool empty=true;\n",f);
  fputs_trace("    for (i=0;i<__cs_exec[item].size;i++)\n",f);
  fputs_trace("      {\n",f);
  fputs_trace("          if (__cs_exec[item].values[i]!=0)\n",f);
  fputs_trace("             empty=false;\n",f);
  fputs_trace("      }\n",f);
  fputs_trace("    if (empty) continue;\n",f);
  fputs_trace("    cs_fputs(\"/\",f);\n",f);
  fputs_trace("    cs_fputs(Convert.ToString(__cs_exec[item].size),f);\n",f);
  fputs_trace("    cs_fputs(\":\",f);\n",f);
  fputs_trace("    cs_fputs(Convert.ToString(__cs_exec[item].signature),f);\n",f);
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
  fputs_trace("              cs_fputs(__cs_int2hex(__cs_exec[item].values[i]),f);\n",f);
  fputs_trace("              break;\n",f);
  fputs_trace("          }\n",f);
  fputs_trace("        }\n",f);
  fputs_trace("        __cs_exec[item].values[i]=0;\n",f);
  fputs_trace("      }\n",f);
  fputs_trace("    cs_fputs(\"\\n\",f);\n",f);
  fputs_trace("  }\n",f);

  /* saving the execution statue */
  fputs_trace("  if (__cs_teststate!=\"\") {\n",f);
  fputs_trace("    cs_fputs(\"!\",f);\n",f);
  fputs_trace("    cs_fputs(__cs_teststate,f);\n",f);
  fputs_trace("    cs_fputs(\"\\n\",f);\n",f);
  fputs_trace("    __cs_teststate=\"\";\n",f);
  fputs_trace("  }\n",f);

  fputs_trace("  cs_fclose(f);\n",f);
  fputs_trace("  } catch  { cs_fclose(f); }\n",f);
  fputs_trace("  __cs_sem_unlock();\n",f);
  fputs_trace("} catch {}\n",f);
  fputs_trace("}\n",f);
  fputs_trace("\n",f);

  /* __coveragescanner_testname */
  fputs_trace("public static \n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_testname(string name)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __cs_testname=name;\n",f);
  fputs_trace("}\n",f);

  /* __coveragescanner_filename */
  fputs_trace("public static \n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_filename(string name)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("try \n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __cs_appname=name;\n",f);
  fputs_trace("  __cs_appname+=\".csexe\";\n",f);
  fputs_trace("} catch {}\n",f);
  fputs_trace("}\n",f);

  /* __coveragescanner_clear */
  fputs_trace("public static \n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_clear()\n",f);
  fputs_trace("{\n",f);
  fputs_trace("try \n",f);
  fputs_trace("{\n",f);
  fputs_trace("  int i,item;\n",f);
  fputs_trace("  __cs_sem_init();\n",f);
  fputs_trace("  __cs_exec_init();\n",f);
  fputs_trace("  for (item=0;__cs_exec[item].values!=null;item++)\n",f);
  fputs_trace("    for (i=0;i<__cs_exec[item].size;i++)\n",f);
  fputs_trace("        __cs_exec[item].values[i]=0;\n",f);
  fputs_trace("} catch {}\n",f);
  fputs_trace("}\n",f);
  fputs_trace("\n",f);

  /* __coveragescanner_teststate */
  fputs_trace("public static \n",f);
  fputs_trace("void ",f);
  fputs_trace(" __coveragescanner_teststate(string state)\n",f);
  fputs_trace("{\n",f);
  fputs_trace("  __cs_teststate=state;\n",f);
  fputs_trace("}\n",f);

  if (!compiler_wrapper.customSetup())
  {
    fputs_trace("static bool __cs_default_exit=true;\n",f);
    fputs_trace("static void __cs_exit()\n",f);
    fputs_trace("{\n",f);
    fputs_trace("  __coveragescanner_save();\n",f);
    fputs_trace("}\n",f);
    fputs_trace("static void __cs_exit_default()\n",f);
    fputs_trace("{\n",f);
    fputs_trace("  if (__cs_default_exit)\n",f);
    fputs_trace("    __cs_exit();\n",f);
    fputs_trace("}\n",f);
    fputs_trace("static void __cs_init()\n",f);
    fputs_trace("{\n",f);
    fputs_trace("  try {\n",f);
    fputs_trace("  __coveragescanner_filename(System.Environment.GetCommandLineArgs()[0]);\n",f);
    fputs_trace("  } catch\n",f);
    fputs_trace("  {__coveragescanner_filename(\"",f);
    fputs_trace(default_csexe_escaped,f);
    fputs_trace("\");}\n",f);
    fputs_trace("}\n",f);
  }
  fputs_trace("public class __cs_lib_t {\n",f);
  fputs_trace("  public",f);
  fputs_trace("  __cs_lib_t()\n",f);
  fputs_trace("    {\n",f);
  if (!compiler_wrapper.customSetup())
    fputs_trace("       CoverageScanner.__cs_init();\n",f);
  fputs_trace("    }\n",f);
  if (!compiler_wrapper.customSetup())
  {
    fputs_trace("  ~__cs_lib_t()\n",f);
    fputs_trace("    {\n",f);
    fputs_trace("       CoverageScanner.__cs_exit();\n",f);
    fputs_trace("    }\n",f);
  }
  fputs_trace("};\n",f);
  fputs_trace("}\n",f);

  if (filename!=NULL)
    fclose(f);
  DEBUG1("==== end generating __cs_libgen.cs source code ====\n");
  FREE(default_csexe_escaped);
}

#ifdef LOG
void CsLibGen::PDEBUG() const
{
  FUNCTION_TRACE;
}
#endif


std::string CsLibGen::csmes_library_source_name() const
{
  FUNCTION_TRACE;
  return TmpFile::object().generate("__cs_library_$ID$.cs",true);
}

