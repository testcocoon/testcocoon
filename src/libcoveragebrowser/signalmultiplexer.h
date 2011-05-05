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

#ifndef SIGNALMULTIPLEXER_H
#define SIGNALMULTIPLEXER_H

#include <qobject.h>
#include <qpointer.h>
#include <QList>

class DocumentObject
{
  public:
    virtual ~DocumentObject()
    {}
    virtual void emitAllSignals() = 0;
};

class SignalMultiplexer : public QObject
{
  Q_OBJECT

  public:
    SignalMultiplexer( QObject *parent = 0 );

    void connect( QObject *sender, const char *signal, const char *slot );
    bool disconnect( QObject *sender, const char *signal, const char *slot );
    void connect( const char *signal, QObject *receiver, const char *slot );
    bool disconnect( const char *signal, QObject *receiver, const char *slot );

    QObject *currentObject() const
    {
      return object;
    }

    public slots:
      void setCurrentObject( QObject *o );

  private:
    struct Connection
    {
      QPointer<QObject> sender;
      QPointer<QObject> receiver;
      const char *signal;
      const char *slot;
    };

    void connect( const Connection &conn );
    void disconnect( const Connection &conn );

  private:

    QPointer<QObject> object;
    QList<Connection> connections;

};

#endif
