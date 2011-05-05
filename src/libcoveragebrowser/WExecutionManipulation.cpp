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

#include "WExecutionManipulation.h"
#include <QFileDialog>
#include <QPushButton>
#include "options.h"
#include "csmesbackgroundcomputations.h"
#include "csmesundoredoframework.h"
#include "MatchExpr.h"
#if QT_COMPLETOR
#include <QCompleter>
#endif

WExecutionManipulation::WExecutionManipulation(const CSMesUndoRedoFramework *c_p,QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent,  fl)
{
   unique_execution_output=false;
  csmes_p=c_p;
  setupUi(this);

#if QT_COMPLETOR
  QStringList lru=Options::get_opt_strlst(QString(),"EXECUTION_MANIPULATION_SOURCE_COMPLETION");
  source_completor_p = new QCompleter(source_p);
  source_completor_model_p=new QStringListModel(lru,source_completor_p);
  source_completor_p->setModel(source_completor_model_p);
  source_completor_p->setModelSorting(QCompleter::UnsortedModel);
  source_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  source_p->setCompleter( source_completor_p );

  lru=Options::get_opt_strlst(QString(),"EXECUTION_MANIPULATION_DESTINATION_COMPLETION");
  QStringList namelist=c_p->executionList() ;
  lru=appendLRUatEnd(namelist,lru);
  destination_completor_p = new QCompleter(destination_p);
  destination_completor_model_p=new QStringListModel(lru,destination_completor_p);
  destination_completor_p->setModel(destination_completor_model_p);
  destination_completor_p->setModelSorting(QCompleter::UnsortedModel);
  destination_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  destination_p->setCompleter( destination_completor_p );
#endif

  source_p->setText(Options::get_opt_str(QString(),"EXECUTION_MANIPULATION_SOURCE",QString()));
  destination_p->setText(Options::get_opt_str(QString(),"EXECUTION_MANIPULATION_DESTINATION",QString()));
  checkInput();
}

/*
 *  Destroys the object and frees any allocated resources
 */
WExecutionManipulation::~WExecutionManipulation()
{
#if QT_COMPLETOR
  QStringList old_lru=Options::get_opt_strlst(QString(),"EXECUTION_MANIPULATION_SOURCE_COMPLETION");
  QStringList new_lru=appendLRU(source_p->text(),old_lru);
  Options::set_opt(QString(),"EXECUTION_MANIPULATION_SOURCE_COMPLETION",new_lru);

  old_lru=Options::get_opt_strlst(QString(),"EXECUTION_MANIPULATION_DESTINATION_COMPLETION");
  new_lru=appendLRU(destination_p->text(),old_lru);
  Options::set_opt(QString(),"EXECUTION_MANIPULATION_DESTINATION_COMPLETION",new_lru);
#endif

  Options::set_opt(QString(),"EXECUTION_MANIPULATION_SOURCE_COMPLETION",source_p->text());
  Options::set_opt(QString(),"EXECUTION_MANIPULATION_DESTINATION_COMPLETION",destination_p->text());

  // no need to delete child widgets, Qt does it all for us
#if QT_COMPLETOR
  delete destination_completor_p;
  delete source_completor_p;
  //delete destination_completor_model_p;
  //delete source_completor_model_p;
#endif
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void WExecutionManipulation::languageChange()
{
    retranslateUi(this);
}


#if QT_COMPLETOR
QStringList WExecutionManipulation::appendLRU(const QString &value,QStringList &old_lru)
{
  QStringList new_lru;
  old_lru.removeAll(value);
  new_lru += value ;
  new_lru += old_lru ;
  while (new_lru.count()>LRU_MAX_ITEM)
    new_lru.removeLast();
  return new_lru;
}

QStringList WExecutionManipulation::appendLRUatEnd(const QStringList &values,QStringList &old_lru)
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


void WExecutionManipulation::on_destination_p_textChanged( const QString & )
{
  checkInput();
}

QString WExecutionManipulation::source() const
{
  return source_p->text();
}

QString WExecutionManipulation::destination() const
{
  return destination_p->text();
}

void WExecutionManipulation::on_source_p_textChanged( const QString & )
{
  checkInput();
}

void WExecutionManipulation::checkInput()
{
   generateSourceDestinationList() ;
   updatePreview();
}

void WExecutionManipulation::setSourceName(const QString &n)
{
  source_label_p->setShown(!n.isEmpty());
  source_p->setShown(!n.isEmpty());
  source_label_p->setText(n+":");
  _source=n;
}

void WExecutionManipulation::setDestinationName(const QString &n)
{
  destination_label_p->setShown(!n.isEmpty());
  destination_p->setShown(!n.isEmpty());
  destination_label_p->setText(n+":");
  _destination=n;
}


QStringList WExecutionManipulation::sourceList() const
{
  return _sourceList;
}

QStringList WExecutionManipulation::destinationList() const
{
  return _destinationList;
}

void WExecutionManipulation::updatePreview() 
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  QStringList executions=csmes_p->executionList();
   message_p->setText(QString());
   preview_p->clear();
   if ((!_destination.isEmpty()) && (!unique_execution_output))
   {
      preview_p->setColumnCount(2);
      QTableWidgetItem *__colItem = new QTableWidgetItem();
      __colItem->setText(_destination);
      preview_p->setHorizontalHeaderItem(1, __colItem);
   }
   else
   {
      preview_p->setColumnCount(1);
   }
   QTableWidgetItem *__colItem = new QTableWidgetItem();
   __colItem->setText(_source);
   preview_p->setHorizontalHeaderItem(0, __colItem);

   int count=_sourceList.count();
   preview_p->setRowCount(count);
   for (int i=0;i<count;i++)
   {
      QTableWidgetItem *item_p = new QTableWidgetItem();
      item_p->setText(_sourceList.at(i));
      preview_p->setItem(i,0,item_p);
   }
   if (unique_execution_output)
   {
      bool errors=false;
      QString explanation;
      if (executions.contains(_destinationList.at(0)))
      {
         QString msg=tr("'%1' is already existing. Overwriting is not possible.").arg(_destinationList.at(0));
         message_p->setText(msg);
         errors=true;
      }
      else if (!CSMesUndoRedoFramework::executionNameValid(_destinationList.at(0),explanation))
      {
         QString msg=tr("'%1' is not a valid execution name.").arg(_destinationList.at(0));
         message_p->setText(msg+" ["+explanation+"]");
         errors=true;
      }
      
      buttons_p->button(QDialogButtonBox::Ok)->setEnabled(!errors);
   }
   else if (!_destination.isEmpty())
   {
     bool errors=false;
      for (int i=0;i<count;i++)
      {
         QString explanation;
         QTableWidgetItem *item_p = new QTableWidgetItem();
         item_p->setText(_destinationList.at(i));
         if (_destinationList.count(_destinationList.at(i))>1)
         {
            item_p->setForeground(COL_RED);
            QFont f=item_p->font();
            f.setBold(true);
            item_p->setFont(f);
            QString msg=tr("Error, line %1: '%2' is not unique").arg(i+1).arg(_destinationList.at(i));
            item_p->setToolTip(msg);
            if (!errors)
              message_p->setText(msg);
           errors=true;
         }
         else if (executions.contains(_destinationList.at(i)))
         {
            item_p->setForeground(COL_RED);
            QFont f=item_p->font();
            f.setBold(true);
            item_p->setFont(f);
            QString msg=tr("Error, line %1: '%2' is already existing. Overwriting is not possible.").arg(i+1).arg(_destinationList.at(i));
            item_p->setToolTip(msg);
            if (!errors)
              message_p->setText(msg);
           errors=true;
         }
         else if (!CSMesUndoRedoFramework::executionNameValid(_destinationList.at(i),explanation))
         {
            item_p->setForeground(COL_RED);
            QFont f=item_p->font();
            f.setBold(true);
            item_p->setFont(f);
            QString msg=tr("Error, line %1: '%2' is not a valid execution name.").arg(i+1).arg(_destinationList.at(i));
            item_p->setToolTip(msg+" ["+explanation+"]");
            if (!errors)
              message_p->setText(msg+" ["+explanation+"]");
           errors=true;
         }
         if (item_p->text().isEmpty())
         {
           item_p->setText(tr("<EMPTY>"));
           QFont f=item_p->font();
           f.setItalic(true);
           item_p->setFont(f);
         }
         preview_p->setItem(i,1,item_p);
      }
      buttons_p->button(QDialogButtonBox::Ok)->setEnabled(!errors);
   }
   preview_p->resizeColumnsToContents ();

}

void WExecutionManipulation::generateSourceDestinationList() 
{
  _destinationList.clear();
  if (unique_execution_output)
     _destinationList.append(destination_p->text());

  _sourceList.clear();
  if (csmes_p)
  {
    QStringList executions=csmes_p->executionList();
    MatchExpr expr;
    expr.setPattern(source_p->text());
    
    for (QStringList::const_iterator it=executions.begin();
        it!=executions.end();
        ++it)
    {
      if (expr.exactMatch(*it))
      {
         _sourceList.append(*it);
         if ((!_destination.isEmpty()) && (!unique_execution_output))
         {
            QString newItem=expr.replace(destination_p->text());
            _destinationList.append(newItem);
         }
      }
    }
    
  }
}
