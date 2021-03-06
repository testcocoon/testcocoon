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

#include "WExecutionRename.h"

WExecutionRename::WExecutionRename(const CSMesUndoRedoFramework *c,QWidget* parent,  Qt::WindowFlags fl):WExecutionManipulation(c,parent,fl)
{
  setWindowTitle(tr("Renaming Executions using Regular Expressions"));
  setSourceName(tr("Actual Execution Name"));
  setDestinationName(tr("New Execution Name"));
}

WExecutionRename::~WExecutionRename()
{
}

