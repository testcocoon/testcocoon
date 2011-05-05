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

#ifndef SCOPE_H
#define SCOPE_H
#include <vector>
#include <string>
#include "debug.h"
#include <stdlib.h>
#include <string.h>

class Scope
{
  public:
    virtual ~Scope() { clear(); }
    void clear() ;
    void addName(const char *n) 
    {
      if (n)
        _scope_names.push_back(STRDUP(n));
      else
        _scope_names.push_back(NULL);
    }
    void pushScope(const Scope &sc);
    void popScope() { _scope_names.pop_back(); }
    int count() const { return _scope_names.size(); }
    const char * operator[](int i) const { return _scope_names[i]; }
    std::string fullName() const;

  private:
      std::vector<char*> _scope_names;
} ;

#endif
