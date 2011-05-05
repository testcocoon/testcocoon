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

#include "libinstrumentationpdef.h"
#include "csmesfilethread.h"
#include <QMutexLocker>

CSMESFileThread::CSMESFileThread() : CSMESFile()
{
}

bool CSMESFileThread::open( const char *filename, access_t a)
{
  QMutexLocker lock(&locker);
  return CSMESFile::open( filename, a);
}

bool CSMESFileThread::reopen( access_t a) 
{
  QMutexLocker lock(&locker);
  return CSMESFile::reopen( a) ;
}

bool CSMESFileThread::openSection(const char *module,const char *module_rel,const char *name,const char *name_rel,const char *info,type_t type,unsigned long signature,access_t a,unsigned long flag)
{
  locker.lock();
  bool res = CSMESFile::openSection(module,module_rel,name,name_rel,info,type,signature,a,flag);
  if (!res)
    locker.unlock();
  return res;
}

void CSMESFileThread::close()
{
  QMutexLocker lock(&locker);
  CSMESFile::close();
}

bool CSMESFileThread::deleteSection(const char *module,const char *name,const char *info,type_t type)
{
  QMutexLocker lock(&locker);
  return CSMESFile::deleteSection(module,name,info,type);
}

bool CSMESFileThread::deleteSectionID(int id)
{
  QMutexLocker lock(&locker);
  return CSMESFile::deleteSectionID(id);
}

bool CSMESFileThread::append(CSMESFile &csmes)
{
  QMutexLocker lock(&locker);
  return CSMESFile::append(csmes);
}

bool CSMESFileThread::merge(CSMESFile &csmes,unsigned long fl_merge,CSMESFile::merge_policy_t policy,std::string &errmsg)
{
  QMutexLocker lock(&locker);
  return CSMESFile::merge(csmes,fl_merge,policy,errmsg);
}

bool CSMESFileThread::closeSection()
{
  bool ret = CSMESFile::closeSection();
  locker.unlock();
  return ret;
}

bool CSMESFileThread::openSectionID(int id)
{
  locker.lock();
  bool ret = CSMESFile::openSectionID(id);
  if (!ret)
    locker.unlock();
  return ret;
}


bool CSMESFileThread::isCoverageBranchOnly()
{
  QMutexLocker lock(&locker);
  return CSMESFile::isCoverageBranchOnly();
}
bool CSMESFileThread::save_blackbox(const char *filename)
{
  QMutexLocker lock(&locker);
  return CSMESFile::save_blackbox(filename);
}
