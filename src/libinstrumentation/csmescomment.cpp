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

#include "csmescomment.h"
#include "Service.h"
#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <QList>

CSMesComment::CSMesComment() : CSMesExecution()
{
}

void CSMesComment::clear()
{
  comments.clear();
  CSMesExecution::clear();
}

bool CSMesComment::setComment(ModuleFile module,SourceFile source,int instrument_index,const QString &comment)
{
  if (!findSourceModule(module,source))
    return false;

  if (!comments.contains(module))
  {
    sources_comments_t v;
    comments[module]=v;
  }
  if (!comments[module].contains(source))
  {
    comments_t v;
    comments[module][source]=v;
  }
  setModificationFlag();
  if (comment==QString::null || comment==QString())
  {
    if (comments[module][source].contains(instrument_index))
      comments[module][source].remove(instrument_index);
  }
  else
    comments[module][source][instrument_index]=comment;
  return true;
}

QString CSMesComment::getComment(ModuleFile module,SourceFile source,int instrument_index) const
{
  if (!findSourceModule(module,source))
    return QString::null;
  if (!comments.contains(module))
    return QString::null;
  if (!comments[module].contains(source))
    return QString::null;
  if (!comments[module][source].contains(instrument_index))
    return QString::null;
  return comments[module][source][instrument_index];
}

QList<int> CSMesComment::commentedIndexes(ModuleFile mod,SourceFile src) const
{
  QList<int> ret;
  ret.clear();
  if (!findSourceModule(mod,src))
    return ret;
  if (!comments.contains(mod))
    return ret;
  if (!comments[mod].contains(src))
    return ret;
  const comments_t &com= comments[mod][src];
  return com.keys();
}

