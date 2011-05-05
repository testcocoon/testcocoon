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
#include "csmesfunctioninfo.h"

QVector<FunctionInfo> CSMesFunctionInfo::FunctionInfoSource( QString module, QString source) const
{
   if (!findSourceModule(module,source))
      return QVector<FunctionInfo>();

   return instrumentations.modules[module].sources[source].functions;
}


bool CSMesFunctionInfo::instrumentationListFunctionPre(QString module,QString source,long start_line,long start_column,long end_line,long end_column,QList<int> &instrumentation_list) const
{
  instrumentation_list.clear();
  if (!findSourceModule(module,source))
  {
    return false;
  }


  const CSMesInstrumentations::Instrumentations &inst=instrumentations.modules[module].sources[source].instrumentations;
  int count=inst.count();
  for (int i=0;i!=count;++i)
  {
    if ( 
        ( inst.at(i).startLinePre()>start_line 
          || ( inst.at(i).startLinePre()==start_line &&  inst.at(i).startColumnPre()>=start_column )
        )
        && 
        ( inst.at(i).endLinePre()<end_line
          || ( inst.at(i).endLinePre()==end_line &&  inst.at(i).endColumnPre()<=end_column )
        )
       )
    {
      instrumentation_list.push_back(i);
    }
  }
  return true;
}

const FunctionInfo * CSMesFunctionInfo::FindFunction(const QString &source,const FunctionInfo &k) const 
{
  const QString &scoped_name=k.getScopedName();
  const QString &prototype=k.getPrototype();
  QVector<FunctionInfo> FunctionsInfo = FunctionInfoSource(QString(),source);
  for (QVector<FunctionInfo>::const_iterator itfct=FunctionsInfo.begin();itfct!=FunctionsInfo.end();++itfct)
  {
    const QString &_scoped_name=(*itfct).getScopedName();
    if (_scoped_name==scoped_name)
    {
       const QString &_prototype=(*itfct).getPrototype();
       if (_prototype==prototype)
          return & (*itfct);
    }
  }
  return NULL;
}

QList<CSMesFunctionInfo::functionskey_t> CSMesFunctionInfo::Functions() const 
{
  QList<functionskey_t> functions;
  const QStringList sources=Sources(ALL);

  for (QStringList::const_iterator itsrc=sources.begin();itsrc!=sources.end();++itsrc)
  {

    QVector<FunctionInfo> FunctionsInfo = FunctionInfoSource(QString(),*itsrc);
    for (QVector<FunctionInfo>::const_iterator itfct=FunctionsInfo.begin();itfct!=FunctionsInfo.end();++itfct)
    {
      functionskey_t f;
      f.source=*itsrc;
      f.method=*itfct;
      functions.append(f);
    }
  }
  return functions;
}

