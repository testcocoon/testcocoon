/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
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

#ifndef ACCESS_LOCKER_H
#define ACCESS_LOCKER_H
#include "libinstrumentationpdef.h"
#include <QMutex>
#include <QThread>
#include <QMutexLocker>
#include <QWaitCondition>

class AccessLocker
{
public:
   AccessLocker();

   void lockRead() const;
   void unlockRead() const;
   void lockWrite() ;
   void unlockWrite() ;
   
   void lock() { lockWrite(); }
   void unlock() { unlockWrite(); }
   void lock() const { lockRead(); }
   void unlock() const { unlockRead(); }

private:
   mutable QMutex mutex;
   int lock_write;
   Qt::HANDLE write_lock_thread_id;
   //mutable QList<Qt::HANDLE> read_lock_thread_id;
   mutable int lock_read;
   mutable QWaitCondition lock_changed;
};


class AccessLockerRead
{
   public:
      AccessLockerRead(const AccessLocker *lock) { _lock=lock; _lock->lock() ; }
      ~AccessLockerRead() { _lock->unlock() ; }
   private:
      const AccessLocker *_lock;
};

class AccessLockerWrite
{
   public:
      AccessLockerWrite(AccessLocker *lock) { _lock=lock; _lock->lock() ; }
      ~AccessLockerWrite() { _lock->unlock() ; }
   private:
      AccessLocker *_lock;
};


#endif
