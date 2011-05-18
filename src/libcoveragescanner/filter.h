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

#ifndef FILTER_H
#define FILTER_H
#include "coveragescannerpdef.h"
#include "regexpr.h"
#include "../libcsmes/fileinfo.h"
#include <list>
#include <string>


class Filter 
{
  protected:
    Filter(bool default_be) : default_behaviour(default_be) { }
    virtual ~Filter();
    enum type_t { FUNCTION_WILDCARD, FUNCTION_REGULAR_EXPRESSION , FILENAME, PATH, FILE_WILDCARD, FILE_REGULAR_EXPRESSION,  FILE_WILDCARD_ABS, FILE_REGULAR_EXPRESSION_ABS} ;
    std::string append(bool ignore_flag,type_t type,const char *expression);
    std::string appendDefault(bool ignore_flag,type_t type,const char *expression);
  public:
    void clear();
    bool isInclude(const char *file,std::string &expression_match) const ;
    bool isExclude(const char *file,std::string &expression_match) const {return !isInclude(file,expression_match); }
#if LOG
    void PDEBUG() const;
#endif
  protected:
    struct filter_t 
    {
      bool ignore;
      type_t type; 
      char * expression ;
      char * expression_abs ;
      regexp *regexpr ;
    };
    std::list<filter_t> filter;
    static std::string setInternal(filter_t & ,bool ignore_flag,type_t type,const char *expression)  ;
  private:
    Filter(const Filter &src);
    Filter& operator=(const Filter &src);
    bool default_behaviour;
} ;

#endif
