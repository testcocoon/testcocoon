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

#ifndef UNDO_MANUALLY_VALIDATED_H
#define UNDO_MANUALLY_VALIDATED_H
#include <QUndoCommand>
#include <QString>
class CSMes ;


class UndoCmdManuallyValidated : public QUndoCommand
{
  public:
    UndoCmdManuallyValidated (CSMes *csmes_p,const QString &module,const QString &source,int index,const bool &old_manually_validated,const bool &new_manually_validated, QUndoCommand *parent);
    virtual void redo();
    virtual void undo();


  private:

    CSMes *m_csmes_p;
    QString m_module;
    QString m_source;
    int m_index;
    bool m_old_manually_validated;
    bool m_new_manually_validated;
};

#endif
