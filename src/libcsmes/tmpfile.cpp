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

#include "tmpfile.h"
#include "system.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <time.h>

void TmpFile::AtExit()
{
  TmpFile::object().deleteFiles();
}



TmpFile::TmpFile()
{
  files=new std::list<tmp_file_t> () ;
  delete_files=true;
  atexit(AtExit);
  id=System::getProcessID();
  id_str=number_base36(id);
  tm=static_cast<unsigned int>(time(NULL));
}

TmpFile& TmpFile::object()
{
  static TmpFile *tmpfile=NULL;
  if (tmpfile==NULL)
    tmpfile=new TmpFile();

  return *tmpfile;
}

std::string TmpFile::number_base36(unsigned int n)
{
  std::string u,s;
  unsigned int base=36;

  do 
  {
    unsigned int v=n%base;
    if(v<10)
      s += v + '0'; 
    else
      s += v - 10 + 'a'; 
    n -= v; //update n value
  }
  while ((n /= base) != 0);

  for (int j = s.length()-1; j >= 0; j--) 
    u += s[j]; //building our string number

  return u;
}

/** generate a file according a template
 *
 * $ID$ is replaced through the PID
 */
std::string TmpFile::generate(const std::string &Template,bool inTempDir) const
{
  for (std::list<tmp_file_t>::const_iterator it=files->begin();it!=files->end();++it)
  {
    if (Template==it->tmpl)
      return it->file;
  }

  std::string tmpl;
  std::string additional_id;
  if (inTempDir)
  {
    std::string base_tmpl=Template;
    int sum=0;
    int template_lg=Template.length();
    std::string::size_type pos=0;
    for (std::string::size_type i=0;i<(unsigned int)template_lg;i++)
    {
      char c=Template[i];
      sum = sum ^ (unsigned int)(c<<((i%4)*8));
      if (c==FILE_SEPARATOR_CHAR || c==FILE_SEPARATOR_CHAR_BIS)
        pos=i+1;
    }
    if (pos!=0)
    {
      base_tmpl.erase(0,pos);
      additional_id="_"+number_base36(sum);
    }
    std::string base_prefix = number_base36(tm) + '_';
    tmpl=tmp_folder();
    tmpl+=FILE_SEPARATOR_CHAR;
    tmpl+=base_prefix;
    tmpl+=base_tmpl;
  }
  else
    tmpl=Template;

  std::string::size_type id_pos;
  while( (id_pos=tmpl.find("$ID$"))!=std::string::npos )
    tmpl.replace(id_pos,4,id_str+additional_id);

  tmp_file_t file;
  file.tmpl=Template;
  file.file=tmpl;
  files->push_back(file);


  return tmpl;
}

void TmpFile::deleteFile(const std::string &file)
{
  if (delete_files)
  {
    System::delete_file(file.c_str());
    removeUnusedFile(file);
  }
}

void TmpFile::removeUnusedFile(const std::string &file)
{
  for (std::list<tmp_file_t>::const_iterator it=files->begin();it!=files->end();++it)
  {
    if (it->file==file)
    {
      files->remove(*it);
      return;
    }
  }
}

void TmpFile::deleteFiles()
{
  if (delete_files)
  {
    for (std::list<tmp_file_t>::const_iterator it=files->begin();it!=files->end();++it)
      System::delete_file(it->file.c_str());
    files->clear();
  }
}


const std::string &TmpFile::tmp_folder() const
{
  FUNCTION_TRACE;
  static std::string folder;
  if (!folder.empty())
    return folder;
#if LOG
  folder=std::string(log_folder());
#else
#if defined(OS_LINUX) || defined(OS_MACX)
  folder = "/tmp/";
#elif defined(OS_WIN32)
  char tmp[CHAINE_LEN];
  if (getenv("TEMP"))
  {
    strcpy(tmp,getenv("TEMP"));
  }
  else
  {
    FATAL1("System variable TEMP not defined");
  }
  strcat(tmp,"\\");
  folder=std::string(tmp);
#endif
#endif
  return folder;
}

