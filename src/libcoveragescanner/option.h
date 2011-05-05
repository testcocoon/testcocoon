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

#ifndef OPTION_H
#define OPTION_H
#include "coveragescannerpdef.h"
#include "filtersource.h"
#include "compilerinterface.h"
#include "filterfunctions.h"
#include "expression.h"
#include "argv.h"

class Option
{
  public:
    const std::string & applicationPath() const { return appli_path; }
    Option(int argc,const char *const *argv);
    virtual ~Option();
    const std::string & toolName() const { return option_tool_name; } 
    void operator=(const Option &src);
    const FilterSource &getFilterFile() const { return filterFile; }
    const FilterFunctions &getFilterFunction() const { return filterFunction; }
    static const Argv& param_args();
    bool isInactive() const { return profile_inactive; }
    bool isVerbose() const { return _verbose; }
    bool isActive() const { return !profile_inactive; }
    void setActive() {profile_inactive=false;}
    void setInactive() {profile_inactive=true;}
    void setCoverageHit() {coverage_hit=true;}
    void setCoverageCount() {coverage_hit=false;}
    bool lockCsexe() const { return lock_csexe; }
    bool preprocessInBackground() const { return preprocess_in_background; }
    void setInstrumentException(bool b) { instrument_exception=b; }
    void setInstrumentAssignment(bool b) { instrument_assignment=b; }
    bool isPartialCondition() const { return partial_condition; }
    bool isCoverageHit() const { return coverage_hit; }
    bool isCoverageCount() const { return !coverage_hit; }
    bool isCoverageBranch() const { return coverage_branch; }
    bool isCoverageMDCS() const { return coverage_mdcs; }
    bool isCoverageDecision() const { return coverage_decision; }
    bool isCoverageCondition() const  { return coverage_condition; }
    bool instrumentExceptions() const { return instrument_exception; }
    bool instrumentAssignments() const { return instrument_assignment; }
    std::string profileToolName() const { return profile_tool_name; }
    instrumentation_option_t getInstrumentationOption() const ;
    const char *defaultCsexeOutput() const { return default_csexe; }
    const char *getLibgenOption() const { return libgen_option; }
    const char *getArchitectureOption() const { return architecture_option; }
    const char *stdinScript() const { return stdin_script; }
    const Argv& param_args_mri() const { return args_mri; }
  private:
    bool parseArgOptionWithParam(const char *arg,const char *pattern,std::string &param);
    Argv args_mri;
    void setCoverageBranch()
    {
      coverage_condition=false;
      coverage_decision=false;
      coverage_branch=true;
      coverage_mdcs=false;
    }
    void setCoverageCondition()
    {
      coverage_condition=true;
      coverage_decision=true;
      coverage_branch=true;
      coverage_mdcs=false;
    }
    void setPartialCondition(bool b) { partial_condition=b; }
    void setCoverageMDCS()
    {
      coverage_condition=true;
      coverage_decision=true;
      coverage_branch=true;
      coverage_mdcs=true;
    }
    void setCoverageDecision()
    {
      coverage_condition=false;
      coverage_decision=true;
      coverage_branch=true;
      coverage_mdcs=false;
    }
    void envVarToArgList(const char *envvar,Argv &args) const;
    bool coverage_hit;
    bool coverage_condition;
    bool instrument_exception;
    bool instrument_assignment;
    bool coverage_decision;
    bool coverage_branch;
    bool coverage_mdcs;
    bool partial_condition;
    FilterSource filterFile;
    FilterFunctions filterFunction;
    std::string appli_path;
    std::string option_tool_name;
    std::string profile_tool_name;
    void suppress_arg(Argv &args,int &index);
    bool profile_inactive;
    bool processArgs (const Argv & args,Argv &processed_args,const CompilerInterface *compiler_p);
    char *default_csexe;
    bool lock_csexe;
    char *libgen_option;
    char *architecture_option;
    bool preprocess_in_background;
    char *stdin_script;
    bool force_active_innactive;
    bool _verbose;
};
void print_usage();
#endif
