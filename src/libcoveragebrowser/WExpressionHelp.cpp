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

#include "WExpressionHelp.h"
#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

WExpressionHelp::WExpressionHelp(QWidget* parent,  Qt::WindowFlags fl)
    : QDialog(parent,  fl)
{
    setupUi(this);

}

/*
 *  Destroys the object and frees any allocated resources
 */
WExpressionHelp::~WExpressionHelp()
{
    // no need to delete child widgets, Qt does it all for us
}

