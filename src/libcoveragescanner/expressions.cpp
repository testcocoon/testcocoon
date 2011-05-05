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

#include "expressions.h"
#include "coveragescannerpdef.h"
#include <stdlib.h>
#include <algorithm>

#ifndef NO_DEBUG
#define PREALLOC_SIZE 1
#else
#define PREALLOC_SIZE 1024
#endif


Expressions::Expressions()
{
  FUNCTION_TRACE;
  values.clear();
}

Expressions::~Expressions()
{
  FUNCTION_TRACE;
  clear();
}

void Expressions::clear()
{
  FUNCTION_TRACE;
  ignore_areas.clear();
  for (expressions_t::iterator it=values.begin();it!=values.end();++it)
  {
    ASSERT ((*it)!=NULL);
    delete *it;
  }
  values.clear();
  clearPatches();
}

bool Expressions::append(Expression *e)
{
  FUNCTION_TRACE;
  if (e==NULL)
    return false;
  long start_line_abs,start_line_pre,start_column_pre,start_line_org,start_column_org;
  long end_line_abs,end_line_pre,end_column_pre,end_line_org,end_column_org;
  const char *f;
  instrumentation_option_t option;
  instrumentation_type_t t;

  if (!e->valid())
  {
    DEBUG1("Invalid measurement\n");
    return false;
  }
  if (!e->read(t,f,option,start_line_abs,start_line_org,start_column_org,start_line_pre,start_column_pre,end_line_abs,end_line_org,end_column_org,end_line_pre,end_column_pre))
  {
    DEBUG1("Could not read the current item\n");
    return false;
  }

  return append_internal(e);
}

/*! \brief Append the expression.
 *
 *  The two sorted indexes are created at the same time.
 */
bool Expressions::append_internal(Expression *e)
{
  FUNCTION_TRACE;
  /* append new item */
  if (values.capacity()<values.size()+1)
	values.reserve(values.capacity()+PREALLOC_SIZE);
  values.resize(values.size()+1);
  values[values.size()-1]=e;
#if LOG
  DEBUG1("New ");
  values[values.size()-1]->PDEBUG();
  DEBUG1("\n");
#endif

  return true;
}

long Expressions::indexes()const
{
  FUNCTION_TRACE;
  long nb_indexes = 0;
  for (expressions_t::const_iterator itValue=values.begin();itValue!=values.end();++itValue)
  {
    if (!(*itValue)->undefined())
      nb_indexes += (*itValue)->indexes();
  }
  return nb_indexes;
}

#if LOG
void Expressions::PDEBUG() const
{
  FUNCTION_TRACE;
  long i;

  DEBUG1("RAW Table:\n");
  for (i=0;i<count();i++)
  {
    if (!values[i]->undefined())
    {
      DEBUG1("  ");
      values[i]->PDEBUG();
      DEBUG1("\n");
    }
  }
}
#endif

bool Expressions::instrument_exists(const char *filename) const
{
  FUNCTION_TRACE;
  for (long i=0;i<count();i++)
  {
    if (values[i]->is_filename(filename) && values[i]->indexes()>0 )
      return true;
  }
  return false;
}

static inline bool expr_sort_cmp(const Expression* d1, const Expression* d2)
{
  if (d1->instrumentation_type() == d2->instrumentation_type())
  {
    if (start_eq(*d1,*d2))
      return end_gt(*d1 , *d2);
    else
      return !start_gt(*d1 , *d2);
  }
  else
    return d1->instrumentation_type() < d2->instrumentation_type();
}


void Expressions::sort_instrumentation_table()
{
   std::sort(values.begin(), values.end(), expr_sort_cmp);
}

long Expressions::count(const char *file) const
{
  FUNCTION_TRACE;
  int nb_total_instrumentation=count();
  int nb_instrumentation=0;
  for (int i=0;i<nb_total_instrumentation;i++)
  {
    const Expression *expr_p =values[i];
    if (expr_p && expr_p->is_filename(file))
      nb_instrumentation++;
  }
  return nb_instrumentation;
}

bool Expressions::instrument_exists_abs(long start_line,long start_column, long end_line, long end_column ) const
{
  FUNCTION_TRACE;
  int i;
  int max=count();
  for (i=0;i<max;i++)
  {
    if (!values[i]->undefined())
    {
      if ( ( ( values[i]->startLineAbsolute()==start_line && values[i]->startColumnPre()>=start_column )
            || values[i]->startLineAbsolute()>start_line )
          && ( ( values[i]->endLineAbsolute()==end_line && values[i]->endColumnPre()<=end_column )
            || values[i]->endLineAbsolute()<end_line ) )
        return true;
    }
  }
  return false;
}

void Expressions::ignore_area(long start_line,long start_column, long end_line, long end_column )
{
  ignore_areas.append(start_line,start_column, end_line, end_column );
}


