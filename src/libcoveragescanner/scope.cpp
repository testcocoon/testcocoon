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

#include "scope.h"

void Scope::clear() 
{
  for (std::vector<char*>::iterator it=_scope_names.begin();it!=_scope_names.end();++it)
    FREE(*it);
  _scope_names.clear();
}

void Scope::pushScope(const Scope &sc)
{
  for (std::vector<char*>::const_iterator it=sc._scope_names.begin();it!=sc._scope_names.end();++it)
    addName( (*it) );
}

std::string Scope::fullName() const
{
  std::string n;
  for (std::vector<char*>::const_iterator it=_scope_names.begin();it!=_scope_names.end();++it)
  {
    if (it!=_scope_names.begin())
      n+="::";
    n+=std::string(*it);
  }
  return n;
}
