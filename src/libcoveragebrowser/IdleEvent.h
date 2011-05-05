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

#ifndef IDLE_EVENT_H
#define IDLE_EVENT_H

#include <QObject>
#include <QTimer>
class WMain;
#define IDLE_WITH_TIMER 1
class IdleEvent : public QObject
{
  Q_OBJECT
public:
  IdleEvent(QObject *wmain,void (*callback)(QObject*));
  virtual ~IdleEvent();

signals:
  void _event();

public slots:
  void send();

private slots:
  void _shot();
private:
#if IDLE_WITH_TIMER
  QTimer *timer_p;
#else
  bool _under_process;
#endif
  void (*_callback)(QObject*);
  QObject *_wmain;
};

#endif
