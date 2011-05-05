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

#include "WExecutionTextDump.h"
#include <QFileDialog>
#include <QPushButton>
#include "options.h"
#include "csmesundoredoframework.h"
#if QT_COMPLETOR
#include <QCompleter>
#endif


WExecutionTextDump::WExecutionTextDump(const CSMesUndoRedoFramework *c_p,QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent,  fl)
{
  setupUi(this);

  connect(buttons_p, SIGNAL(accepted()), this, SLOT(process()));
  connect(buttons_p, SIGNAL(rejected()), this, SLOT(reject()));
  QPushButton* applyButton = buttons_p->button(QDialogButtonBox::Apply);
  connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
  file_p->setText(Options::get_opt_str(QString(),"TEXT_DUMP_FILE_NAME",file_p->text()));
  unexecuted_format_p->setText(Options::get_opt_str(QString(),"TEXT_DUMP_UNEXECUTED_FORMAT",unexecuted_format_p->text()));
  executed_format_p->setText(Options::get_opt_str(QString(),"TEXT_DUMP_EXECUTED_FORMAT",executed_format_p->text()));
#if QT_COMPLETOR
  QStringList lru=Options::get_opt_strlst(QString(),"/TEXT_DUMP_EXECUTED_FORMAT_COMPLETION");
  executed_format_completor_p = new QCompleter(executed_format_p);
  executed_format_completor_model_p=new QStringListModel(lru,executed_format_completor_p);
  executed_format_completor_p->setModel(executed_format_completor_model_p);
  executed_format_completor_p->setModelSorting(QCompleter::UnsortedModel);
  executed_format_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  executed_format_p->setCompleter( executed_format_completor_p );

  lru=Options::get_opt_strlst(QString(),"/TEXT_DUMP_UNEXECUTED_FORMAT_COMPLETION");
  unexecuted_format_completor_p = new QCompleter(unexecuted_format_p);
  unexecuted_format_completor_model_p=new QStringListModel(lru,unexecuted_format_completor_p);
  unexecuted_format_completor_p->setModel(unexecuted_format_completor_model_p);
  unexecuted_format_completor_p->setModelSorting(QCompleter::UnsortedModel);
  unexecuted_format_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  unexecuted_format_p->setCompleter( unexecuted_format_completor_p );

  lru=Options::get_opt_strlst(QString(),"TEXT_DUMP_FILE_NAME_HISTORY");
  file_completor_p = new QCompleter(file_p);
  file_completor_model_p=new QStringListModel(lru,file_completor_p);
  file_completor_p->setModel(file_completor_model_p);
  file_completor_p->setModelSorting(QCompleter::UnsortedModel);
  file_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  file_p->setCompleter( file_completor_p );
#endif

  checkInput();
}

/*
 *  Destroys the object and frees any allocated resources
 */
WExecutionTextDump::~WExecutionTextDump()
{
}

void WExecutionTextDump::saveSettings()
{
#if QT_COMPLETOR
  QStringList old_lru=Options::get_opt_strlst(QString(),"/TEXT_DUMP_EXECUTED_FORMAT_COMPLETION");
  QStringList new_lru=appendLRU(executed_format_p->text(),old_lru);
  Options::set_opt(QString(),"/TEXT_DUMP_EXECUTED_FORMAT_COMPLETION",new_lru);

  old_lru=Options::get_opt_strlst(QString(),"/TEXT_DUMP_UNEXECUTED_FORMAT_COMPLETION");
  new_lru=appendLRU(unexecuted_format_p->text(),old_lru);
  Options::set_opt(QString(),"/TEXT_DUMP_UNEXECUTED_FORMAT_COMPLETION",new_lru);
  old_lru=Options::get_opt_strlst(QString(),"/TEXT_DUMP_FILE_NAME_HISTORY");
  new_lru=appendLRU(file_p->text(),old_lru);
  Options::set_opt(QString(),"/TEXT_DUMP_FILE_NAME_HISTORY",new_lru);
#endif

  Options::set_opt(QString(),"/TEXT_DUMP_FILE_NAME",file_p->text());
  Options::set_opt(QString(),"/TEXT_DUMP_UNEXECUTED_FORMAT",unexecuted_format_p->text());
  Options::set_opt(QString(),"/TEXT_DUMP_EXECUTED_FORMAT",executed_format_p->text());

  // no need to delete child widgets, Qt does it all for us
#if QT_COMPLETOR
  delete unexecuted_format_completor_p;
  delete executed_format_completor_p;
  delete file_completor_p;
#endif
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void WExecutionTextDump::languageChange()
{
    retranslateUi(this);
}


#if QT_COMPLETOR
QStringList WExecutionTextDump::appendLRU(const QString &value,QStringList &old_lru)
{
  QStringList new_lru;
  old_lru.removeAll(value);
  new_lru += value ;
  new_lru += old_lru ;
  while (new_lru.count()>LRU_MAX_ITEM)
    new_lru.removeLast();
  return new_lru;
}
#endif

void WExecutionTextDump::on_unexecuted_format_p_textChanged( const QString & )
{
  checkInput();
}

void WExecutionTextDump::on_file_p_textChanged( const QString & )
{
  checkInput();
}

void WExecutionTextDump::on_executed_format_p_textChanged( const QString & )
{
  checkInput();
}

void WExecutionTextDump::checkInput()
{
  bool ok_enable=((!unexecuted_format_p->text().isEmpty() || !executed_format_p->text().isEmpty()) && !file_p->text().isEmpty());
  QPushButton *ok_p=buttons_p->button(QDialogButtonBox::Ok);
  ok_p->setEnabled(ok_enable);
  QPushButton *apply_p=buttons_p->button(QDialogButtonBox::Apply);
  apply_p->setEnabled(ok_enable);
}

void WExecutionTextDump::apply()
{
  saveSettings();
}

QString WExecutionTextDump::fileName() const 
{
  return file_p->text();
}

QString WExecutionTextDump::formatExecuted() const
{
  return executed_format_p->text();
}

QString WExecutionTextDump::formatUnexecuted() const
{
  return unexecuted_format_p->text();
}

void WExecutionTextDump::on_file_browse_p_clicked()
{
   QString dir=Options::get_opt_str(QString(),"TEXT_DUMP_OUTPUT_DIR",QString());
   QString selectedFilter=Options::get_opt_str(QString(),"TEXT_DUMP_OUTPUT_SELECTED_FILTER",QString());
   QString title=tr("Text File Name");

   QString filter;
   filter=tr("text (*.txt *.log)");
   filter+=";;";
   filter+=tr("All (*)");

   QString filename=QFileDialog::getSaveFileName ( this, title , dir , filter , &selectedFilter , FILE_DIALOG_OPTIONS) ;

   if (filename!=QString())
   {
      Options::set_opt(QString(),"TEXT_DUMP_OUTPUT_DIR",dir);
      Options::set_opt(QString(),"TEXT_DUMP_OUTPUT_SELECTED_FILTER",selectedFilter);
      file_p->setText(filename);
   }
}


void WExecutionTextDump::process ()
{
  saveSettings();
  accept();
}

