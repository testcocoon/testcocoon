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

#include "WStatistic.h"
#include <QProgressBar>
#include "coveragebrowserpdef.h"
#include "options.h"
#include "Service.h"
#include "csmesundoredoframework.h"
#include <QStyle>
#include <QStyleFactory>
#include "CoverageSettings.h"

WStatistic::WStatistic(QWidget* parent,  Qt::WindowFlags fl)
    : QWidget(parent, fl)
{
  csmes_p=NULL;
  setupUi(this);
  setCoverageLevel(CoverageSettings::object().getCoverageLevel());
  QStyle *bargraph_style_p=QStyleFactory::create("Cleanlooks");
  if (bargraph_style_p)
  {
    coverage_bar_p->setStyle(bargraph_style_p);
    coverage_level1_bar_p->setStyle(bargraph_style_p);
  }
}

WStatistic::~WStatistic()
{
}


void WStatistic::refreshView()
{
  Instrumentation::coverage_method_t  coverage_method = CoverageSettings::object().getCoverageMethod();
  int  coverage_level = CoverageSettings::object().getCoverageLevel();
  setCoverageLevel(coverage_level);

  if ( coverage_method==Instrumentation::METHOD_UNKNOWN)
    return;
  if (csmes_p)
  {
    int nb_tested=-1;
    int nb_untested=-1;
    csmes_p->statistic(coverage_level,coverage_method,nb_tested,nb_untested);
    setCoverageValue(nb_tested,nb_untested);
    if (coverage_level!=1)
    {
      int nb_tested=-1;
      int nb_untested=-1;
      csmes_p->statistic(1,coverage_method,nb_tested,nb_untested);
      setCoverageLevel1Value(nb_tested,nb_untested);
    }
  }
}

void WStatistic::setCSMes(const CSMesUndoRedoFramework *csmes)
{
  csmes_p=csmes;
}

void WStatistic::showLevel1(bool b)
{
   if (coverage_level1_bar_p->isHidden()==b)
   {
      if (b)
      {
         coverage_level1_label_p->show();
         coverage_level1_bar_p->show();
         coverage_level1_value_p->show();
      }
      else
      {
         coverage_level1_label_p->hide();
         coverage_level1_bar_p->hide();
         coverage_level1_value_p->hide();
      }
      adjustSize();
   }
}

void WStatistic::setCoverageLevel(int coverage_level)
{
  if (coverage_level==1)
  {
    showLevel1(false);
    setCoverageText(tr("Coverage:"));
  }
  else
  {
    showLevel1(true);
    setCoverageText(tr("Coverage level %1:").arg(coverage_level));
    setCoverageLevel1Text(tr("Coverage level %1:").arg(1));
  }
}


void WStatistic::setCoverageLevel1Text(const QString &t)
{
   coverage_level1_label_p->setText(t);
}


void WStatistic::setCoverageLevel1Value(int nb_tested,int nb_untested)
{
  double v=-1.0;
  if (nb_tested+nb_untested>0)
    v= static_cast<double>(nb_tested)/static_cast<double>(nb_tested+nb_untested);
  if (v>=0)
    coverage_level1_bar_p->setValue(static_cast<int>(100*v));
  else
    coverage_level1_bar_p->setValue(0);
  coverage_level1_value_p->setText(CSMes::printStat(nb_tested,nb_untested));
}


void WStatistic::setCoverageText(const QString &t)
{
   coverage_label_p->setText(t);
}


void WStatistic::setCoverageValue(int nb_tested,int nb_untested)
{
  double v=-1.0;
  if (nb_tested+nb_untested>0)
    v= static_cast<double>(nb_tested)/static_cast<double>(nb_tested+nb_untested);
  if (v>=0)
    coverage_bar_p->setValue(static_cast<int>(100*v));
  else
    coverage_bar_p->setValue(0);
  coverage_value_p->setText(CSMes::printStat(nb_tested,nb_untested));
}

void WStatistic::on_coverage_level1_label_p_clicked(bool)
{
  refreshView();
}

void WStatistic::on_coverage_label_p_clicked(bool)
{
  refreshView();
}
