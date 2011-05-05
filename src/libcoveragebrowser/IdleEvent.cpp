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

#include "IdleEvent.h"
#include "WMain.h"

IdleEvent::IdleEvent(QObject *wmain,void (*callback)(QObject *)):QObject(wmain) 
{
#if IDLE_WITH_TIMER
  timer_p=new QTimer(this);
  timer_p->setInterval(50);
  timer_p->setSingleShot(true);
  connect(timer_p, SIGNAL(timeout()), this, SLOT(_shot()),Qt::QueuedConnection);
#else
  _under_process=false;
  connect(this,SIGNAL(_event()),this,SLOT(_shot()),Qt::QueuedConnection);
#endif
  _callback=callback;
  _wmain=wmain;
}

IdleEvent::~IdleEvent()
{
#if IDLE_WITH_TIMER
   delete timer_p;
#endif
}

void IdleEvent::_shot()
{
#if !IDLE_WITH_TIMER
  _under_process=false;
#endif
  _callback(_wmain);
}

void IdleEvent::send()
{
#if IDLE_WITH_TIMER
  timer_p->start();
#else
  if (_under_process)
    return;
  _under_process=true;
  emit _event();
#endif
}
