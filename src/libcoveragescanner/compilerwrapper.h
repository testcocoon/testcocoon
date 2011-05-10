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

#ifndef COMPILER_WRAPPER_H
#define COMPILER_WRAPPER_H
#include "compilerinterface.h"
#include "compilerwrappertextoption.h"
#include "compilerwrapperlanguageoption.h"
#include "compilerwrapperlistoption.h"
#include "compilerwrapperenumoption.h"
#include "compilerwrapperintoption.h"
#include "compilerwrapperoptionpool.h"
#include "compilerwrapperregexpoption.h"
#include "argv.h"
#include "tmpfile.h"
#include "defines.h"
class Option;
class TestWrapper;
class TestCoverageScannerProfile;
class TestResponseFile;

class CompilerWrapper:public CompilerInterface
{
  friend class TestWrapper;
  friend class TestResponseFile;
  friend class TestCoverageScannerProfile;
  CompilerWrapperOptionPool pool;
    const Option &option;
  public:
    CompilerWrapper(const Option &);
    virtual ~CompilerWrapper();

    type_t type() const;
    bool preprocessor(const char *filename,char **command) const;
    bool usePreprocessor() const ;
    bool compile_file(char **command_p,const char *source,const char *destination) const;
    bool  destination(char **dest,bool &in_cmd_line) const;
    Expression::code_style_t codeStyle() const;
    bool libraries(FileName *filenames) const;
    virtual void setArgs(const Argv &);
    virtual void setArgsStdoutMriScript(const Argv &);
    virtual bool useCompilerToGenerateObjects() const;
    void clear();
  public:
    bool objects(FileName *filenames) const;
    const char* responseFile(int index,int &arg_index_min,int &arg_index_max) const;
    const char *const * dyn_lib_ext() const;
    const char *const * static_lib_ext() const;
    const char *const * obj_ext() const;
    const char *const * pdb_ext() const;
    const char *const * sbr_ext() const;
    const char *const * cpp_ext() const;
    const char *const * c_ext() const;
    const char *const * cs_ext() const;
    const char *const * ignore_ext() const;
    const char *const * exit_funct_ext() const;
    const char *function_attribute() const;
    const char *dll_export() const;
    const char *dll_import() const;
    bool customSetup() const;
    bool setupMS() const;
    bool setupPOSIX() const;
    bool setupCpp() const;
    bool setupMSCE() const;
    bool setupGNU() const;
    bool unixLibraryName() const;
    bool useResponseFile() const ;
    void display_temporary_files() const;
    virtual bool deactivateCoverageScanner(const Argv& args) const;
    virtual bool generateCoverageScannerLibrary() const ;
    bool forceDllExport() const;
    std::string csmes_library_object_name() const { return TmpFile::object().generate(coveragescanner_library_object->value(),true); }
    const char *errorFormat() const { return error_format->value(); }
    const char* toolName() const { return tool_str->value(); }
    const char* additionalArgumentSeparator() const { return additional_argument_separator->value(); } 
    bool compiler(char **command,bool hide_arguments) const;
    bool stdinMriScript(const Argv &args) const;
    bool defineMatchCoverageOn(const char *) const ;
    bool defineMatchCoverageCount(const char *) const ;
    bool defineMatchCoverageHit(const char *) const ;
    bool defineMatchCoverageBranch(const char *) const ;
    bool defineMatchCoverageDecition(const char *) const ;
    bool defineMatchCoverageMDCS(const char *) const ;
    bool defineMatchCoverageCondition(const char *) const ;
    bool defineMatchCoverageNoLockCsexe(const char *) const ;
    bool defineMatchCoverageLockCsexe(const char *) const ;
    bool defineMatchCoveragePartialInstrumentation(const char *) const ;
    bool defineMatchCoverageFullInstrumentation(const char *) const ;
    bool defineMatchCoverageNoExceptions(const char *) const ;
    bool defineMatchCoverageNoAssignment(const char *) const ;
    Defines defineFromCommandLine() const;
    bool append_cslibrary(char **new_command_p,const char *compiler_command,const char *libname) const;
    bool enableCoveragescannerPerDefault() const ;
    int callNativeTool() ;


  protected:
    virtual bool isDestination(const char*argument) const;
  private:
    void generateFullArgs();
    Argv args_full_args;
    const Argv & param_args_full_args() const { return args_full_args; }
    const Argv &  param_args_additiona_separator() const { return args_additional_separator; } 
    char * libraryPath(const char *filename,const FileName &library_path) const;
    void stripArgs();
    char * extractBasename() const;
    std::string _profile_name;
    char *profile_path;




    static std::string  replace_var(const std::string &str,const char*var,const char*val,bool &found) ;
    void replace_std_var(CompilerWrapperTextOption *);
    std::string replace_basename_src(const std::string &var) const ;
    bool checkProfile() const ;
    void replace_std_vars();
    bool readProfile();
    void initDefault();
    void languageAnalysis();


    void BuildLibraryPath(FileName &libraryPath) const;
    char * libgen_str;
    Argv args_additional_separator ;
    Argv args_stdin_mri_script;

    CompilerWrapperLanguageOption *language_supported;
    CompilerWrapperTextOption *additional_argument_separator;
    CompilerWrapperEnumOption *ar_command_format_option; /*!< AR format: arg0: command, arg1: operation arg2: destination */
    CompilerWrapperEnumOption *enable_coveragescanner_per_default;
    CompilerWrapperTextOption *compiler_cmd_str;
    CompilerWrapperEnumOption *compiler_hide_option_1_arg_append;
    CompilerWrapperTextOption *compiler_output_default_str;
    CompilerWrapperEnumOption *compiler_output_option_append;
    CompilerWrapperTextOption *coveragescanner_library_object;
    CompilerWrapperEnumOption *custom_setup;
    CompilerWrapperEnumOption *deactivate_coveragescanner;
    CompilerWrapperEnumOption *deactivate_coveragescanner_option_1_arg_append;
    CompilerWrapperTextOption *dll_export_str;
    CompilerWrapperTextOption *dll_import_str;
    CompilerWrapperEnumOption *dll_output_static_lib_append;
    CompilerWrapperEnumOption *static_lib_as_default;
    CompilerWrapperTextOption *error_format;
    CompilerWrapperEnumOption *force_dll_export;
    CompilerWrapperTextOption *function_attribute_str;
    CompilerWrapperEnumOption *generate_coveragescanner_library; /*!< Used for skipping the generation of the coveragescanner library (necessary for static lib) */
    CompilerWrapperEnumOption *hide_option_1_arg_append;
    CompilerWrapperEnumOption *library_path_option_append ;
    CompilerWrapperEnumOption *link_library_option_append;
    CompilerWrapperTextOption *link_output_default_str;
    CompilerWrapperEnumOption *link_output_option_append;
    CompilerWrapperEnumOption *linker_hide_option_1_arg_append;
    CompilerWrapperEnumOption *pdb_output_option_append;
    CompilerWrapperRegExpOption *coveragescanner_coverage_on;
    CompilerWrapperRegExpOption *coveragescanner_coverage_hit;
    CompilerWrapperRegExpOption *coveragescanner_coverage_count;
    CompilerWrapperRegExpOption *coveragescanner_coverage_nolock_csexe ;
    CompilerWrapperRegExpOption *coveragescanner_coverage_lock_csexe;
    CompilerWrapperRegExpOption *coveragescanner_coverage_branch ;
    CompilerWrapperRegExpOption *coveragescanner_coverage_decision;
    CompilerWrapperRegExpOption *coveragescanner_coverage_mdcs;
    CompilerWrapperRegExpOption *coveragescanner_coverage_condition;
    CompilerWrapperRegExpOption *coveragescanner_coverage_no_exceptions ;
    CompilerWrapperRegExpOption *coveragescanner_coverage_partial_instrumentation;
    CompilerWrapperRegExpOption *coveragescanner_coverage_full_instrumentation;
    CompilerWrapperRegExpOption *coveragescanner_coverage_no_assignment;
    CompilerWrapperTextOption *preprocessor_cmd_str;
    CompilerWrapperEnumOption *preprocessor_define_option_append;
    CompilerWrapperEnumOption *preprocessor_hide_option_1_arg_append;
    CompilerWrapperEnumOption *preprocessor_keep_option_1_arg_append;
    CompilerWrapperEnumOption *use_response_file;
    CompilerWrapperIntOption *minimum_command_line_size_for_response_file;
    CompilerWrapperEnumOption *response_file_append; //!< command is appended to the filename
    CompilerWrapperTextOption *response_file_str;   //!< option file command
    CompilerWrapperEnumOption *sbr_output_option_append;
    CompilerWrapperListOption *table_c_ext ;
    CompilerWrapperListOption *table_ignore_ext ;
    CompilerWrapperListOption *table_compile_option; //!< list of compile option (-c, /C, ...)
    CompilerWrapperListOption *table_compiler_hide_option_1_arg; /*!< List of options  with 1 argument to hide to the compiler */
    CompilerWrapperListOption *table_compiler_hide_option_no_arg; /*!< List of options to hide to the compiler */
    CompilerWrapperListOption *table_compiler_output_option;
    CompilerWrapperListOption *table_cpp_ext ;
    CompilerWrapperListOption *table_cs_ext ;
    CompilerWrapperListOption *table_deactivate_coveragescanner_option_1_arg; /*!< List of options  with 1 argument to disable coveragescanner */
    CompilerWrapperListOption *table_deactivate_coveragescanner_option_no_arg; //!< deactivates coverage scanner
    CompilerWrapperListOption *table_default_lib_path; /*!< default library path */
    CompilerWrapperListOption *table_dll_option; /*!< option which indicates to the linker to generate a DLL */
    CompilerWrapperListOption *table_dll_output_static_lib; /*!< option which permits to specify the implicit lib generated by a DLL */
    CompilerWrapperListOption *table_dyn_lib_ext ;
    CompilerWrapperListOption *table_exit_func_ext ;
    CompilerWrapperListOption *table_force_dll_export_option; /*!< Force dll option when this options occurs */
    CompilerWrapperListOption *table_hide_option_1_arg; /*!< List of options  with 1 argument to hide to the compiler/linker */
    CompilerWrapperListOption *table_hide_option_no_arg; /*!< List of options to hide to the compiler/linker */
    CompilerWrapperListOption *table_library_path_option; /*!< add a directory to the path (ex:-L) */
    CompilerWrapperListOption *table_link_library_option; /*!< option which permits to define the libraries which are linked(eg: -l) */
    CompilerWrapperListOption *table_link_output_option;
    CompilerWrapperListOption *table_linker_hide_option_1_arg; /*!< List of options  with 1 argument to hide to the linker */
    CompilerWrapperListOption *table_linker_hide_option_no_arg; /*!< List of options to hide to the linker */
    CompilerWrapperListOption *table_obj_ext ;
    CompilerWrapperListOption *table_pdb_ext ;
    CompilerWrapperListOption *table_pdb_output_option;
    CompilerWrapperTextOption *preprocessor_define_separator_option; //!< Separator for defines options
    CompilerWrapperListOption *table_preprocessor_define_option; //!< list of preprocessor option (-D, /D, ...)
    CompilerWrapperListOption *table_preprocessor_hide_option_1_arg; /*!< List of options  with 1 argument to hide to the preprocessor */
    CompilerWrapperListOption *table_preprocessor_keep_option_1_arg; 
    CompilerWrapperListOption *table_preprocessor_hide_option_no_arg; /*!< List of options to hide to the preprocessor */
    CompilerWrapperListOption *table_response_file_ext ; //!< list of option file extension ex:.rst
    CompilerWrapperListOption *table_sbr_ext ;
    CompilerWrapperListOption *table_sbr_output_option;
    CompilerWrapperListOption *table_skip_generate_coveragescanner_library_option; /*!< skip the generation of coveragescanner library when this option is occuring (static lib) */
    CompilerWrapperListOption *table_static_lib_ext ;
    CompilerWrapperListOption *table_stdin_mri_script_option;
    CompilerWrapperTextOption *tool_str;
    CompilerWrapperEnumOption *unix_library_name;
    CompilerWrapperListOption *table_c_language_option; //!< list of options wich permits to switch in C mode
    CompilerWrapperListOption *table_cpp_language_option; //!< list of options wich permits to switch in C mode

    CompilerWrapperListOption *table_language_selection_option; //!< list of options wich permits to select the language
    CompilerWrapperEnumOption *language_selection_option_append; //!< append mode form table_language_option
    CompilerWrapperListOption *table_language_selection_option_cpp_keywords; //!< list of C++ keyword which activetae the C++ mode
    CompilerWrapperListOption *table_language_selection_option_c_keywords; //!< list of C keyword which activetae the C mode

    bool checkOptionDefined(const CompilerWrapperOption * option) const;
    const std::string &responseFileName() const ;
    mutable std::string _response_file_name;
    std::string generateCommand(const std::string &executable,const std::list<std::string> &params)  const ;
    std::string generateCommandWithoutResponseFile(const std::string &executable,const std::list<std::string> &params)  const ;
    bool  compiler_option_sbr_pdb(std::list<std::string> &commands, int &i, const CompilerWrapperListOption &option_table,const CompilerWrapperEnumOption &option_append,bool append_name,const char *ext) const;
    static void suppressExtension(char *s,char *&e);
    static void suppressExtension(std::string &s,std::string &e);
    void setUseResponseFile(bool b) { use_response_file->setValue(b?CompilerWrapperEnumOption::OPT_YES:CompilerWrapperEnumOption::OPT_NO); }
    void setMinimumCommandLineSizeForResponseFile(int v) { minimum_command_line_size_for_response_file->setValue(v); }
  public:
	std::string preprocessor_temp_filename(const std::string &) const;
    bool preprocessor_use_temp_filename() const;
    const char* CoverageScannerDefineOptionInArguments() const;
    void RemoveCoverageScannerDefine(Argv &args) const;
};

#endif
