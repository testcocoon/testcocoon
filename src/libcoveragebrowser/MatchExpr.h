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

#ifndef MATCHEXPR_H
#define MATCHEXPR_H

#include <QRegExp>

class MatchExpr :public QRegExp
{
   public: 
      MatchExpr() ;
      MatchExpr ( const QString & pattern, Qt::CaseSensitivity cs = Qt::CaseSensitive) ;
      MatchExpr ( const MatchExpr & rx )  ;

      void setPattern(const QString &pat);
      QString replace(const QString &pattern) ;

      static QString convertToRegExp(const QString & ) ;
      static QString convertToWilcard(const QString & ) ;
} ;

#endif
