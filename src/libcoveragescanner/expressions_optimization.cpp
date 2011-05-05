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
#define SWAP(a,b) itemp=(a);(a)=(b);(b)=itemp;
#define M 7
#define NSTACK 50
#define NR_END 1

void Expressions::optimize(Expression::code_style_t code_style)
{
  FUNCTION_TRACE;

  DEBUG1("\nList of instrumentations:\n");
  PDEBUG();

  DEBUG1("Suppression of instrumentations to skip\n");
  if (suppress_instrumentations_to_skip())
  {
    suppress_invalid_instrumentations();
    DEBUG1("List of instrumentations:\n");
    PDEBUG();
  }
  DEBUG1("Suppression of disabled instrumentation\n");
  if (suppress_disabled_instrumentations())
  {
    suppress_invalid_instrumentations();
    DEBUG1("List of instrumentations:\n");
    PDEBUG();
  }

  sort_instrumentation_table();
  generate_parents();

  DEBUG1("Suppression of redondant instrumentations\n");
  if (suppress_redondant_instrumentation())
  {
    suppress_invalid_instrumentations();
    DEBUG1("List of instrumentations:\n");
    PDEBUG();
  }

  DEBUG1("Suppression of sub-conditions\n");
  if (suppress_sub_conditions())
  {
    suppress_invalid_instrumentations();
    DEBUG1("List of instrumentations:\n");
    PDEBUG();
  }

  DEBUG1("Suppression of non-instrumentation\n");
  if (suppress_non_instrumentations(code_style))
  {
    suppress_invalid_instrumentations();
    DEBUG1("List of instrumentations:\n");
    PDEBUG();
  }
  DEBUG1("Suppression of disabled instrumentation\n");
  if (suppress_disabled_instrumentations())
  {
    suppress_invalid_instrumentations();
    DEBUG1("List of instrumentations:\n");
    PDEBUG();
  }
  DEBUG1("\n");
  generatePatchCode(code_style);
}

static inline bool patch_ptr_less (const Patch *p1,const Patch *p2) 
{
  return (*p1) < (*p2);
}

bool Expressions::generatePatchCode(Expression::code_style_t code_style)
{
  long i;
  long nb_indexes =0;
  const int nb_values=values.size();
  clearPatches();

  for (i=0;i<nb_values;i++)
  {
    int idx=i;
    if (!values[idx]->undefined())
    {
      std::list<const Patch *> patches=values[idx]->generatePatchCode(code_style,"",nb_indexes);
      nb_indexes += values[idx]->indexes();
      _patches.insert(_patches.end(),patches.begin(),patches.end());
    }
  }
  _patches.sort(patch_ptr_less);

#if LOG
  DEBUG1("Patch code list:\n");
  for (std::list<const Patch *>::const_iterator it=_patches.begin();it!=_patches.end();++it)
  {
    DEBUG1("  ");
    (*it)->PDEBUG();
    DEBUG1("\n");
  }
#endif
  return true;
}

void Expressions::clearPatches()
{
  for (std::list<const Patch *>::iterator it=_patches.begin();it!=_patches.end();++it)
    delete *it;
  _patches.clear();
}

bool Expressions::suppress_redondant_instrumentation()
{ // values is already sorted.
  FUNCTION_TRACE;
  bool modified = false;
  const int nb_values=values.size();
  for (int i=0;i<nb_values-1;i++)
  {
    if (*values[i] == *values[i+1])
    {
#if LOG
      DEBUG1("Suppression of ");
      values[i]->PDEBUG();
      DEBUG1("\n");
#endif

      values[i]->clear();
      modified=true;
    }
  }
  return modified ;
}

bool Expressions::suppress_invalid_instrumentations()
{
  FUNCTION_TRACE;
  bool modified = false;
  int nb_values=values.size();
  for (int i=0;i<nb_values;i++)
  {
    if (! values[i]->valid())
    {
      int pos=nb_values-1;
      while (pos>0 && ( ! values[pos]->valid() ) )
      {
        delete values[pos];
        values[pos]=NULL;
        pos--;
      }
      if (pos>i)
      {
        delete values[i];
        values[i]=values[pos];
        nb_values=pos;
      }
      else
        nb_values=i;
      modified = true;
    }
  }
  if (modified)
    values.resize(nb_values);
  return modified ;
}

bool Expressions::suppress_sub_conditions()
{
  FUNCTION_TRACE;
  bool modified = false;
  bool ret=false;
  
  ret = suppress_sub_conditions_not_included_in_condition();
  modified = modified || ret ;
  
  ret = suppress_sub_conditions_with_sub_conditions();
  modified = modified || ret ;
  return modified;
}


bool Expressions::suppress_disabled_instrumentations()
{
  FUNCTION_TRACE;
  bool modified = false;
  for (expressions_t::iterator itValue=values.begin();itValue!=values.end();++itValue)
  {
    if (!(*itValue)->getOption().coverage_enable)
    {
#if LOG
      DEBUG1("Remove");
      (*itValue)->PDEBUG();
      DEBUG1("\n");
#endif
      (*itValue)->clear();
      modified = true;
    }
  }
  return modified ;
}

bool Expressions::suppress_non_instrumentations(Expression::code_style_t code_style)
{
  FUNCTION_TRACE;
  bool modified = false;
  for (expressions_t::iterator itValue=values.begin();itValue!=values.end();++itValue)
  {

    std::list<const Patch *> patches=(*itValue)->generatePatchCode(code_style,"",0);
    if (patches.empty())
    {
#if LOG
      DEBUG1("Remove");
      (*itValue)->PDEBUG();
      DEBUG1("\n");
#endif
      (*itValue)->clear();
      modified = true;
    }
    for (std::list<const Patch *>::iterator  it=patches.begin();it!=patches.end();++it)
      delete *it;
  }
  return modified ;
}

bool Expressions::suppress_sub_conditions_with_sub_conditions()
{
  FUNCTION_TRACE;
  bool modified = false;
  std::list<ExpressionInterface*> to_remove;
  for (expressions_t::iterator itExpression=values.begin();itExpression!=values.end();++itExpression)
  {
    if (!(*itExpression)->valid())
      continue;
    if (!(*itExpression)->children().empty())
      continue;
    bool remove_top_item=false;
    for (Expression *parent_p=(*itExpression);parent_p;parent_p=parent_p->parent())
    {
      if (   parent_p->instrumentation_type()==INSTRUMENTATION_CONDITION_FULL
          || parent_p->instrumentation_type()==INSTRUMENTATION_CONDITION_FALSE_ONLY
          || parent_p->instrumentation_type()==INSTRUMENTATION_IF_THEN
          || parent_p->instrumentation_type()==INSTRUMENTATION_SUB_CONDITION
          || parent_p->instrumentation_type()==INSTRUMENTATION_WHILE
          || parent_p->instrumentation_type()==INSTRUMENTATION_FOR
          || parent_p->instrumentation_type()==INSTRUMENTATION_CASE
          || parent_p->instrumentation_type()==INSTRUMENTATION_LABEL
          || parent_p->instrumentation_type()==INSTRUMENTATION_IF_THEN_ELSE
         )
      {
        if (remove_top_item)
          to_remove.push_back(parent_p);
      }
      if (parent_p->instrumentation_type()==INSTRUMENTATION_SUB_CONDITION)
        remove_top_item=true;
      if (parent_p->instrumentation_type()==INSTRUMENTATION_SOME_BINARY_OPERATOR_EXPR)
        remove_top_item=false;
    }
  }

  for (std::list<ExpressionInterface*>::iterator it=to_remove.begin();it!=to_remove.end();++it)
  {
#if LOG
    DEBUG1("Remove");
    (*it)->PDEBUG();
    DEBUG1("\n");
#endif
    modified = true;
    (*it)->clear();
  }
  return modified ;
}


bool Expressions::suppress_sub_conditions_not_included_in_condition()
{
  FUNCTION_TRACE;
  bool modified = false;
  for (expressions_t::iterator itExpression=values.begin();itExpression!=values.end();++itExpression)
  {
    bool remove = true;
    if ((*itExpression)->instrumentation_type()!=INSTRUMENTATION_SUB_CONDITION)
      continue;
    if (!(*itExpression)->valid())
      continue;

    for (const Expression *parent_p=(*itExpression)->parent();parent_p;parent_p=parent_p->parent())
    {
      if (!parent_p->valid())
        continue;
      if (  parent_p->instrumentation_type()==INSTRUMENTATION_CONDITION_FULL
          || parent_p->instrumentation_type()==INSTRUMENTATION_CONDITION_FALSE_ONLY
          || parent_p->instrumentation_type()==INSTRUMENTATION_IF_THEN
          || parent_p->instrumentation_type()==INSTRUMENTATION_WHILE
          || parent_p->instrumentation_type()==INSTRUMENTATION_FOR
          || parent_p->instrumentation_type()==INSTRUMENTATION_CASE
          || parent_p->instrumentation_type()==INSTRUMENTATION_LABEL
          || parent_p->instrumentation_type()==INSTRUMENTATION_ASSIGNMENT
          || parent_p->instrumentation_type()==INSTRUMENTATION_IF_THEN_ELSE )
      {
        remove = false;
        break;
      }
    }
    if (remove)
    {
      modified = true;
#if LOG
      DEBUG1("Remove");
      (*itExpression)->PDEBUG();
      DEBUG1("\n");
#endif
      (*itExpression)->clear();
    }
  }
  return modified ;
}

bool Expressions::expressionAncestor(const Expression *item, instrumentation_type_t maybe_parent_type) const
{
  for (const Expression *cur=item->parent();cur;cur=cur->parent())
  {
    if (cur->instrumentation_type()==maybe_parent_type)
      return true;
  }
  return false;
}

bool Expressions::expressionAncestor(const Expression *item, const Expression *maybe_parent) const
{
  for (const Expression *cur=item->parent();cur;cur=cur->parent())
  {
    if (cur==maybe_parent)
      return true;
  }
  return false;
}

void Expressions::filterFunctions(const Functions &f)
{
  FUNCTION_TRACE;
  bool modified = false;
  DEBUG1("Suppression of instrumentations of functions to ignore\n");
  for (expressions_t::iterator expression=values.begin();expression!=values.end();++expression)
  {
    if (
        (*expression)->valid()
        &&
        f.isExcludeFunction(*(*expression))
       )
    {
#if LOG
      DEBUG1("Remove");
      (*expression)->PDEBUG();
      DEBUG1("\n");
#endif
      modified=true;
      (*expression)->clear();
    }
  }

  if (modified)
  {
    DEBUG1("Suppression of invalid instrumentations\n");
    suppress_invalid_instrumentations();
  }
  DEBUG1("List of instrumentations:\n");
  PDEBUG();
}

void Expressions::generate_parents()
{
  FUNCTION_TRACE;
  const int nb_values=values.size();
  for (int iParent=0;iParent<nb_values;iParent++)
  {
    Expression *parent = values[iParent] ;
    const Expression::_childrens_t &childrens=parent->children();

    for (Expression::_childrens_t::const_iterator itChidId=childrens.begin() ; itChidId!=childrens.end() ; ++itChidId)
    {
      for (int iChild=0;iChild<nb_values;iChild++)
      {
        Expression *child = values[iChild] ;
        if (child==*itChidId)
        {
          child->setParent(parent);
          break;
        }
      }
    }
  }
}

bool Expressions::suppress_instrumentations_to_skip()
{
  FUNCTION_TRACE;
  bool modified = false;
  const int nb_values=values.size();
  for (int i=0;i<nb_values;i++)
  {
    Expression *cur_p = values[i] ;
    if (ignore_areas.isInIgnoreArea(*cur_p))
    {
      modified = true;
#if LOG
      DEBUG1("Remove");
      cur_p->PDEBUG();
      DEBUG1("\n");
#endif
      cur_p->clear();
    }
  }
  return modified ;
}

