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

#include "WExplanation.h"
#include <QBuffer>
#include "htmlwriter.h"
#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>
#include "csmesbackgroundcomputations.h"
#include "csmesundoredoframework.h"
#include "CoverageSettings.h"
#include "coveragebrowserpdef.h"
#include "helpclient.h"
#include "options.h"

WExplanation::WExplanation(QWidget* parent,  Qt::WindowFlags fl)
    : QWidget(parent, fl)
{
    setupUi(this);

    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
WExplanation::~WExplanation()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void WExplanation::languageChange()
{
    retranslateUi(this);
}


void WExplanation::init()
{
  QFont f;
  f.fromString(Options::get_opt_str(QString(),"FONT_MAIN",DEF_FONT_MAIN));
  setFont(f);  
  csmes_p=NULL;
  //text_p->setTextFormat(Qt::RichText);
  text_p->setReadOnly(true);
  connect( & CSMesBackgroundComputations::GetObject(),
      SIGNAL(explanation(const ModuleFile &,const SourceFile &,const QList<int> &,CSMes::source_type_t ,int ,Instrumentation::coverage_method_t ,int,const QString &)),
      this,
      SLOT(setExplanation(const ModuleFile &,const SourceFile &,const QList<int> &,CSMes::source_type_t ,int ,Instrumentation::coverage_method_t ,int,const QString &)));
}

void WExplanation::destroy()
{
}

void WExplanation::setExplanation(const ModuleFile &module,const SourceFile &source,const QList<int> &lines_indexs,CSMes::source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit,const QString &text)
{
	if ( !_requested_module.isEmpty() &&            _requested_module            != module )
		return;
	if ( !_requested_source.isEmpty() &&            _requested_source            != source)
		return;
	if ( _requested_lines_indexs      != lines_indexs)
		return;
	if ( _requested_source_type       != source_type)
		return;
	if ( _requested_executed_by_limit != executed_by_limit)
		return;
	if ( _requested_coverage_level    != coverage_level)
		return;
	if ( _requested_method            != method)
		return;
	setExplanation(text);
}

void WExplanation::setExplanation(const QString &text)
{
   _requested_module.clear();
   _requested_source.clear();
  text_p->setHtml(text);
}

void WExplanation::setCSMes(CSMesUndoRedoFramework *c_p)
{
  QFont f;
  f.fromString(Options::get_opt_str(QString(),"FONT_MAIN",DEF_FONT_MAIN));
  setFont(f);  
  csmes_p=c_p;
}

void WExplanation::setExplanationOriginal (const QString &module,const QString &source, const QList<int>& lines,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit)
{
  _setExplanation(module,source,lines,CSMes::ORIGINAL,coverage_level,method,executed_by_limit);
}


void WExplanation::setExplanationPreprocessed (const QString &module,const QString &source,const QList<int> &indexs,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit)
{
  _setExplanation(module,source,indexs,CSMes::CSMES,coverage_level,method,executed_by_limit);
}

void WExplanation::_setExplanation(const QString &module,const QString &source,const QList<int> &lines_indexs,CSMes::source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit)
{
  if (csmes_p)
  {
     _requested_module            = module;
     _requested_source            = source;
     _requested_lines_indexs      = lines_indexs;
     _requested_source_type       = source_type;
     _requested_executed_by_limit = executed_by_limit;
     _requested_coverage_level    = coverage_level;
     _requested_method            = method;
     setExplanation(tr("Processing, please wait..."));
     CSMesBackgroundComputations::GetObject().calculateExplanation(module,source,lines_indexs,source_type,coverage_level,method,executed_by_limit);
  }
  else
    setExplanation(QString());
}
