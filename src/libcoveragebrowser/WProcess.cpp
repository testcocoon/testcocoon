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

#include "WProcess.h"
#include <QProcess>
#include <QKeyEvent> 
#include <QTextStream>

WProcess::WProcess(QWidget* parent,  Qt::WindowFlags fl)
    : QDialog(parent,  fl)
{
  printStdOutput=true;
  stdout_p=NULL;
  setupUi(this);
  cmd_p = new QProcess (this);
}

WProcess::~WProcess()
{
  if (QProcess::NotRunning!=cmd_p->state())
    cmd_p->kill();
  delete cmd_p;
}

void WProcess::languageChange()
{
    retranslateUi(this);
}

int WProcess::execute(const QString &command,
    const QStringList &arguments,
    const QString &title,
    bool close_when_finished)
{
  grabKeyboard();
  setWindowTitle(title);
  connect(cmd_p,SIGNAL(readyReadStandardOutput()),this,SLOT(readyReadStandardOutput()));
  connect(cmd_p,SIGNAL(readyReadStandardError()),this,SLOT(readyReadStandardError()));
  connect(cmd_p,SIGNAL(error(QProcess::ProcessError )),this,SLOT(error(QProcess::ProcessError )));
  connect(cmd_p,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
  close_p->setText(tr("Cancel"));
  text_p->clear();
  status_p->setText(tr("Executing %1 ...").arg(command));
  cmd_p->start(command,arguments);
  returned_code=-1;
  if (cmd_p->waitForStarted())
  {
    status_p->setText(tr("%1 is running").arg(command));
    this->close_when_finished=close_when_finished;

    if (! exec() )
      returned_code=-1;
  }

  releaseKeyboard();
  stdout_p=NULL;
  return returned_code;
}

void WProcess::readyReadStandardError()
{
  text_p->setTextColor(COL_STDERR);
  text_p->insertPlainText(cmd_p->readAllStandardError());
}

void WProcess::on_close_p_clicked()
{
  if (QProcess::NotRunning!=cmd_p->state())
  {
    returned_code=-2;
    cmd_p->kill();
  }
  accept();
}

void WProcess::readyReadStandardOutput()
{
  QByteArray data=cmd_p->readAllStandardOutput();
  if (printStdOutput)
  {
    text_p->setTextColor(COL_STDOUT);
    text_p->insertPlainText(data);
  }
  if (stdout_p)
    stdout_p->append(data);
}

void WProcess::error ( QProcess::ProcessError error )
{
  close_p->setText(tr("Close"));
  QString errorStr;
  switch (error)
  {
    case QProcess::FailedToStart:
      errorStr=tr("Failed to start");
      break;
    case QProcess::Crashed:
      errorStr=tr("Crashed");
      break;
    case QProcess::Timedout:
      errorStr=tr("Timedout");
      break;
    case QProcess::WriteError:
      errorStr=tr("Write error");
      break;
    case QProcess::ReadError:
      errorStr=tr("Read error");
      break;
    case QProcess::UnknownError:
      errorStr=tr("Unknown error");
      break;
  }
  status_p->setText(errorStr);
}

void WProcess::finished ( int exitCode, QProcess::ExitStatus  exitStatus  )
{
  close_p->setText(tr("Close"));
  returned_code=exitCode;
  if (exitStatus==QProcess::NormalExit)
  {
    status_p->setText(QString());
    if (close_when_finished && exitCode==0)
      accept();
  }
}

void WProcess::keyPressEvent ( QKeyEvent * event )
{
  if (cmd_p)
  {
    if (cmd_p->state()==QProcess::Running)
    {
      QTextStream stream(cmd_p);
      stream<<event->text();
    }
  }
  event->accept();
}
