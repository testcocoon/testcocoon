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

#ifndef ___SYSTEM_H
#define ___SYSTEM_H
#include <stdio.h>
#if defined(OS_LINUX) || defined(OS_MACX)
#include <sys/types.h>
#elif defined(OS_WIN32)
#include <windows.h>
#endif
#include <string>
#include "commonglobal.h"
#include "libcsmespdef.h"

class LIBCSMES_API System
{
  public:
    static int exec(const char *command,const char *stdout_filename,const char *input_txt=NULL);
    bool call(const char *command,bool capture_stdout);
    int readStdout(char *buffer,int buffer_sz);
    int writeStdin(const char *buffer,int buffer_sz);
    int exitValue();
    static void command2argv(const char *command,char **argv_p[]);
    static void strip_quotes(char *);
    static void stripQuotes(std::string&);
    static char * quote_argument(const char*str);
    static std::string quoteArgument(const std::string &str);
    static bool delete_file(const char * name);
    static bool rename_file ( const char * oldname , const char * newname );
    static int getProcessID() ;
    static bool fileTruncate(FILE *,_I64 s);
    static std::string appendExecSuffix(const std::string &);
    static std::string suppressExecSuffix(const std::string &f);
  private:
    static bool needQuotes(const char *str) ;
    bool capture_stdout;
#if defined(OS_LINUX) || defined(OS_MACX)
    int child_fds[2];
    pid_t child_pid;
    FILE* child_stream_stdout;
    FILE* child_stream_stdin;
#elif defined(OS_WIN32)
    HANDLE hChildProcess ;
    HANDLE hOutputRead,hInputWrite;
    PROCESS_INFORMATION pi;
HANDLE PrepAndLaunchRedirectedChild(
    const char *command,
    HANDLE hChildStdOut,
    HANDLE hChildStdIn,
    HANDLE hChildStdErr);
#endif
} ;

#endif
