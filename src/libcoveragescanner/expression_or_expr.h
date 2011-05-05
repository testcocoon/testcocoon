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

#ifndef EXPRESSION_OR_EXPR_H
#define EXPRESSION_OR_EXPR_H
#include "coveragescannerpdef.h"
#include "csmesfile.h"
#include "patch.h"
#include "expression_nop.h"
#include <string>
#include <list>

class StringPool;
class Source;
class ExpressionOrExpr: public ExpressionNOP
{
public:
    ExpressionOrExpr(const char *f,instrumentation_option_t options,long s_line_absolute, long s_line_org, long s_column_org, long s_line_pre, long s_column_pre, long e_line_absolute, long e_line_org, long e_column_org, long e_line_pre, long e_column_pre,long depth,Expression *expression_id1,Expression *expression_id2);
    virtual ~ExpressionOrExpr();

#if LOG
    virtual void PDEBUG() const ;
#endif
private:
    void operator=(const ExpressionOrExpr &);
    ExpressionOrExpr(const ExpressionOrExpr &);
};

#endif
