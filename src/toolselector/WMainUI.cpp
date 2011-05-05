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

#include "WMainUI.h"
#include <qmessagebox.h>
#include <qfiledialog.h>
#ifdef OS_WIN32
#include <windows.h>
#endif
#include "visualstudio.h"
#include "intelcompiler.h"
#include "msys.h"
#include "mingw.h"
#include "cygwin.h"
#include "notools.h"

WMainUI::WMainUI(QWidget *parent,Qt::WFlags f) : QDialog(parent,f)
{
  setupUi(this);
  tools.append(new VisualStudio);
  tools.append(new MinGW);
  tools.append(new CygWin);
  tools.append(new NoTools);
  tools.append(new MSys);
  tools.append(new IntelCompiler);

  QStringList tool_list;
  QList<Tools*>::Iterator it;
  for (it=tools.begin();it!=tools.end();++it)
  {
    tool_list+=(*it)->toolList();
  }
  tool_list.sort();
  list_p->insertItems(0,tool_list);
  updateView();
}

WMainUI::~WMainUI()
{
  qDeleteAll(tools);
}

void WMainUI::on_close_p_clicked()
{
  accept();
}


void WMainUI::updateView()
{
  const QListWidgetItem * item_p=list_p->currentItem();
  if (item_p==NULL)
    return;
  QString item=item_p->text();
  if (item==QString::null)
  {
    directory_p->hide();
    directory_label_p->hide();
    install_p->hide();
    uninstall_p->hide();
    set_console_p->hide();
    open_console_p->hide();
  }
  else
  {
    QList<Tools*>::Iterator it;
    Tools *tool_p=NULL;
    for (it=tools.begin();it!=tools.end();++it)
    {
      tool_p=*it;
      if (tool_p->toolList().contains(item))
        break;
    }

    if (tool_p)
    {
      directory_p->setText(tool_p->directory(item));
      if (tool_p->uninstall(item,directory_p->text())==QString::null)
        uninstall_p->hide();
      else
      {
        uninstall_p->setText(tool_p->uninstall(item,directory_p->text()));
        uninstall_p->show();
      }
      if (tool_p->install(item,directory_p->text())==QString::null)
        install_p->hide();
      else
      {
        install_p->setText(tool_p->install(item,directory_p->text()));
        install_p->show();
      }
      if (tool_p->directoryLabel(item)==QString::null)
      {
        directory_label_p->hide();
        directory_p->hide();
      }
      else
      {
        directory_label_p->setText(tool_p->directoryLabel(item)+':');
        directory_label_p->show();
        directory_p->show();
      }
      set_console_p->setShown (tool_p->setConsole(item,directory_p->text()));
      open_console_p->setShown (tool_p->setConsole(item,directory_p->text()));

    }
  }
  resize(640,240);
}

void WMainUI::on_list_p_itemClicked( QListWidgetItem * )
{

}


void WMainUI::on_uninstall_p_clicked()
{
  const QListWidgetItem * item_p=list_p->currentItem();
  if (item_p==NULL)
    return;
  QString item=item_p->text();
  QList<Tools*>::Iterator it;
  Tools *tool_p=NULL;
  for (it=tools.begin();it!=tools.end();++it)
  {
    tool_p=*it;
    if (tool_p->toolList().contains(item))
      break;
  }

  if (tool_p)
  {
    QString res=tool_p->uninstallTool(item,directory_p->text());
    if (res!=QString::null)
      QMessageBox::information(this,tr("Uninstallation"),res);
  }

  updateView();
}


void WMainUI::on_install_p_clicked()
{
  const QListWidgetItem * item_p=list_p->currentItem();
  if (item_p==NULL)
    return;
  QString item=item_p->text();
  QList<Tools*>::Iterator it;
  Tools *tool_p=NULL;
  for (it=tools.begin();it!=tools.end();++it)
  {
    tool_p=*it;
    if (tool_p->toolList().contains(item))
      break;
  }

  if (tool_p)
  {
    QString res=tool_p->installTool(item,directory_p->text());
    if (res!=QString::null)
      QMessageBox::information(this,tr("Installation"),res);
    tool_p->createTutorial(item,directory_p->text(),"tutorial\\build.bat");
  }

  updateView();
}


void WMainUI::on_set_console_p_clicked()
{
  const QListWidgetItem * item_p=list_p->currentItem();
  if (item_p==NULL)
    return;
  QString item=item_p->text();
  QList<Tools*>::Iterator it;
  Tools *tool_p=NULL;
  for (it=tools.begin();it!=tools.end();++it)
  {
    tool_p=*it;
    if (tool_p->toolList().contains(item))
      break;
  }

  if (tool_p)
  {
    QString res=tool_p->createConsole(item,directory_p->text(),"testcocoonenv.bat",false);
    if (res!=QString::null)
      QMessageBox::information(this,tr("Build Environment Selection"),res);
    tool_p->createTutorial(item,directory_p->text(),"tutorial\\build.bat");
  }

  updateView();
}


void WMainUI::on_directory_p_clicked()
{
  QString d=QFileDialog::getExistingDirectory(this,directory_label_p->text(),directory_p->text());
  if (d!=QString::null)
  {
    const QListWidgetItem * item_p=list_p->currentItem();
    if (item_p==NULL)
      return;
    QString item=item_p->text();
    QList<Tools*>::Iterator it;
    Tools *tool_p=NULL;
    for (it=tools.begin();it!=tools.end();++it)
    {
      tool_p=*it;
      if (tool_p->toolList().contains(item))
        break;
    }

    if (tool_p)
      tool_p->setDirectory(d);

    directory_p->setText(d);
  }
  updateView();
}


void WMainUI::on_open_console_p_clicked()
{
  const QListWidgetItem * item_p=list_p->currentItem();
  if (item_p==NULL)
    return;
  QString item=item_p->text();
  QList<Tools*>::Iterator it;
  Tools *tool_p=NULL;
  for (it=tools.begin();it!=tools.end();++it)
  {
    tool_p=*it;
    if (tool_p->toolList().contains(item))
      break;
  }

  if (tool_p)
  {
    QString res=tool_p->createConsole(item,directory_p->text(),"testcocoonenv.bat",false);
    if (res!=QString::null)
    {
      hide();
      system("testcocoonconsole.bat");
      show();
    }
    tool_p->createTutorial(item,directory_p->text(),"tutorial\\build.bat");
  }
  updateView();
}


void WMainUI::on_list_p_currentItemChanged ( QListWidgetItem * /*current*/, QListWidgetItem * /*previous*/ )
{
  updateView();
}
