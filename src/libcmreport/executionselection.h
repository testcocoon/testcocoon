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

#ifndef EXECUTION_SELECTION_H
#define EXECUTION_SELECTION_H
#include "cmreportpdef.h"
#include <qstring.h>
#include <QList>
#include "csmes.h"

class ExecutionSelection
{
  public:
    ExecutionSelection(CSMes &);
    void selectRegex(const QString &);
    void deselectRegex(const QString &);
    void apply(bool debug);
    void setTestCountMode(bool b) {_test_count_mode=b;}
    void setCoverageMethod(Instrumentation::coverage_method_t m) {_method=m; }
    Instrumentation::coverage_method_t getCoverageMethod() const { return _method; }
    bool isTestCountMode() const { return _test_count_mode; }

  private:
    CSMes &_csmes;
    ExecutionNames _selected_executions;
    bool _test_count_mode;
    Instrumentation::coverage_method_t _method;
  enum selection_type_t
  {
    SELECT,DESELECT
  };
  struct selection_t
  {
    selection_type_t type;
    QString expression;
  };
  QList<selection_t> _selections;
  void _selectRegex(const QString &);
  void _deselectRegex(const QString &);
};

#endif
