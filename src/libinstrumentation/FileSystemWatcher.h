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

#ifndef FILE_SYSTEM_WATCHER_H
#define FILE_SYSTEM_WATCHER_H
#include <QFileSystemWatcher>
#include <QString>
#include "libinstrumentationpdef.h"

class LIBINSTRUMENTATION_API FileSystemWatcher : public QFileSystemWatcher
{
  Q_OBJECT

  public:
    FileSystemWatcher ( const QString& file);

    virtual ~FileSystemWatcher();

  private:
    void watchFile(const QString &);
    QString _file;

  private slots:
    void directoryChangedSlot ( const QString & );
    void fileChangedSlot ( const QString & );
  signals:
    void fileChanged();
};

#endif
