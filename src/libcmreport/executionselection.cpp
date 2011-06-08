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

#include "executionselection.h"
#include <QRegExp>
#include <iostream>
using namespace std;

ExecutionSelection::ExecutionSelection(CSMes &m):_csmes(m)
{
  _test_count_mode=false;
  _method=Instrumentation::COVERAGE_CONDITION;
  _selections.clear();
}

void ExecutionSelection::selectRegex(const QString &exp)
{
  selection_t sel;
  sel.type=SELECT;
  sel.expression=exp;
  _selections += sel;
}

void ExecutionSelection::_selectRegex(const QString &exp)
{
  QRegExp regex(exp);
  ExecutionNames executions=_csmes.executionList();
  for (ExecutionNames::const_iterator it=executions.begin();it!=executions.end();++it)
  {
    if (regex.exactMatch(*it) && !_selected_executions.contains(*it))
      _selected_executions+=*it;
  }
}

void ExecutionSelection::deselectRegex(const QString &exp)
{
  selection_t sel;
  sel.type=DESELECT;
  sel.expression=exp;
  _selections += sel;
}
  
void ExecutionSelection::_deselectRegex(const QString &exp)
{
  QRegExp regex(exp);
  ExecutionNames new_selected;
  for (ExecutionNames::const_iterator it=_selected_executions.begin();it!=_selected_executions.end();++it)
  {
    if (!regex.exactMatch(*it))
      new_selected+=*it;
  }
  _selected_executions=new_selected;
}

void ExecutionSelection::apply(bool debug)
{
  _selected_executions.clear();
  for (QList<selection_t>::const_iterator it=_selections.begin();it!=_selections.end();++it)
  {
    switch ((*it).type)
    {
      case SELECT:
        _selectRegex((*it).expression);
        break;
      case DESELECT:
        _deselectRegex((*it).expression);
        break;
    }
  }
  
  if (debug)
  {
    ExecutionNames executions=_csmes.executionList();
    cerr << "Execution List:" << endl;
    for (ExecutionNames::const_iterator itExec=executions.begin();itExec!=executions.end();++itExec)
    {
      QString item=*itExec;
      cerr << '\t' << item.toStdString() <<endl;
      if ( _selected_executions.contains(*itExec) )
        cerr << '\t' << "[Selected]" <<endl;
      cerr <<endl;
    }
  }
  _csmes.selectExecutionsComparaison(_selected_executions, ExecutionNames(),_test_count_mode, _method, CSMes::COMPARAISON_MODE_NONE) ;
}
