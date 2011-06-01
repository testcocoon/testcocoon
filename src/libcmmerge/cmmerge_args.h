/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
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

#ifndef CMMERGE_ARGS_H
#define CMMERGE_ARGS_H

#include "csmesfile.h"
#include <list>
#include <string>

class CMMerge
{
  private:
    CMMerge();
  public:
    static CMMerge &instance() ;
    bool analyse_args(int argc,const char*const * argv);
    bool getopt_process(int argc,const char* const* argv);
    void clear() ;

    CSMESFile::access_t access() const { return _access; }
    CSMESFile::merge_policy_t  policy() const { return _policy; }
    bool  verbose() const { return _verbose; }
    const std::string &reference() const { return _reference_csmes; }
    const std::string &output() const { return _output_filename; }
    const std::list<std::string> & inputs() const { return _input_filenames; }

  private:

    CSMESFile::access_t _access;
    CSMESFile::merge_policy_t _policy;
    bool _verbose;
    std::string _reference_csmes;
    std::string _output_filename;
    std::list<std::string> _input_filenames;

    static bool getoption_verbose(const char *);
    static bool getoption_instrumentation_and_execution(const char *filename);
    static bool getoption_input(const char *filename);
    static bool getoption_output(const char *file);
    static bool getoption_append(const char *);
};

#endif
