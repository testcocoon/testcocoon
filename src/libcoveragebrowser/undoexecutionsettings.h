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

#ifndef UNDO_EXECUTION_SETTINGS_H
#define UNDO_EXECUTION_SETTINGS_H
#include <QUndoCommand>
#include <QString>
#include "CoverageSettings.h"
class CSMes ;


class UndoCmdExecutionSettings : public QUndoCommand
{
  public:
    UndoCmdExecutionSettings (CSMes *csmes_p,const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, int coverage_level,Instrumentation::coverage_method_t method, CSMes::comparaison_mode_t m, bool execution_analysis_mode,QUndoCommand *parent) ;
    virtual ~UndoCmdExecutionSettings ();
    virtual void redo();
    virtual void undo();
    bool nop() const ;


  private:

    CSMes *m_csmes_p;
    CoverageSettings undo_settings;
    CoverageSettings redo_settings;
};
#endif
