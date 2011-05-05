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

#ifndef PATCH_CS_H
#define PATCH_CS_H
#include "coveragescannerpdef.h"
#include "patch.h"

class PatchCs : public Patch
{
  public:
    PatchCs(const char *_f,long _line_rel,long _line,long _col,long _depth,const instrumentation_option_t &_option,long _offset,const char *_table_name,PatchPriority p) ;

    virtual std::string code(const std::string &table_name) const;
  protected:
    virtual std::string _code(const std::string &table_name) const=0;

  private:
    void hide_code_cs(long line_rel,long column_abs,std::string &patch_code) const;
    void indent_code_cs(long column_abs,std::string &patch_code) const;
    void start_hide_code_cs(std::string &patch_code) const;
    void end_hide_code_cs(long line_rel,long column_abs,std::string &patch_code) const;


};
#endif
