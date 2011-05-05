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


#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H
#include "expression.h"
#include "functions.h"
#include "ignoreareas.h"

class TestExpressions;
class Expressions
{
public:
    Expressions();
    virtual ~Expressions();

    bool append(Expression *);
    const Expression& at(long id)const { return *values[id]; }
    bool instrument_exists(const char* filename) const;
    bool instrument_exists_abs(long start_line,long start_column, long end_line, long end_column ) const;
    void clear();
    long indexes() const ;
    long count() const { return values.size(); }
    long count(const char *file) const ;
    void filterFunctions(const Functions &);
    void optimize(Expression::code_style_t);
    const std::list<const Patch *> &patches() const { return _patches; }
    void ignore_area(long start_line,long start_column, long end_line, long end_column );

#if LOG
    void PDEBUG() const;
#endif
private:
    friend class TestExpressions;
    IgnoreAreas ignore_areas;
    bool expressionAncestor(const Expression *item, const Expression *maybe_parent) const;
    bool expressionAncestor(const Expression *item, instrumentation_type_t maybe_parent_type) const;
    bool append_internal(Expression *);
    typedef std::vector<Expression*> expressions_t ;
    expressions_t values;
    bool generatePatchCode(Expression::code_style_t code_style);
    void clearPatches();
    bool suppress_non_instrumentations(Expression::code_style_t code_style);
    bool suppress_disabled_instrumentations();
    bool suppress_invalid_instrumentations();
    bool suppress_sub_conditions();
    bool suppress_sub_conditions_not_included_in_condition();
    bool suppress_redondant_instrumentation();
    bool suppress_instrumentations_to_skip();
    bool suppress_sub_conditions_with_sub_conditions();
    void generate_parents();


    void sort_instrumentation_table();

    std::list<const Patch *> _patches;
} ;
#endif

