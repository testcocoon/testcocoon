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

#ifndef FILTER_SOURCE_H
#define FILTER_SOURCE_H
#include "coveragescannerpdef.h"
#include "regexpr.h"
#include "filter.h"
#include "../libcsmes/fileinfo.h"
#include <list>
#include <string>


class FilterSource : public Filter
{
  public:
    FilterSource() : Filter(false) {}
    enum type_t { 
      FILENAME=Filter::FILENAME,
      PATH=Filter::PATH,
      FILE_WILDCARD=Filter::FILE_WILDCARD,
      FILE_REGULAR_EXPRESSION=Filter::FILE_REGULAR_EXPRESSION } ;
    std::string append(bool ignore_flag,type_t type,const char *expression);
    std::string appendDefault(bool ignore_flag,type_t type,const char *expression);
  private:
    FilterSource(const FilterSource &src);
    FilterSource& operator=(const FilterSource &src);
} ;

#endif
