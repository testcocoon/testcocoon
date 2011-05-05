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

#include "FileSystemWatcher.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>

FileSystemWatcher::FileSystemWatcher ( const QString& file  ): QFileSystemWatcher ( )
{
  connect(this,SIGNAL( fileChanged ( const QString & )),
      this,SLOT( fileChangedSlot ( const QString & )));
  connect(this,SIGNAL( directoryChanged ( const QString & )),
      this,SLOT( directoryChangedSlot ( const QString & )));
  _file=file;
  watchFile(file);
}

void FileSystemWatcher::watchFile(const QString &file)
{
  if (!files().isEmpty())
    removePaths(files());
  if (!directories().isEmpty())
    removePaths(directories());
  QFileInfo fileInfo(file);
  if (fileInfo.exists())
    addPath(file);
  QString filePath=fileInfo.absoluteDir().path();
  QFileInfo filePathInfo(filePath);
  if (filePathInfo.exists())
    addPath(filePath);
}

void FileSystemWatcher::fileChangedSlot ( const QString & )
{
  QFileInfo fileInfo(_file);
  if ( fileInfo.exists() )
    emit fileChanged();
}

void FileSystemWatcher::directoryChangedSlot ( const QString & )
{
  if (!files().contains(_file))
  {
    QFileInfo fileInfo(_file);
    if ( fileInfo.exists() )
    {
      addPath(_file);
      emit fileChanged();
    }
  }
}

FileSystemWatcher::~FileSystemWatcher()
{
}
