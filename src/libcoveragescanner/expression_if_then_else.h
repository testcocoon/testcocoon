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

#ifndef EXPRESSION_IF_THEN_ELSE_H
#define EXPRESSION_IF_THEN_ELSE_H
#include "coveragescannerpdef.h"
#include "csmesfile.h"
#include "patch.h"
#include "expression.h"
#include <string>
#include <list>

class StringPool;
class Source;
class ExpressionIfThenElse: public Expression
{
public:
    ExpressionIfThenElse(const char *f,instrumentation_option_t options,long s_line_absolute, long s_line_org, long s_column_org, long s_line_pre, long s_column_pre, long e_line_absolute, long e_line_org, long e_column_org, long e_line_pre, long e_column_pre,long depth,
     long s_line_patch1, long s_column_patch1,  long e_line_patch1, long e_column_patch1,
     long s_line_patch2, long s_column_patch2,  long e_line_patch2, long e_column_patch2,
     Expression *expression_id1
     );
    virtual ~ExpressionIfThenElse();

    virtual std::list<const Patch *> generatePatchCode(code_style_t t,const char *table_name,long index);

    virtual instrumentation_type_t instrumentation_type() const { return type ; }
    virtual long startIndex() const { return _index; }

    virtual long indexes() const;

#if LOG
    virtual void PDEBUG() const ;
#endif
    virtual CSMESFile::instrumentation_t instrumentationType() const ;
private:
    void operator=(const ExpressionIfThenElse &);
    ExpressionIfThenElse(const ExpressionIfThenElse &);
};

#endif
