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

#include "coveragescannerpdef.h"
#include "expression.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stringpool.h"
#include "system.h"
#include <sstream>
#include "expressions.h"
#include "source.h"

StringPool Expression::filenames_pool;


Expression::Expression()
{
  FUNCTION_TRACE;
  _parent=0;
  clear();
}

Expression::~Expression()
{
  FUNCTION_TRACE;
}

void Expression::clear()
{
  FUNCTION_TRACE;
  _patch_location.resize(2);
  _patch_location[0].start_depth         = -1;
  _patch_location[1].start_depth         = -1;
  _patch_location[0].end_depth           = -1;
  _patch_location[1].end_depth           = -1;
  _patch_location[0].start_line_absolute = -1;
  _patch_location[0].end_line_absolute   = -1;
  _patch_location[0].start_column        = -1;
  _patch_location[0].end_column          = -1;
  _patch_location[1].start_line_absolute = -1;
  _patch_location[1].end_line_absolute   = -1;
  _patch_location[1].start_column        = -1;
  _patch_location[1].end_column          = -1;
  _location_pre.start_line_absolute        = -1;
  _location_pre.end_line_absolute          = -1;
  _location_pre.start_line             = -1;
  _location_pre.end_line               = -1;
  _location_pre.start_column           = -1;
  _location_pre.end_column             = -1;
  _location_org.start_line             = -1;
  _location_org.end_line               = -1;
  _location_org.start_column           = -1;
  _location_org.end_column             = -1;
  _patch_location[1].start_column        = -1;
  _patch_location[1].end_column          = -1;
  type                       = INSTRUMENTATION_UNDEFINED;
  _code_style                = CODESTYLE_UNKNOWN;
  _index                     = -1;
  _file                      = NULL;
  clearTree();
}


bool Expression::valid() const
{
  FUNCTION_TRACE;
  if (_location_pre.start_line_absolute==-1)
    return false;
  if (_location_pre.end_line_absolute==-1)
    return false;
  if (_location_pre.start_line==-1)
    return false;
  if (_location_pre.end_line==-1)
    return false;
  if (_location_org.start_line==-1)
    return false;
  if (_location_org.end_line==-1)
    return false;
  if (_location_pre.start_column==-1)
    return false;
  if (_location_pre.end_column==-1)
    return false;
  if (type==INSTRUMENTATION_UNDEFINED)
    return false;
  if (! (_location_org.start_column==-1L && _location_org.end_column==-1L))
  {
	if ((_location_org.start_column==_location_org.end_column) && (_location_org.start_line==_location_org.end_line))
	  return false;
  }
  if ((_location_org.start_column>_location_org.end_column) && (_location_org.start_line==_location_org.end_line))
    return false;
  if (_location_org.start_line>_location_org.end_line)
    return false;
  if (! (_location_pre.start_column==-1L && _location_pre.end_column==-1L))
  {
	if ((_location_pre.start_column==_location_pre.end_column) && (_location_pre.start_line==_location_pre.end_line))
	  return false;
  }
  if ((_location_pre.start_column>_location_pre.end_column) && (_location_pre.start_line==_location_pre.end_line))
    return false;
  if (_location_pre.start_line>_location_pre.end_line)
    return false;
  return true;
}

bool Expression::write(instrumentation_type_t t, const char *f,instrumentation_option_t options,long s_line_absolute, long s_line_org, long s_column_org, long s_line_pre, long s_column_pre, long e_line_absolute, long e_line_org, long e_column_org, long e_line_pre, long e_column_pre,long depth)
{
  FUNCTION_TRACE;
  type                   = t;
  _location_pre.start_line_absolute    = s_line_absolute;
  _location_org.start_line         = s_line_org;
  _location_org.end_column         = e_column_org;
  _location_pre.start_line         = s_line_pre;
  _location_pre.end_column         = e_column_pre;
  _location_pre.end_line_absolute      = e_line_absolute;
  _location_org.end_line           = e_line_org;
  _location_org.start_column       = s_column_org;
  _location_pre.end_line           = e_line_pre;
  _location_pre.start_column       = s_column_pre;
  instrumentation_option = options;

  _file = filenames_pool.add(f);

  return true;
}

void Expression::writePatchArea(int idx,long s_line_absolute, long s_column, long e_line_absolute, long e_column,long depth)
{
  FUNCTION_TRACE;
  if (((int)_patch_location.size())<idx+1)
    _patch_location.resize(idx+1);
  _patch_location[idx].start_line_absolute = s_line_absolute;
  _patch_location[idx].end_line_absolute   = e_line_absolute;
  _patch_location[idx].start_column        = s_column;
  _patch_location[idx].end_column          = e_column;
  _patch_location[idx].end_depth           = -depth;
  _patch_location[idx].start_depth         = depth;
}

bool Expression::read(instrumentation_type_t &t, const char *&f,instrumentation_option_t &options,long &s_line_absolute, long &s_line_org, long &s_column_org, long &s_line_pre, long &s_column_pre, long &e_line_absolute,  long &e_line_org, long &e_column_org,  long &e_line_pre, long &e_column_pre) const 
{
  FUNCTION_TRACE;
  t               = type;
  s_line_absolute = _location_pre.start_line_absolute;
  e_line_absolute = _location_pre.end_line_absolute;
  s_line_org      = _location_org.start_line;
  e_line_org      = _location_org.end_line;
  s_column_org    = _location_org.start_column;
  e_column_org    = _location_org.end_column;
  s_line_pre      = _location_pre.start_line;
  e_line_pre      = _location_pre.end_line;
  s_column_pre    = _location_pre.start_column;
  e_column_pre    = _location_pre.end_column;
  f               = file();
  options         = getOption();
  return true;
}

void Expression::setParent(Expression *p)
{
  _parent=p;
}


void Expression::clearTree()
{
#if 0
  if (_parent)
  {
    for (_childrens_t::iterator itChild=_parent->_childrens.begin();itChild!=_parent->_childrens.end();++itChild)
    {
      if ((*itChild)==_parent)
      {
        _parent->_childrens.erase(itChild);
        break;
      }
    }
  }
  for (_childrens_t::iterator itChild=_childrens.begin();itChild!=_childrens.end();++itChild)
  {
    (*itChild)->_parent=_parent;
    if (_parent)
      _parent->_childrens.push_back(*itChild);
  }
#endif
  _parent=NULL;
  _childrens.clear();
}



void Expression::addChildren(Expression *c)
{
  _childrens.push_back(c);
}


#if LOG
void Expression::PDEBUG() const
{
  FUNCTION_TRACE;
  DEBUG8(" pre:%s:(%li,%li)->(%li,%li) abs:%li->%li",file(),_location_pre.start_line,_location_pre.start_column,_location_pre.end_line,_location_pre.end_column, _location_pre.start_line_absolute,_location_pre.end_line_absolute);
  DEBUG5(" org:(%li,%li)->(%li,%li)",_location_org.start_line,_location_org.start_column,_location_org.end_line,_location_org.end_column);
  if (_patch_location[0].start_line_absolute>=0)
  {
    DEBUG5(" patch1:(%li,%li)->(%li,%li)",
        _patch_location[0].start_line_absolute,_patch_location[0].start_column,_patch_location[0].end_line_absolute,_patch_location[0].end_column);
  }
  if (_patch_location[1].start_line_absolute>=0)
  {
    DEBUG5(" patch2:(%li,%li)->(%li,%li)",
        _patch_location[1].start_line_absolute,_patch_location[1].start_column,_patch_location[1].end_line_absolute,_patch_location[1].end_column);
  }
}
#endif



bool Expression::is_filename(const char *f) const
{
  FUNCTION_TRACE;
  if (file()==NULL)
    return false;
  return strcmp(f,file())==0;
}

CSMESFile::instrumentation_childrens_t Expression::childrensCSMESFile() const 
{
  FUNCTION_TRACE;
  CSMESFile::instrumentation_childrens_t childrens;
  childrens.type=CSMESFile::CHILDRENS_NONE;
  for (_childrens_t::const_iterator itExpr=children().begin();itExpr!=children().end();++itExpr)
  {
    CSMESFile::instrumentation_children_t child;
    child.identifier=reinterpret_cast<_I64>(*itExpr);
    child.type= CSMESFile::CHILDREN_REL;
    childrens.childrens.push_back(child);
  }
  return childrens;
}
