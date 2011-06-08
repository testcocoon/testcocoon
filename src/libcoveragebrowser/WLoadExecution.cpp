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

#include "WLoadExecution.h"
#include <QFileDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include "options.h"
#include "csmesundoredoframework.h"
#if QT_COMPLETOR
#include <QCompleter>
#endif

static QString csexeFromCsmes(const QString &cexeFile);
WLoadExecution::WLoadExecution(const CSMesUndoRedoFramework *c_p,QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent,  fl)
{
  setupUi(this);
  apply_only=false;
  QPushButton *ok_p= buttons_p->button(QDialogButtonBox::Ok);
  ok_p->setText(tr("Import"));
  connect(buttons_p, SIGNAL(accepted()), this, SLOT(process()));
  connect(buttons_p, SIGNAL(rejected()), this, SLOT(reject()));
  QPushButton* applyButton = buttons_p->button(QDialogButtonBox::Apply);
  connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

#if QT_COMPLETOR
  QStringList lru=Options::get_opt_strlst(QString(),"/CSEXE_FILE_COMPLETION");
  lru=appendLRU(csexeFromCsmes(c_p->getFilename()),lru);
  file_completor_p = new QCompleter(file_p);
  file_completor_model_p=new QStringListModel(lru,file_completor_p);
  file_completor_p->setModel(file_completor_model_p);
  file_completor_p->setModelSorting(QCompleter::UnsortedModel);
  file_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  file_p->setCompleter( file_completor_p );

  lru=Options::get_opt_strlst(QString(),"/CSEXE_NAME_COMPLETION");
  ExecutionNames namelist=c_p->executionList() ;
  lru=appendLRUatEnd(namelist.toQStringList(),lru);
  name_completor_p = new QCompleter(name_p);
  name_completor_model_p=new QStringListModel(lru,name_completor_p);
  name_completor_p->setModel(name_completor_model_p);
  name_completor_p->setModelSorting(QCompleter::UnsortedModel);
  name_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  name_p->setCompleter( name_completor_p );

  lru=Options::get_opt_strlst(QString(),"/CSEXE_SCRIPT_COMPLETION");
  script_completor_p = new QCompleter(script_p);
  script_completor_model_p=new QStringListModel(lru,script_completor_p);
  script_completor_p->setModel(script_completor_model_p);
  script_completor_p->setModelSorting(QCompleter::UnsortedModel);
  script_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  script_p->setCompleter( script_completor_p );

  lru=Options::get_opt_strlst(QString(),"/CSEXE_SCRIPT_ARGS_COMPLETION");
  arguments_completor_p = new QCompleter(arguments_p);
  arguments_completor_model_p=new QStringListModel(lru,arguments_completor_p);
  arguments_completor_p->setModel(arguments_completor_model_p);
  arguments_completor_p->setModelSorting(QCompleter::UnsortedModel);
  arguments_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  arguments_p->setCompleter( arguments_completor_p );
#endif

  display_import_summary_p->setChecked(Options::get_opt_bool(QString(),"DISPLAY_IMPORT_SUMMARY",true));
  file_p->setText(Options::get_opt_str(QString(),"/CSEXE_FILE",QString()));
  name_p->setText(Options::get_opt_str(QString(),"/CSEXE_NAME",tr("<no name>")));
  delete_p->setChecked(Options::get_opt_bool(QString(),"/CSEXE_DELETE"));
  minimize_memory_usage_p->setChecked(Options::get_opt_bool(QString(),"/CSEXE_MINIMIZE_MEMORY_USAGE",false));
  import_preprocessing_p->setCurrentIndex(Options::get_opt_int(QString(),"/CSEXE_DUPLICATES"));
  script_p->setText(Options::get_opt_str(QString(),"/CSEXE_SCRIPT",QString()));
  arguments_p->setText(Options::get_opt_str(QString(),"/CSEXE_SCRIPT_ARGS",QString()));
  watch_p->setCurrentIndex(Options::get_opt_int(QString(),"/CSEXE_WATCH"));
  type_t t=static_cast<type_t>(Options::get_opt_int(QString(),"/CSEXE_TYPE"));
  switch (t)
  {
    case CSEXE_FILE:
      select_file_p->setChecked(true);
      break;
    default:
      select_script_p->setChecked(true);
      break;
  }
  checkInput();
}

/*
 *  Destroys the object and frees any allocated resources
 */
WLoadExecution::~WLoadExecution()
{
#if QT_COMPLETOR
  QStringList old_lru=Options::get_opt_strlst(QString(),"/CSEXE_FILE_COMPLETION");
  QStringList new_lru=appendLRU(file_p->text(),old_lru);
  Options::set_opt(QString(),"/CSEXE_FILE_COMPLETION",new_lru);

  old_lru=Options::get_opt_strlst(QString(),"/CSEXE_NAME_COMPLETION");
  new_lru=appendLRU(name_p->text(),old_lru);
  Options::set_opt(QString(),"/CSEXE_NAME_COMPLETION",new_lru);

  old_lru=Options::get_opt_strlst(QString(),"/CSEXE_SCRIPT_COMPLETION");
  new_lru=appendLRU(script_p->text(),old_lru);
  Options::set_opt(QString(),"/CSEXE_SCRIPT_COMPLETION",new_lru);

  old_lru=Options::get_opt_strlst(QString(),"/CSEXE_SCRIPT_ARGS_COMPLETION");
  new_lru=appendLRU(arguments_p->text(),old_lru);
  Options::set_opt(QString(),"/CSEXE_SCRIPT_ARGS_COMPLETION",new_lru);
#endif

  Options::set_opt(QString(),"/CSEXE_FILE",file_p->text());
  Options::set_opt(QString(),"/CSEXE_NAME",name_p->text());
  Options::set_opt(QString(),"/CSEXE_SCRIPT",script_p->text());
  Options::set_opt(QString(),"/CSEXE_SCRIPT_ARGS",arguments_p->text());
  Options::set_opt(QString(),"/CSEXE_TYPE",static_cast<int>(type()));
  Options::set_opt(QString(),"/CSEXE_DELETE",delete_p->isChecked());
  Options::set_opt(QString(),"/CSEXE_MINIMIZE_MEMORY_USAGE",minimize_memory_usage_p->isChecked());
  Options::set_opt(QString(),"/CSEXE_WATCH",watch_p->currentIndex());
  Options::set_opt(QString(),"/CSEXE_DUPLICATES",import_preprocessing_p->currentIndex());

  // no need to delete child widgets, Qt does it all for us
#if QT_COMPLETOR
  delete name_completor_p;
  delete file_completor_p;
  delete script_completor_p;
  delete arguments_completor_p;
  //delete arguments_completor_model_p;
  //delete script_completor_model_p;
  //delete name_completor_model_p;
  //delete file_completor_model_p;
#endif
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void WLoadExecution::languageChange()
{
    retranslateUi(this);
}


#if QT_COMPLETOR
QStringList WLoadExecution::appendLRU(const QString &value,QStringList &old_lru)
{
  QStringList new_lru;
  old_lru.removeAll(value);
  new_lru += value ;
  new_lru += old_lru ;
  while (new_lru.count()>LRU_MAX_ITEM)
    new_lru.removeLast();
  return new_lru;
}

QStringList WLoadExecution::appendLRUatEnd(const QStringList &values,QStringList &old_lru)
{
  QStringList new_lru=old_lru;
  for (QStringList::const_iterator value=values.begin();value!=values.end();++value)
  {
    if (new_lru.count()>=LRU_MAX_ITEM)
      return new_lru;

    if (!new_lru.contains(*value))
    {
      new_lru += *value;
    }
  }
  return new_lru;
}
#endif


void WLoadExecution::on_select_file_p_toggled(bool )
{
  checkInput();
}

void WLoadExecution::on_display_import_summary_p_toggled(bool value)
{
  Options::set_opt(QString(),"DISPLAY_IMPORT_SUMMARY",value);
}

void WLoadExecution::on_browse_script_p_clicked()
{
  QString f;
  QString selectedFilters=Options::get_opt_str(QString(),"BROWSE_SCRIPT_FILTER",QString());
  if ( (f = QFileDialog::getOpenFileName(this,
          tr("Executable"),
          script_p->text(),
#ifdef OS_WIN32
          tr("Applications (*.exe);;Batch files (*.bat);;All files (*)"),
#else
          tr("Scripts (*.sh);;All files (*)"),
#endif
          &selectedFilters,
          FILE_DIALOG_OPTIONS
          )) != QString::null )
  {
    Options::set_opt(QString(),"BROWSE_SCRIPT_FILTER",selectedFilters);
    script_p->setText(f);
  }
}

void WLoadExecution::on_browse_p_clicked()
{
  QString f;
  QString selectedFilters=Options::get_opt_str(QString(),"BROWSE_FILE_FILTER",QString());
  if ( (f = QFileDialog::getOpenFileName(this,
          tr("Open Execution Report"),
          file_p->text(),
#ifdef OS_WIN32
          tr("Executions (*.exe.csexe);;Executions (*.dll.csexe);;Executions (*.csexe);;All files (*)"),
#else
          tr("Executions (*.csexe);;All files (*)"),
#endif
          &selectedFilters,
          FILE_DIALOG_OPTIONS
          )) != QString::null )
  {
    Options::set_opt(QString(),"BROWSE_FILE_FILTER",selectedFilters);
    file_p->setText(f);
  }
}

void WLoadExecution::on_name_p_textChanged( const QString & )
{
  checkInput();
}

QString WLoadExecution::file() const
{
  ASSERT(type()==CSEXE_FILE);
  return file_p->text();
}

QString WLoadExecution::name() const
{
  return name_p->text();
}

WLoadExecution::watch_t WLoadExecution::watch() const
{
  ASSERT(type()==CSEXE_FILE);
  return static_cast<watch_t>(watch_p->currentIndex());
}


bool WLoadExecution::deleteAfterLoading() const
{
  ASSERT(type()==CSEXE_FILE);
  return delete_p->isChecked();
}

void WLoadExecution::on_file_p_textChanged( const QString & )
{
  checkInput();
}

void WLoadExecution::on_script_p_textChanged( const QString & )
{
  checkInput();
}

void WLoadExecution::checkInput()
{
   bool ok=true;
   QString explanation;
   type_t t=type();
   switch (t)
   {
   case CSEXE_FILE:
      {
         QFileInfo f(file());
         if (!f.exists())
         {
            explanation=tr("File '%1' not existing").arg(file());
            ok=false;
         }
      }
      break;
   case CSEXE_SCRIPT:
      if (script().length()==0)
      {
         explanation=tr("No script defined");
         ok=false;
      }
      else
      {
         QFileInfo f(script());
         if (!f.exists())
         {
            explanation=tr("Script '%1' not existing").arg(script());
            ok=false;
         }
      }
      break;
   }
   if (ok)
      ok=CSMesUndoRedoFramework::executionNameValid(name(),explanation);

   explanation_p->setVisible(!ok);
   if (!ok)
     explanation_p->setText(explanation);
   QPushButton *ok_p= buttons_p->button(QDialogButtonBox::Ok);
   ok_p->setEnabled(ok);
   QPushButton *apply_p=buttons_p->button(QDialogButtonBox::Apply);
   apply_p->setEnabled(ok);
}

bool WLoadExecution::minimizeMemoryUsage() const
{
  return minimize_memory_usage_p->isChecked();
}

WLoadExecution::type_t WLoadExecution::type() const
{
  ASSERT(select_file_p->isChecked() || select_script_p->isChecked());
  ASSERT(select_file_p->isChecked() != select_script_p->isChecked());
  if (select_file_p->isChecked())
    return CSEXE_FILE;
  else
    return CSEXE_SCRIPT;
}

QString WLoadExecution::script() const
{
  ASSERT(type()==CSEXE_SCRIPT);
  return script_p->text();
}

Executions::execution_status_t WLoadExecution::defaultStatus() const
{
  if (execution_status_to_be_checked_manually_p->isChecked())
    return Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY;
  if (execution_status_failed_p->isChecked())
    return Executions::EXECUTION_STATUS_FAILED;
  if (execution_status_passed_p->isChecked())
    return Executions::EXECUTION_STATUS_PASSED;
  return Executions::EXECUTION_STATUS_UNKNOWN;
}

CSMesUndoRedoFramework::csexe_import_policy_t WLoadExecution::importPolicy() const
{
  switch (import_preprocessing_p->currentIndex())
  {
    case 0: return CSMesUndoRedoFramework::CSEXE_POLICY_IGNORE_DUPLICATES;
    case 1: return CSMesUndoRedoFramework::CSEXE_POLICY_IMPORT_DUPLICATES;
    case 2: return CSMesUndoRedoFramework::CSEXE_POLICY_IMPORT_DUPLICATES_AND_EMPTY;
    case 3: return CSMesUndoRedoFramework::CSEXE_POLICY_MERGE_ALL_TOGETHER;
    default:
            ASSERT(false);
            return CSMesUndoRedoFramework::CSEXE_POLICY_IGNORE_DUPLICATES;
  }
}

bool WLoadExecution::applyOnly() const
{
  return apply_only;
}

void WLoadExecution::apply()
{
  apply_only=true;
  accept();
}

static QString csexeFromCsmes(const QString &csmesFile)
{
  QString csexeFile=csmesFile;
#ifdef OS_WIN32
  csexeFile.replace(".csmes",".csexe",Qt::CaseInsensitive);
#else
  csexeFile.replace(".csmes",".csexe",Qt::CaseSensitive);
#endif
  QFileInfo (f);
  if (f.exists())
    return csexeFile;
  else
    return QString();
}


QStringList WLoadExecution::arguments() const
{
  ASSERT(type()==CSEXE_SCRIPT);
  QString args=arguments_p->text();
  int args_lg=args.size();
  QStringList args_list;
  enum { PROCESS_ARG, BETWEEN_ARG, IGNORE_ONE_CHAR, IGNORE_STRING } mode,last_mode;
  mode=BETWEEN_ARG;
  last_mode=mode;
  QString current_arg;
  for (int i=0;i<args_lg;i++)
  {
    const QChar &c=args.at(i);
    if (c==QChar(' '))
    {
      switch (mode)
      {
        case PROCESS_ARG:
          args_list+=current_arg;
          current_arg.clear();
          mode=BETWEEN_ARG;
          break;
        case IGNORE_STRING:
          current_arg+=c;
          break;
        case IGNORE_ONE_CHAR:
          current_arg+=c;
          mode=last_mode ;
          break;
        case BETWEEN_ARG:
          break;
      }
    }
#if defined( OS_LINUX ) || defined(OS_MACX)
    else if (c==QChar('\\'))
    {
      switch (mode)
      {
        case PROCESS_ARG:
        case IGNORE_STRING:
        case BETWEEN_ARG:
          last_mode=mode;
          mode=IGNORE_ONE_CHAR;
          break;
        case IGNORE_ONE_CHAR:
          current_arg+=c;
          mode=last_mode ;
          break;
      }
    }
#endif
    else if (c==QChar('"'))
    {
      switch (mode)
      {
        case BETWEEN_ARG:
        case PROCESS_ARG:
          mode=IGNORE_STRING;
          break;
        case IGNORE_STRING:
          mode=PROCESS_ARG;
          break;
        case IGNORE_ONE_CHAR:
          break;
      }
    }
    else
    {
      if (mode==BETWEEN_ARG)
        mode=PROCESS_ARG;
      current_arg+=c;
    }
  }
  switch (mode)
  {
    case IGNORE_ONE_CHAR:
    case IGNORE_STRING:
    case PROCESS_ARG:
      args_list+=current_arg;
      break;
    case BETWEEN_ARG:
      break;
  }
  return args_list;
}

void WLoadExecution::process ()
{
  accept();
}
