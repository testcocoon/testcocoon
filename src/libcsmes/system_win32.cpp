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

#include "system.h"
#include "libcsmespdef.h"
#include <stdio.h> 
#include <windows.h> 
#include <io.h> 
#pragma comment(lib, "User32.lib")
#include <io.h> 

#define BUFSIZE 4096 


/////////////////////////////////////////////////////////////////////// 
// PrepAndLaunchRedirectedChild
// Sets up STARTUPINFO structure, and launches redirected child.
/////////////////////////////////////////////////////////////////////// 
HANDLE System::PrepAndLaunchRedirectedChild(
    const char *command,
    HANDLE hChildStdOut,
    HANDLE hChildStdIn,
    HANDLE hChildStdErr)
{
  FUNCTION_TRACE;
  char *file=NULL;
  int pos=0;
  bool in_quote=false;
  for (pos=0;command[pos]!='\0';pos++)
  {
    if (command[pos]=='"')
      in_quote = ! in_quote;
    if (in_quote)
      continue;

    if (command[pos]==' ')
      break;
  }
  file=(char*)MALLOC(pos+1+4);
  if (command[0]=='"')
  { // stripping quotes
    memcpy(file,&command[1],pos-1);
    for (int i=0;i<pos-1;i++)
      if (file[i]=='"')
        file[i]='\0';
  }
  else
    memcpy(file,command,pos);
  file[pos]='\0';
  const char*extension=".exe";
  size_t file_lg=strlen(file);
  if (file_lg<4 || ! (strcmp(extension,&file[file_lg-4])==0) )
    strcat(file,extension);

  wchar_t actDirBuffer[MAX_PATH];
  GetCurrentDirectory(MAX_PATH, actDirBuffer);
  size_t file_sz=strlen(file);
  wchar_t *file_w=(wchar_t*)MALLOC((file_sz+1)*sizeof(wchar_t));
  wchar_t file_abs_w[MAX_PATH+4];
  wchar_t *file_name_w;
  wchar_t extension_w[50];;
  mbstowcs(extension_w,extension,sizeof(extension));
  mbstowcs(file_w,file,file_sz+1);
  size_t cmdBuffer_sz=strlen(command);
  wchar_t *cmdBuffer_w=(wchar_t*)MALLOC((cmdBuffer_sz+1)*sizeof(wchar_t));
  mbstowcs(cmdBuffer_w,command,cmdBuffer_sz+1);
  if (SearchPath(NULL,file_w,extension_w,MAX_PATH,file_abs_w,&file_name_w)==0)
  {
    FATAL3("Error: finding path of %s: %u",file,GetLastError());
  }

  STARTUPINFO si;

  // Set up the start up info struct.
  ZeroMemory(&si,sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  if (capture_stdout)
  {
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hChildStdOut;
    si.hStdInput  = hChildStdIn;
    si.hStdError  = hChildStdErr;
    // Use this if you want to hide the child:
    //     si.wShowWindow = SW_HIDE;
    // Note that dwFlags must include STARTF_USESHOWWINDOW if you want to
    // use the wShowWindow flags.
  }


  // Launch the process that you want to redirect (in this case,
  // Child.exe). Make sure Child.exe is in the same directory as
  // redirect.c launch redirect from a command line to prevent location
  // confusion.
  if (!CreateProcess(file_abs_w,cmdBuffer_w,NULL,NULL,TRUE,
        0,
        NULL,NULL,&si,&pi))
  {
    ERROR2("CreateProcess errno:%i\n",GetLastError());
    return false;
  }


  // Set global child process handle to cause threads to exit.
  HANDLE hChildProcess = pi.hProcess;


  // Close any unnecessary handles.
  //if (!CloseHandle(pi.hThread)) ERROR2("CloseHandle errno:%i\n",GetLastError());
  return hChildProcess;
}


bool System::call(const char *command,bool capt_stdout)
{
  hOutputRead=NULL;
  hInputWrite=NULL;
  capture_stdout=capt_stdout;
  hChildProcess = NULL;

  HANDLE hOutputWrite=GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE hInputRead=GetStdHandle(STD_INPUT_HANDLE);
  SECURITY_ATTRIBUTES sa;


  // Set up the security attributes struct.
  sa.nLength= sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = TRUE;

  if (capture_stdout)
  {
     HANDLE hOutputReadTmp=NULL;
     HANDLE hInputWriteTmp=NULL;

    // Create the child output pipe.
    if (!CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0))
    {
      ERROR2("CreatePipe errno:%i\n",GetLastError());
      return false;
    }


    // Create the child input pipe.
    if (!CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0))
    {
      ERROR2("CreatePipe errno:%i\n",GetLastError());
      return false;
    }


    // Create new output read handle and the input write handles. Set
    // the Properties to FALSE. Otherwise, the child inherits the
    // properties and, as a result, non-closeable handles to the pipes
    // are created.
    if (!DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
          GetCurrentProcess(),
          &hOutputRead, // Address of new handle.
          0,FALSE, // Make it uninheritable.
          DUPLICATE_SAME_ACCESS))
    {
      ERROR2("DupliateHandle errno:%i\n",GetLastError());
      return false;
    }

    if (!DuplicateHandle(GetCurrentProcess(),hInputWriteTmp,
          GetCurrentProcess(),
          &hInputWrite, // Address of new handle.
          0,FALSE, // Make it uninheritable.
          DUPLICATE_SAME_ACCESS))
    {
      ERROR2("DupliateHandle errno:%i\n",GetLastError());
      return false;
    }


    // Close inheritable copies of the handles you do not want to be
    // inherited.
    if (!CloseHandle(hOutputReadTmp)) 
    {
      ERROR2("CloseHandle errno:%i\n",GetLastError());
      return false;
    }
    if (!CloseHandle(hInputWriteTmp)) 
    {
      ERROR2("CloseHandle errno:%i\n",GetLastError());
      return false;
    }

  }

  hChildProcess = PrepAndLaunchRedirectedChild(command,hOutputWrite,hInputRead,GetStdHandle(STD_ERROR_HANDLE));


  if (capt_stdout)
  {
    // Close pipe handles (do not continue to modify the parent).
    // You need to make sure that no handles to the write end of the
    // output pipe are maintained in this process or else the pipe will
    // not close when the child process exits and the ReadFile will hang.
    if (!CloseHandle(hOutputWrite)) 
    {
      ERROR2("CloseHandle errno:%i\n",GetLastError());
      return false;
    }
    if (!CloseHandle(hInputRead )) 
    {
      ERROR2("CloseHandle errno:%i\n",GetLastError());
      return false;
    }
  }

  return true;
}


static int stripChars(char *buffer,int buffer_sz)
{
  FUNCTION_TRACE;
  int new_buffer_sz=buffer_sz;
  int j=0;
  for (int i=0;i<buffer_sz;i++)
  {
    buffer[j]=buffer[i];
    switch (buffer[i])
    {
      case '\r':
        new_buffer_sz--;
        break;
      default:
        j++;
        break;
    }
  }
  return new_buffer_sz;
}

int System::writeStdin(const char *buffer,size_t buffer_sz)
{
  FUNCTION_TRACE;
  ASSERT(capture_stdout);
  size_t nb_written=0;
  do
  {
    DWORD nb_wr=0;
    if (!WriteFile(hInputWrite,buffer+nb_written, static_cast<DWORD>(buffer_sz-nb_written),&nb_wr,NULL))
      return (int)nb_written;
    nb_written+=nb_wr;
  } 
  while (nb_written!=buffer_sz);
  return (int)nb_written;
}

int System::readStdout(char *buffer,size_t buffer_sz)
{
  FUNCTION_TRACE;
  ASSERT(capture_stdout);
  DWORD dwRead;
  do
  {
    if( !ReadFile( hOutputRead, buffer, static_cast<DWORD>(buffer_sz), &dwRead, NULL) )
      return 0;
  }
  while (dwRead==0);
  //dwRead=stripChars(buffer,dwRead);
  return static_cast<size_t>(dwRead);
}

int System::exitValue()
{
  FUNCTION_TRACE;
  WaitForSingleObject(pi.hProcess,INFINITE);
  // Force the read on the input to return by closing the stdin handle.
  if (capture_stdout)
  {
    if (!CloseHandle(hOutputRead)) 
    {
      DEBUG2("CloseHandle errno:%i\n",GetLastError());
    }
    if (!CloseHandle(hInputWrite)) 
    {
      DEBUG2("CloseHandle errno:%i\n",GetLastError());
    }
  }
  DWORD ret_value;
  GetExitCodeProcess(pi.hProcess,&ret_value);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return ret_value;
}

int System::getProcessID() 
{
  static int pid=0;
  if (pid==0)
    pid=GetCurrentProcessId();
  return pid;
}

std::string System::appendExecSuffix(const std::string & exec_param)
{
  FUNCTION_TRACE;
	size_t lg_exec_param=exec_param.length();
	if (lg_exec_param<4 || strcasecmp(&(exec_param.c_str())[lg_exec_param-4],".exe")!=0)
	{ // append .exe extension
      return exec_param+".exe";
	}
	else
      return exec_param;
}

bool System::fileTruncate(FILE *f ,_I64 s)
{
  int fno=fileno(f);
  int ret = _chsize_s(fno,s);

  return ret == 0;
}

char * System::quote_argument(const char*str)
{
  FUNCTION_TRACE;
  char *out;

  if (str==NULL)
    return NULL;

  if (needQuotes(str))
  {
    out=(char*)MALLOC(strlen(str)+3);
    strcpy(out,"\"");
    strcat(out,str);
    strcat(out,"\"");
    DEBUG3("Quoting argument '%s'->'%s'\n",str,out);
  }
  else
    out=STRDUP(str);
  return out;
}


void System::strip_quotes(char * arg)
{
  FUNCTION_TRACE;
  int id=0;
  bool escape=false;
  while (arg[id]!='\0')
  {
      bool have_stripped = false;
      if (arg[id]=='"' && !escape)
      { /* stripping quotes */
          char *tmp=&arg[id];
          while (*tmp) 
          {
              *tmp=*(tmp+1);
              tmp++;
          }
          have_stripped=true;
      }

      if (arg[id]=='\\')
      {
          if (escape)
              escape=false;
          else
              escape=true;
      }
      else
          escape=false;
      if (!have_stripped)
          id++;
  }
}

