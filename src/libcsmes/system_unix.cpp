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
#include "fileinfo.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


bool System::call(const char *command,bool capt_stdout)
{
  FUNCTION_TRACE;
  DEBUG3("System::call cmd='%s' capture_stdout=%s\n",command,(capt_stdout?"true":"false"));
  capture_stdout=capt_stdout;
  /* Create a pipe.  File descriptors for the two ends of the pipe are
     placed in fds.  */
  int fds_stdin[2];
  int fds_stdout[2];
  if (capture_stdout)
  {
    pipe (fds_stdin);
    pipe (fds_stdout);
  }
  /* Fork a child process.  */
  pid_t pid = fork ();
  if (pid == (pid_t) 0) 
  { /* This is the child process.  */
    if (capture_stdout)
    {
      /* close the read descriptor.  */
      close (fds_stdout[0]);
      /* Connect the write end of the pipe to standard output.  */
      dup2 (fds_stdout[1], STDOUT_FILENO);
      /* close the write descriptor.  */
      close (fds_stdin[1]);
      /* Connect the read end of the pipe to standard intput.  */
      dup2 (fds_stdin[0], STDIN_FILENO);
    }
    char **argv=NULL;
    command2argv(command,&argv);
    /* Replace the child process with the program.  */
    execvp (argv[0], argv);
  }
  else 
  { /* This is the parent process.  */
    child_pid=pid;
    if (capture_stdout)
    {
      child_fds[0]=fds_stdout[0];
      child_fds[1]=fds_stdin[1];

      /* Close of the write descriptor.  */
      close (fds_stdout[1]);
      /* Convert the write file descriptor to a FILE object, and write
         to it.  */
      child_stream_stdout = fdopen (fds_stdout[0], "rb");
      /* Close of the read descriptor.  */
      close (fds_stdin[0]);
      /* Convert the write file descriptor to a FILE object, and write
         to it.  */
      child_stream_stdin = fdopen (fds_stdin[1], "wb");
    }
    DEBUG3("System::call SUCCESS cmd='%s' capture_stdout=%s\n",command,(capt_stdout?"true":"false"));
    return true;
  }

  DEBUG3("System::call ERROR cmd='%s' capture_stdout=%s\n",command,(capt_stdout?"true":"false"));
  return false;
}

int System::writeStdin(const char *buffer,int buffer_sz)
{
  FUNCTION_TRACE;
  ASSERT(capture_stdout);
  int nb_written=0;
  do
  {
    nb_written+=fwrite(buffer+nb_written,1,buffer_sz-nb_written,child_stream_stdin);
  } 
  while (nb_written!=buffer_sz);
  fflush(child_stream_stdin);
  return nb_written;
}

int System::readStdout(char *buffer,int buffer_sz)
{
  FUNCTION_TRACE;
  ASSERT(capture_stdout);
  int nb_read=0;
  do
  {
    if (feof(child_stream_stdout))
      return 0;
    nb_read=fread(buffer,1,buffer_sz,child_stream_stdout);
  } while (nb_read==0);
  return nb_read;
}

int System::exitValue()
{
  FUNCTION_TRACE;
  int status;
  if (capture_stdout)
  {
    close (child_fds[0]);
    close (child_fds[1]);
  }
  /* Wait for the child process to finish.  */
  waitpid (child_pid, &status, 0);
  return status;
}


int System::getProcessID() 
{
   static int pid=0;
   if (pid==0)
      pid=getpid();
   return pid;
}

std::string System::appendExecSuffix(const std::string &f)
{
   return f;
}

bool System::fileTruncate(FILE *f ,_I64 s)
{
  int fno=fileno(f);
  int ret = ftruncate(fno,s);

  return ret == 0;
}
