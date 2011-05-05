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

#include "accesslocker.h"

AccessLocker::AccessLocker()
{
  lock_read=0;
  lock_write=0;
}

void AccessLocker::lockRead() const
{
  Qt::HANDLE threadId=QThread::currentThreadId();
  for (;;)
  {
    mutex.lock();
    ASSERT(lock_read>=0);
    ASSERT(lock_write>=0);
    if (   (lock_write==0)
        || ( threadId==write_lock_thread_id )
       )
    {
      //read_lock_thread_id.prepend(threadId);
      lock_read++;
      mutex.unlock();
      return;
    }
    lock_changed.wait(&mutex);
    mutex.unlock();
  }
}

void AccessLocker::unlockRead() const
{
  //Qt::HANDLE threadId=QThread::currentThreadId();
  QMutexLocker lock(&mutex);
  lock_read--;
  //read_lock_thread_id.removeOne(threadId);
  lock_changed.wakeAll();
  ASSERT(lock_read>=0);
  ASSERT(lock_write>=0);
}

void AccessLocker::lockWrite() 
{
  Qt::HANDLE threadId=QThread::currentThreadId();
  for (;;)
  {
    mutex.lock();
    ASSERT(lock_read>=0);
    ASSERT(lock_write>=0);
    if (lock_write>0)
    {
      if ( write_lock_thread_id==threadId)
      {
        lock_write++;
        mutex.unlock();
        return;
      }
    }
    if (lock_read==0 && lock_write==0)
    {
      write_lock_thread_id=threadId;
      lock_write++;
      mutex.unlock();
      return;
    }
    lock_changed.wait(&mutex);
    mutex.unlock();
  }
}

void AccessLocker::unlockWrite() 
{
  QMutexLocker lock(&mutex);
  lock_write--;
  lock_changed.wakeAll();
  ASSERT(lock_read>=0);
  ASSERT(lock_write>=0);
}
