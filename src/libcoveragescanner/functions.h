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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "coveragescannerpdef.h"
#include "filterfunctions.h"
#include "function.h"
#include <list>

class CSMESFile;
class FileName;
class Expression;
class Expressions;
class TestCCsCppParserFunction ;

class Functions
{
  public:
    Functions(const FilterFunctions *p) {filter_functions_p=p;}
    void append(Function &f) ;
    void clear()                   { list.clear(); exclude_list.clear(); }
#if LOG
    void PDEBUG() const       ;
#endif
    void save(const char *module_name,const FileName *filenames_p,const Expressions *expressions_p,unsigned long signature,CSMESFile &f);
    bool isExcludeFunction(const Expression &) const;
  protected:
    int count() const { return list.size() ; }
  private:
    static bool containsExpression(const std::list<Function> &functions,const Expression &) ;
    std::list<Function> list,exclude_list;
    const FilterFunctions *filter_functions_p;
    
    friend class TestCCsCppParserFunction ;
};

#endif
