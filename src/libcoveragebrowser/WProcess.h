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

#ifndef WPROCESS_H
#define WPROCESS_H

#include "coveragebrowserpdef.h"
#include "ui_WProcessUI.h"
#include <QProcess>

class WProcess : public QDialog, public Ui::WProcessUI
{
  Q_OBJECT

public:
    WProcess(QWidget* parent = 0, Qt::WindowFlags fl = 0);
    virtual ~WProcess();


    void setStdOutput(QByteArray *b) { stdout_p=b; }
    int execute(const QString &command,
        const QStringList &arguments,
        const QString &title,
        bool close_when_finished);
    void setPrintStdOutput(bool b) {printStdOutput=b; }

protected slots:
  virtual void languageChange();
private slots:
  void readyReadStandardOutput();
  void readyReadStandardError();
  void error ( QProcess::ProcessError error );
  void finished ( int exitCode, QProcess::ExitStatus exitStatus );
  void on_close_p_clicked();
protected:
  virtual void keyPressEvent ( QKeyEvent * event );
private:
  QProcess *cmd_p;
  int returned_code;
  int close_when_finished;
  QByteArray *stdout_p;
  bool printStdOutput;
};

#endif 
