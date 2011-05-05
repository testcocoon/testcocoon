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


#include "MatchExpr.h"
#include <QObject>

MatchExpr::MatchExpr() : QRegExp () 
{
}

MatchExpr::MatchExpr ( const QString & pat, Qt::CaseSensitivity cs ) : QRegExp( ) 
{
   setPattern(pat);
   setCaseSensitivity(cs);
}

MatchExpr::MatchExpr ( const MatchExpr & rx ) : QRegExp(rx) 
{
   setPattern(pattern());
}

void MatchExpr::setPattern(const QString &pat)
{
   if (pat.length()>0 && pat.at(0)=='=')
   {
      setPatternSyntax(RegExp2);
      QString newPat=pat.mid(1);
      QRegExp::setPattern(newPat);
   }
   else
   {
      setPatternSyntax(Wildcard);
      QRegExp::setPattern(pat);
   }
}

QString MatchExpr::convertToRegExp(const QString & wild) 
{
  QString regexp="="+wild;
  regexp.replace(".","\\.");
  regexp.replace("?",".");
  regexp.replace("*",".*");
  return regexp;
}

QString MatchExpr::convertToWilcard(const QString & regexp)
{
  QString wild=regexp.mid(1);
  wild.replace(".*","*");
  wild.replace(".","?");
  return wild;
}

QString MatchExpr::replace(const QString &pattern) 
{
   QString newItem=pattern;
   newItem.replace(QRegExp("^&|[^\\\\]&"),cap(0));
   for (int i=1;i<10;i++)
   {
     QString pattern("^\\\\%1|[^\\\\]\\\\%1");
     pattern=pattern.arg(i);
     newItem.replace(QRegExp(pattern),cap(i));
   }
   newItem.replace("\\\\","\\");
   return newItem;
}
