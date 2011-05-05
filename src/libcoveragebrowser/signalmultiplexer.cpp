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

#include "signalmultiplexer.h"
#include <QList>

  SignalMultiplexer::SignalMultiplexer( QObject *parent )
: QObject( parent)
{}

void SignalMultiplexer::connect( QObject *sender,
    const char *signal,
    const char *slot )
{
  Connection conn;
  conn.sender = sender;
  conn.signal = signal;
  conn.slot = slot;

  connections << conn;
  connect( conn );
}

bool SignalMultiplexer::disconnect( QObject *sender,
    const char *signal,
    const char *slot )
{
  for ( QList<Connection>::Iterator it = connections.begin();
      it != connections.end(); ++it )
  {
    Connection conn = *it;
    if ( (QObject*)conn.sender == sender &&
        qstrcmp( conn.signal, signal ) == 0 &&
        qstrcmp( conn.slot, slot ) == 0 )
    {
      disconnect( conn );
      connections.erase( it );
      return true;
    }
  }
  return false;
}

void SignalMultiplexer::connect( const char *signal,
    QObject *receiver,
    const char *slot )
{
  Connection conn;
  conn.receiver = receiver;
  conn.signal = signal;
  conn.slot = slot;

  connections << conn;
  connect( conn );
}

bool SignalMultiplexer::disconnect( const char *signal,
    QObject *receiver,
    const char *slot )
{
  for ( QList<Connection>::Iterator it = connections.begin();
      it != connections.end(); ++it )
  {
    Connection conn = *it;
    if ( (QObject*)conn.receiver == receiver &&
        qstrcmp( conn.signal, signal ) == 0 &&
        qstrcmp( conn.slot, slot ) == 0 )
    {
      disconnect( conn );
      connections.erase( it );
      return true;
    }
  }
  return false;
}

void SignalMultiplexer::setCurrentObject( QObject *o )
{
  if ( o == object )
    return;

  QList<Connection>::ConstIterator it;
  if (object)
  {
    for ( it = connections.begin(); it != connections.end(); ++it )
      disconnect( *it );
  }
  object = o;
  if (object)
  {
    for ( it = connections.begin(); it != connections.end(); ++it )
      connect( *it );
    DocumentObject *d = dynamic_cast<DocumentObject*>( o );
    if ( d )
      d->emitAllSignals();
  }
}

void SignalMultiplexer::connect( const Connection &conn )
{
  if ( !object )
    return;
  if ( !conn.sender && !conn.receiver )
    return;

  if ( conn.sender )
    QObject::connect( (QObject*)conn.sender, conn.signal,
        (QObject*)object, conn.slot );
  else
    QObject::connect( (QObject*)object, conn.signal,
        (QObject*)conn.receiver, conn.slot );
}

void SignalMultiplexer::disconnect( const Connection &conn )
{
  if ( !object )
    return;
  if ( !conn.sender && !conn.receiver )
    return;

  if ( conn.sender )
    QObject::disconnect( (QObject*)conn.sender, conn.signal,
        (QObject*)object, conn.slot );
  else
    QObject::disconnect( (QObject*)object, conn.signal,
        (QObject*)conn.receiver, conn.slot );
}

