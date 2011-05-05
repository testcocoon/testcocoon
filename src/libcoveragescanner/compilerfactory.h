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

#ifndef COMPILER_FACTORY_
#define COMPILER_FACTORY_
#include "compilerinterface.h"
class Option;

class CompilerFactory
{
    CompilerFactory() {}
    static CompilerFactory& instance() ;
  public:
    static CompilerInterface* create(const Option &option);

    static std::string  application_path(const std::string &exe_param) ;
    static std::string profile_name(const std::string &option_tool_name,const std::string &appli_path) ;
    static bool find_tool_type (const char* argv0,bool option,const std::string &appli_path,std::string &option_tool_name, bool &profile_inactive) ;
    static std::string  findNativeTool(const std::string &profile_tool_name,const Argv & _args) ;
  private:
    static bool profile_existing(const std::string &prof, const std::string &path) ;
    static void extract_tool_name(const std::string &,std::string &,bool suppress_exe_extension,bool &inactive);
    static std::string findFileInPath(const std::string & ,int index=0);
    static bool name_check(const std::string &exe_str,const std::string &compiler_name,bool arg0) ;
    static void suppressPathEntry(const char *abs_filename);
};

#endif
