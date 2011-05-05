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

#ifndef EXPRESSION_INTERFACE_H
#define EXPRESSION_INTERFACE_H
#include "coveragescannerpdef.h"
#include <vector>
enum instrumentation_type_t
{
    INSTRUMENTATION_UNDEFINED,
    INSTRUMENTATION_ASSIGNMENT,
    INSTRUMENTATION_COMPUTATION,
    INSTRUMENTATION_SOME_BINARY_OPERATOR_EXPR,
    INSTRUMENTATION_AND_EXPR,
    INSTRUMENTATION_OR_EXPR,
    INSTRUMENTATION_NOT_EXPR,
    INSTRUMENTATION_PARENTHESIS_EXPR,
    INSTRUMENTATION_SAME_EXPRESSION_AS,
    INSTRUMENTATION_IF_THEN,
    INSTRUMENTATION_IF_THEN_ELSE,
    INSTRUMENTATION_CONDITION_FALSE_ONLY,
    INSTRUMENTATION_CONDITION_FULL,
    INSTRUMENTATION_WHILE,
    INSTRUMENTATION_FOR,
    INSTRUMENTATION_CASE,
    INSTRUMENTATION_LABEL,
    INSTRUMENTATION_MAIN_FUNCTION, /*!< not a read instrumentation: the index size if 0.
                                   Usage: mark the location of the main function */
    INSTRUMENTATION_SUB_CONDITION, /*!< \note must be the last (lowest priority) */
} ; /*!< type of instrumentations, the order is important for the sort function */

typedef struct
{
  char coverage_mdcs;
  char coverage_branch;
  char coverage_condition;
  char coverage_decision;
  char partial_condition;
  char coverage_count;
  char coverage_hit;
  char coverage_enable;
  char instrument_exception;
  char instrument_assignment;
} instrumentation_option_t;

class ExpressionInterface
{
  public:
    virtual instrumentation_type_t instrumentation_type() const =0;
    virtual bool valid() const=0;
    virtual void clear()=0;
#if LOG
    virtual void PDEBUG() const=0;
#endif
};

#endif

