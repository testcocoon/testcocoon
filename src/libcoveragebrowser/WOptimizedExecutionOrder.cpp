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

#include "WOptimizedExecutionOrder.h"
#include <qvariant.h>
#include <qimage.h>
#include <Qt>
#include <QPushButton>
#include <QTextDocument>
#include <QTextStream>
#include <QMessageBox>
#include <QTableWidget>
#include <QFileDialog>
#include <QtDebug>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qregexp.h>
#include "coveragebrowserpdef.h"
#include "options.h"
#include <QMap>

WOptimizedExecutionOrder::WOptimizedExecutionOrder(const QStringList &execution_list,Instrumentation::coverage_method_t method, QWidget* parent, Qt::WindowFlags fl) : QDialog(parent, fl)
{
  setupUi(this);

  _best_nb_tested=-1;
  _optimized_execution_nb_tested=-1;
  _method=method;
  _optimized_execution_list.clear();
  _execution_list=execution_list;
  _under_process.clear();
  int count=_execution_list.count();
  progress_p->setRange(0,count);
  progress_p->setValue(0);
  button_p->button(QDialogButtonBox::Ok)->setEnabled(false);
  button_p->button(QDialogButtonBox::Save)->setEnabled(false);
  button_p->button(QDialogButtonBox::Cancel)->setEnabled(true);
  connect( & CSMesBackgroundComputations::GetObject(),
     SIGNAL(statisticExecution(const QStringList &,const QStringList&,bool,int ,Instrumentation::coverage_method_t,CSMes::comparaison_mode_t,int ,int )),
     this,
     SLOT(statisticExecution(const QStringList &,const QStringList&,bool,int ,Instrumentation::coverage_method_t,CSMes::comparaison_mode_t,int ,int )));
  connect (button_p->button(QDialogButtonBox::Save),SIGNAL(clicked()),
      this,SLOT(save()));
  connect (button_p->button(QDialogButtonBox::Cancel),SIGNAL(clicked()),
      this,SLOT(reject()));
  connect (button_p->button(QDialogButtonBox::Ok),SIGNAL(clicked()),
      this,SLOT(accept()));
}

WOptimizedExecutionOrder::~WOptimizedExecutionOrder()
{
  // no need to delete child widgets, Qt does it all for us
}

void WOptimizedExecutionOrder::calculateStatistics()
{
  _best_nb_tested=-1;
  _under_process.clear();
  _optimized_execution_nb_tested=0;
  continueCalculateStatistics();
}

void WOptimizedExecutionOrder::continueCalculateStatistics()
{
  _finished=true;
  _under_process.clear();
  QMap<int,QStringList> prio_list;
  for (QStringList::const_iterator it=_execution_list.begin();it!=_execution_list.end();++it)
  {
    if (!_optimized_execution_list.contains(*it))
    {
      _under_process.append(*it);
      _finished=false;
      if (_nb_tested_info.contains(*it))
        prio_list[_nb_tested_info[*it]].append(*it);
      else
        prio_list[0].append(*it);
    }
  }


  for (QMap<int,QStringList>::const_iterator itProcessPrio=prio_list.begin();itProcessPrio!=prio_list.end();++itProcessPrio)
  for (QStringList::const_iterator itProcess=itProcessPrio.value().begin();itProcess!=itProcessPrio.value().end();++itProcess)
  {
    QStringList list=_optimized_execution_list;
    list+=*itProcess;
    CSMesBackgroundComputations::GetObject().calculateStatisticExecution(list,QStringList(),false,1,_method,CSMes::COMPARAISON_MODE_NONE);
  }
}


void WOptimizedExecutionOrder::languageChange()
{
  retranslateUi(this);
}

QString WOptimizedExecutionOrder::toHtml() const
{
  int icol,irow;
  QString output;
  QTextStream text(&output);

  text << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\" \"http://www.w3.org/TR/REC-html40/loose.dtd\">" << endl;
  text << "<HTML>" << endl;
  text << "<HEAD>" << endl;
  text << "<TITLE>"+tr("Optimized Execution Order")+"</TITLE>"  << endl;
  text << "</HEAD>"  << endl;
  text << "<BODY >"  << endl;

  text << "<H1>"+tr("Optimized Execution Order")+"</H1>"  << endl;
  text << "<TABLE>"  << endl;
  text << "<TR>"  << endl;
  text << "<TH>" ;
  text << Qt::escape(tr("Order"));
  text << "</TH>"  << endl;
  for (icol = 0 ;icol < execution_table_p->columnCount();icol++)
  {
    text << "<TH>" ;
    text << Qt::escape(execution_table_p->horizontalHeaderItem(icol)->text());
    text << "</TH>"  << endl;
  }
  text << "</TR>"  << endl;
  for (irow = 0 ;irow < execution_table_p->rowCount();irow++)
  {
    text << "<TR>" ;
    text << "<TD>" ;
    text << QString::number(irow+1);
    text << "</TD>"  << endl;
    for (icol = 0 ;icol < execution_table_p->columnCount();icol++)
    {
      text << "<TD>" ;
      text << Qt::escape(execution_table_p->item(irow,icol)->text());
      text << "</TD>"  << endl;
    }
    text << "</TR>"  << endl;
  }
  text << "</TABLE>"  << endl;
  text << "</BODY >"  << endl;

  return output;
}

void WOptimizedExecutionOrder::save()
{
  QString selectedFilter;
  QString filename=QFileDialog::getSaveFileName ( this , tr("Saving execution order..."), Options::get_opt_str(QString(),"EXECUTION_ORDER_FILE",""),
      tr("HTML File") + " (*.htm*)",
      &selectedFilter,
      FILE_DIALOG_OPTIONS
      );
  if (filename!=QString())
  {
    Options::set_opt(QString(),"EXECUTION_ORDER_FILE",filename);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
      QMessageBox::warning(this,tr("Error"),tr("Could not open file '%1'.").arg(filename), QMessageBox::Ok,QMessageBox::NoButton);
      return;
    }
    QTextStream text(&file);
    text << toHtml();
    file.close();
  }
}

void WOptimizedExecutionOrder::statisticExecution(const QStringList &mes,const QStringList &comparaisons,bool execution_analysis,int coverage_level,Instrumentation::coverage_method_t method,CSMes::comparaison_mode_t comparaison_mode,int nb_tested,int nb_untested)
{
  if (_method!=method)
    return ;
  if (coverage_level!=1)
    return ;
  if (execution_analysis)
    return;
  if (comparaison_mode!=CSMes::COMPARAISON_MODE_NONE)
    return;
  if (!comparaisons.isEmpty())
    return;
  QStringList executions;

  if (mes.count()!=_optimized_execution_list.count()+1)
    return;
  for (QStringList::const_iterator it=mes.begin();it!=mes.end();++it)
  {
    if (!_execution_list.contains(*it))
      return;
    if (!_optimized_execution_list.contains(*it))
      executions += *it;
  }
  if (executions.count()!=1)
    return;
  QString current_execution=executions.at(0);

  if ( (nb_tested>_best_nb_tested)
        ||
        ( 
         nb_tested==_best_nb_tested 
         && 
         _best_execution<current_execution
        )
     )
  {
    execution_table_p->setRowCount(mes.count());
    execution_table_p->setItem(mes.count()-1,0,new QTableWidgetItem(current_execution));
    execution_table_p->setItem(mes.count()-1,1,new QTableWidgetItem(QString::number(static_cast<double>(nb_tested)/static_cast<double>(nb_tested+nb_untested)*100.0,'F',6)+"%"));
    execution_table_p->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
    execution_table_p->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    execution_table_p->scrollToBottom();
    if (nb_tested>_best_nb_tested)
       cancelRequestNbTestedBelow(nb_tested);
    _best_nb_tested=nb_tested;
    _best_execution=current_execution;
  }
  if (nb_tested==_optimized_execution_nb_tested)
    removeExecution(current_execution);
  else
     setNbTestedInfo(current_execution,nb_tested);
  _under_process.removeAll(current_execution);
  if (_under_process.isEmpty())
  {
     progress_p->setValue(progress_p->value()+1);
     _nb_tested_info.remove(_best_execution);
    _optimized_execution_list+=_best_execution;
    _optimized_execution_nb_tested=_best_nb_tested;
    if (_execution_list.count()==_optimized_execution_list.count())
    {
      progress_label_p->hide();
      progress_p->hide();
      button_p->button(QDialogButtonBox::Ok)->setEnabled(true);
      button_p->button(QDialogButtonBox::Cancel)->setEnabled(false);
      button_p->button(QDialogButtonBox::Save)->setEnabled(true);

      _finished=true;
    }
    else
      continueCalculateStatistics();
  }
}

void WOptimizedExecutionOrder::removeExecution(const QString &e)
{
  int n=_execution_list.removeAll(e);
  if (n>0)
  {
    int count=_execution_list.count();
    progress_p->setRange(0,count);
  }
  _nb_tested_info.remove(e);
}

void WOptimizedExecutionOrder::setNbTestedInfo(const QString &e,int n)
{
   _nb_tested_info[e]=n-_optimized_execution_nb_tested;
}

void WOptimizedExecutionOrder::clearNbTestedInfo()
{
   _nb_tested_info.clear();
}

void WOptimizedExecutionOrder::cancelRequestNbTestedBelow(int n)
{
   int v=n-_optimized_execution_nb_tested;
   for (QMap<QString,int>::const_iterator it=_nb_tested_info.begin();it!=_nb_tested_info.end();++it)
   {
      const int nb_tested_diff=it.value();
      if (nb_tested_diff<v)
      {
         QStringList list=_optimized_execution_list;
         list+=it.key();
         int remove_count=_under_process.removeAll(it.key());
         if (remove_count>0)
            CSMesBackgroundComputations::GetObject().cancelStatisticExecution(list,QStringList(),false,1,_method,CSMes::COMPARAISON_MODE_NONE);
      }
   }
}
