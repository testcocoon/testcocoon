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

#ifndef SERVICE_H
#define SERVICE_H
#include "libinstrumentationpdef.h"
#include <qglobal.h>

#include <QString>
#include <QMainWindow>
#include <QWorkspace>

LIBINSTRUMENTATION_API void            printStatus(const QString &, float progress=-1.0);
LIBINSTRUMENTATION_API void            set_MainWindow(QMainWindow *);
LIBINSTRUMENTATION_API QMainWindow *   get_MainWindow();
LIBINSTRUMENTATION_API void            set_Workspace(QWorkspace *);
LIBINSTRUMENTATION_API QWorkspace *   get_Workspace();
LIBINSTRUMENTATION_API int tabSize();
LIBINSTRUMENTATION_API void setTabSize(int v);

#endif
