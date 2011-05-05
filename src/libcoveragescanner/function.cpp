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

#include "function.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "coveragescannerpdef.h"
#include "source.h"


Function::Function()
{
  FUNCTION_TRACE;
  scoped_name=NULL;
  prototype_str=NULL;
  clear(); 
}

Function::Function(const Function &f)
{
  FUNCTION_TRACE;
  scoped_name=NULL;
  prototype_str=NULL;
  *this=f;
}

Function::~Function()
{
  FUNCTION_TRACE;
  FREE(scoped_name);
  FREE(prototype_str);
}

void Function::clear()
{
  FUNCTION_TRACE;
  source_p=NULL;
  FREE(scoped_name);
  FREE(prototype_str);
  to_exclude=false;
  scoped_name=NULL;
  prototype_str=NULL;
  end_code_column_pre=-1;
  end_code_column_orig=-1;
  end_code_line_pre=-1;
  end_code_line_orig=-1;
  end_code_line_abs=-1;
  end_prototype_column_pre=-1;
  end_prototype_column_orig=-1;
  end_prototype_line_pre=-1;
  end_prototype_line_orig=-1;
  end_prototype_line_abs=-1;
  start_code_column_pre=-1;
  start_code_column_orig=-1;
  start_code_line_pre=-1;
  start_code_line_orig=-1;
  start_code_line_abs=-1;
  start_prototype_column_pre=-1;
  start_prototype_column_orig=-1;
  start_prototype_line_pre=-1;
  start_prototype_line_orig=-1;
  start_prototype_line_abs=-1;
}

bool Function::operator==(const Function &s) const
{
  return ( end_code_column_pre         == s.end_code_column_pre )
    &&   ( end_code_line_abs           == s.end_code_line_abs )
    &&   ( end_prototype_column_pre    == s.end_prototype_column_pre )
    &&   ( end_prototype_line_abs      == s.end_prototype_line_abs )
    &&   ( start_code_column_pre       == s.start_code_column_pre )
    &&   ( start_code_line_abs         == s.start_code_line_abs )
    &&   ( start_prototype_column_pre  == s.start_prototype_column_pre )
    &&   ( start_prototype_line_abs    == s.start_prototype_line_abs ) ;
}

void Function::operator=(const Function &s)
{
  FUNCTION_TRACE;

  FREE(scoped_name);
  FREE(prototype_str);
  source_p=s.source_p;
  if (s.scoped_name)
    scoped_name=STRDUP(s.scoped_name);
  if (s.prototype_str)
    prototype_str=STRDUP(s.prototype_str);
  file_name=s.file_name;
  to_exclude=s.to_exclude;
  end_code_column_pre         = s.end_code_column_pre ;
  end_code_column_orig        = s.end_code_column_orig ;
  end_code_line_pre           = s.end_code_line_pre ;
  end_code_line_orig          = s.end_code_line_orig ;
  end_code_line_abs           = s.end_code_line_abs ;
  end_prototype_column_pre    = s.end_prototype_column_pre ;
  end_prototype_column_orig   = s.end_prototype_column_orig ;
  end_prototype_line_pre      = s.end_prototype_line_pre ;
  end_prototype_line_orig     = s.end_prototype_line_orig ;
  end_prototype_line_abs      = s.end_prototype_line_abs ;
  start_code_column_pre       = s.start_code_column_pre ;
  start_code_column_orig      = s.start_code_column_orig ;
  start_code_line_pre         = s.start_code_line_pre ;
  start_code_line_orig        = s.start_code_line_orig ;
  start_code_line_abs         = s.start_code_line_abs ;
  start_prototype_column_pre  = s.start_prototype_column_pre ;
  start_prototype_column_orig = s.start_prototype_column_orig ;
  start_prototype_line_pre    = s.start_prototype_line_pre ;
  start_prototype_line_orig   = s.start_prototype_line_orig ;
  start_prototype_line_abs    = s.start_prototype_line_abs ;
}

#if LOG
void Function::PDEBUG() const
{
  FUNCTION_TRACE;
  DEBUG2("Function: Name:\"%s\" ",scoped_name);

  DEBUG8(" Proto:<%s:(%i,%i)->(%i,%i) %i->%i>",
      file_name,
      start_prototype_line_pre,start_prototype_column_pre,
      end_prototype_line_pre,end_prototype_column_pre,
      start_prototype_line_orig,end_prototype_line_orig);
  if (prototype())
    DEBUG2("'%s'",prototype());

  DEBUG8(" Code:<%s:(%i,%i)->(%i,%i) %i->&i>",
      file_name,
      start_code_line_pre,start_code_column_pre,
      end_code_line_pre,end_code_column_pre,
      start_code_line_orig,end_code_line_orig);

  if (to_exclude)
    DEBUG1("[EXCLUDE]");
  DEBUG1("\n");
}
#endif

/*! Extraction of the prototype string 
 * \retval true Success
 * \retval false Failure
 */
bool Function::extractPrototype(char**prototype_p /*!< prototype (must be FREE by the called)*/) const
{
  FUNCTION_TRACE;
  source_p->_extract_substring(start_prototype_line_abs,start_prototype_column_pre,end_prototype_line_abs,end_prototype_column_pre,prototype_p);
  return prototype_p!=NULL;
}

const char*Function::prototype() const
{
  FUNCTION_TRACE;
   if (prototype_str)
      return prototype_str;
   extractPrototype(&prototype_str);
   return prototype_str;
}

void Function::getFunctionCode(long &s_line_abs,long &e_line_abs,long &s_line_orig,long &s_column_orig,long &e_line_orig,long &e_column_orig,long &s_line_pre,long &s_column_pre,long &e_line_pre,long &e_column_pre) const
{
  FUNCTION_TRACE;
  s_line_abs    = start_code_line_abs;
  e_line_abs    = end_code_line_abs;
  s_line_orig   = start_code_line_orig;
  s_column_orig = start_code_column_orig;
  e_line_orig   = end_code_line_orig;
  e_column_orig = end_code_column_orig;
  s_line_pre    = start_code_line_pre;
  s_column_pre  = start_code_column_pre;
  e_line_pre    = end_code_line_pre;
  e_column_pre  = end_code_column_pre;
}

void Function::getFunctionPrototype(long &s_line_abs,long &e_line_abs,long &s_line_orig,long &s_column_orig,long &e_line_orig,long &e_column_orig,long &s_line_pre,long &s_column_pre,long &e_line_pre,long &e_column_pre) const
{
  FUNCTION_TRACE;
  s_line_abs    =  start_prototype_line_abs    ;
  e_line_abs    =  end_prototype_line_abs      ;
  s_line_orig   =  start_prototype_line_orig   ;
  s_column_orig =  start_prototype_column_orig ;
  e_line_orig   =  end_prototype_line_orig     ;
  e_column_orig =  end_prototype_column_orig   ;
  s_line_pre    =  start_prototype_line_pre    ;
  s_column_pre  =  start_prototype_column_pre  ;
  e_line_pre    =  end_prototype_line_pre      ;
  e_column_pre  =  end_prototype_column_pre    ;
}

void Function::setFunctionCode(long s_line_abs,long e_line_abs,long s_line_orig,long s_column_orig,long e_line_orig,long e_column_orig,long s_line_pre,long s_column_pre,long e_line_pre,long e_column_pre)
{
  FUNCTION_TRACE;
  start_code_line_abs    = s_line_abs    ;
  end_code_line_abs      = e_line_abs    ;
  start_code_line_orig   = s_line_orig   ;
  start_code_column_orig = s_column_orig ;
  end_code_line_orig     = e_line_orig   ;
  end_code_column_orig   = e_column_orig ;
  start_code_line_pre    = s_line_pre    ;
  start_code_column_pre  = s_column_pre  ;
  end_code_line_pre      = e_line_pre    ;
  end_code_column_pre    = e_column_pre  ;
}

void Function::setFunctionPrototype(long s_line_abs,long e_line_abs,long s_line_orig,long s_column_orig,long e_line_orig,long e_column_orig,long s_line_pre,long s_column_pre,long e_line_pre,long e_column_pre)
{
  FUNCTION_TRACE;
  FREE(prototype_str);
  prototype_str=NULL;
  start_prototype_line_abs    =  s_line_abs    ;
  end_prototype_line_abs      =  e_line_abs    ;
  start_prototype_line_orig   =  s_line_orig   ;
  start_prototype_column_orig =  s_column_orig ;
  end_prototype_line_orig     =  e_line_orig   ;
  end_prototype_column_orig   =  e_column_orig ;
  start_prototype_line_pre    =  s_line_pre    ;
  start_prototype_column_pre  =  s_column_pre  ;
  end_prototype_line_pre      =  e_line_pre    ;
  end_prototype_column_pre    =  e_column_pre  ;
}

const char* Function::getScopedName() const
{
  FUNCTION_TRACE;
  return scoped_name;
}

void Function::setScopedName(const char *n)
{
  FUNCTION_TRACE;
  ASSERT(n);
  FREE(scoped_name);
  scoped_name=STRDUP(n);
}

void Function::pushScopedName(const char *n)
{
  FUNCTION_TRACE;
  ASSERT(n);
  if (scoped_name)
  {
    int lg=strlen(scoped_name);
    lg+=strlen(n);
    lg+=2+1;
    scoped_name=(char*)REALLOC(scoped_name,lg*sizeof(char));
    strcat(scoped_name,"::");
    strcat(scoped_name,n);
    ASSERT(lg==(int)(strlen(scoped_name)+1));
  }
  else
    setScopedName(n);
}

void Function::clearScopedName()
{
  FUNCTION_TRACE;
  FREE(scoped_name);
  scoped_name=NULL;
}

const Source* Function::getSource() const
{
  FUNCTION_TRACE;
  return source_p;
}

void Function::setSource(const Source *s)
{
  FUNCTION_TRACE;
  source_p=s;
}


bool Function::containsExpression(const Expression &e) const
{
  FUNCTION_TRACE;
  return e.start_ge(start_code_line_abs,start_code_column_pre)
    && e.end_le(end_code_line_abs,end_code_column_pre);
}
