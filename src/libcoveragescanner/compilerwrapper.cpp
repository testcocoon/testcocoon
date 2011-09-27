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

#include "coveragescannerpdef.h"
#include "compilerwrapper.h"
#include "option.h"
#include "compilerfactory.h"
#include "argv.h"
#include "debug.h"
#include "compiler.h"
#include "utf.h"
#include "source.h"
#include "filename.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <list>
#include <vector>
#include "regexp_custom.h"
#include <sys/stat.h>
#include "system.h"
#include "tmpfile.h"
#if defined(OS_LINUX) || defined(OS_MACX)
#include <dirent.h>
#include <unistd.h>
#elif defined(OS_WIN32)
#include "ms_dirent.h"
#include <windows.h>
#endif
#define PDB_APPEND_NAME 0
#define SBR_APPEND_NAME 1
#define COVERAGESCANNER_DEFINE "__COVERAGESCANNER__"
#define COVERAGESCANNER_DEFINE_LG (sizeof(COVERAGESCANNER_DEFINE)-1)

CompilerWrapper::CompilerWrapper(const Option &t) : CompilerInterface(t), option( t) 
{
  FUNCTION_TRACE;
  language_supported                                 = new CompilerWrapperLanguageOption ( pool,"LANGUAGE");
  additional_argument_separator                      = new CompilerWrapperTextOption ( pool,"ADDITIONAL_ARGUMENT_SEPARATOR");
  ar_command_format_option                           = new CompilerWrapperEnumOption ( pool,"AR_COMMAND_FORMAT_OPTION");
  compiler_cmd_str                                   = new CompilerWrapperTextOption ( pool,"COMPILER_CMD");
  compiler_hide_option_1_arg_append                  = new CompilerWrapperEnumOption ( pool,"COMPILER_HIDE_OPTION_ONE_ARG_APPEND");
  compiler_output_default_str                        = new CompilerWrapperTextOption ( pool,"COMPILER_OUTPUT_DEFAULT");
  compiler_output_option_append                      = new CompilerWrapperEnumOption ( pool,"COMPILER_OUTPUT_OPTION_APPEND");
  coveragescanner_library_object                     = new CompilerWrapperTextOption ( pool,"COVERAGESCANNER_LIBRARY_OBJECT");
  custom_setup                                       = new CompilerWrapperEnumOption ( pool,"CUSTOM_SETUP");
  deactivate_coveragescanner                         = new CompilerWrapperEnumOption ( pool,"DEACTIVATE_COVERAGESCANNER");
  deactivate_coveragescanner_option_1_arg_append     = new CompilerWrapperEnumOption ( pool,"DEACTIVATE_COVERAGESCANNER_OPTION_ONE_ARG_APPEND");
  dll_export_str                                     = new CompilerWrapperTextOption ( pool,"DLL_EXPORT");
  dll_import_str                                     = new CompilerWrapperTextOption ( pool,"DLL_IMPORT");
  dll_output_static_lib_append                       = new CompilerWrapperEnumOption ( pool,"DLL_OUTPUT_STATIC_LIB_APPEND");
  error_format                                       = new CompilerWrapperTextOption ( pool,"ERROR_FORMAT");
  force_dll_export                                   = new CompilerWrapperEnumOption ( pool,"FORCE_DLL_EXPORT");
  function_attribute_str                             = new CompilerWrapperTextOption ( pool,"FUNCTION_ATTRIBUTE");
  generate_coveragescanner_library                   = new CompilerWrapperEnumOption ( pool,"GENERATE_COVERAGESCANNER_LIBRARY");
  hide_option_1_arg_append                           = new CompilerWrapperEnumOption ( pool,"HIDE_OPTION_ONE_ARG_APPEND");
  library_path_option_append                         = new CompilerWrapperEnumOption ( pool,"LIBRARY_PATH_OPTION_APPEND");
  link_library_option_append                         = new CompilerWrapperEnumOption ( pool,"LINK_LIBRARY_OPTION_APPEND");
  link_output_default_str                            = new CompilerWrapperTextOption ( pool,"LINK_OUTPUT_DEFAULT");
  link_additional_args                               = new CompilerWrapperTextOption ( pool,"LINK_ADDITIONAL_ARGUMENTS");
  link_output_option_append                          = new CompilerWrapperEnumOption ( pool,"LINK_OUTPUT_OPTION_APPEND");
  linker_hide_option_1_arg_append                    = new CompilerWrapperEnumOption ( pool,"LINKER_HIDE_OPTION_ONE_ARG_APPEND");
  pdb_output_option_append                           = new CompilerWrapperEnumOption ( pool,"PDB_OUTPUT_OPTION_APPEND");
  enable_coveragescanner_per_default                 = new CompilerWrapperEnumOption ( pool,"ENABLE_COVERAGESCANNER_PER_DEFAULT");
  coveragescanner_coverage_on                        = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_ON");
  coveragescanner_coverage_hit                       = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_HIT");
  coveragescanner_coverage_count                     = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_COUNT");
  coveragescanner_coverage_nolock_csexe              = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_NOLOCK_CSEXE");
  coveragescanner_coverage_lock_csexe                = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_LOCK_CSEXE");
  coveragescanner_coverage_branch                    = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_BRANCH");
  coveragescanner_coverage_decision                  = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_DECISION");
  coveragescanner_coverage_mdcs                      = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_MDCS");
  coveragescanner_coverage_condition                 = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_CONDITION");
  coveragescanner_coverage_no_exceptions             = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_NO_EXCEPTIONS");
  coveragescanner_coverage_partial_instrumentation   = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_PARTIAL_INSTRUMENTATION");
  coveragescanner_coverage_full_instrumentation      = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_FULL_INSTRUMENTATION");
  coveragescanner_coverage_no_assignment             = new CompilerWrapperRegExpOption ( pool,"COVERAGESCANNER_COVERAGE_NO_ASSIGNMENTS");
  preprocessor_cmd_str                               = new CompilerWrapperTextOption ( pool,"PREPROCESSOR_CMD");
  preprocessor_define_option_append                  = new CompilerWrapperEnumOption ( pool,"PREPROCESSOR_DEFINE_OPTION_APPEND");
  preprocessor_hide_option_1_arg_append              = new CompilerWrapperEnumOption ( pool,"PREPROCESSOR_HIDE_OPTION_ONE_ARG_APPEND");
  preprocessor_keep_option_1_arg_append              = new CompilerWrapperEnumOption ( pool,"PREPROCESSOR_KEEP_OPTION_ONE_ARG_APPEND");
  response_file_append                               = new CompilerWrapperEnumOption ( pool,"RESPONSE_FILE_OPTION_APPEND");
  use_response_file                                  = new CompilerWrapperEnumOption ( pool,"USE_RESPONSE_FILE");
  minimum_command_line_size_for_response_file        = new CompilerWrapperIntOption ( pool,"MINIMUM_COMMAND_LINE_SIZE_FOR_RESPONSE_FILE");
  response_file_str                                  = new CompilerWrapperTextOption ( pool, "RESPONSE_FILE_OPTION");
  sbr_output_option_append                           = new CompilerWrapperEnumOption ( pool,"SBR_OUTPUT_OPTION_APPEND");
  table_c_ext                                        = new CompilerWrapperListOption ( pool,"C_EXT" );
  table_ignore_ext                                   = new CompilerWrapperListOption ( pool,"IGNORE_EXT" );
  table_compile_option                               = new CompilerWrapperListOption ( pool,"COMPILER_OPTION" );
  table_compiler_hide_option_1_arg                   = new CompilerWrapperListOption ( pool, "COMPILER_HIDE_OPTION_ONE_ARG");
  table_compiler_hide_option_no_arg                  = new CompilerWrapperListOption ( pool, "COMPILER_HIDE_OPTION_NO_ARG");
  table_compiler_output_option                       = new CompilerWrapperListOption ( pool, "COMPILER_OUTPUT_OPTION");
  table_cpp_ext                                      = new CompilerWrapperListOption ( pool,"CPP_EXT" );
  table_cs_ext                                       = new CompilerWrapperListOption ( pool, "C#_EXT");
  table_deactivate_coveragescanner_option_1_arg      = new CompilerWrapperListOption ( pool, "DEACTIVATE_COVERAGESCANNER_OPTION_ONE_ARG");
  table_deactivate_coveragescanner_option_no_arg     = new CompilerWrapperListOption ( pool, "DEACTIVATE_COVERAGESCANNER_OPTION_NO_ARG");
  table_default_lib_path                             = new CompilerWrapperListOption ( pool, "DEFAULT_LIB_PATH");
  table_dll_option                                   = new CompilerWrapperListOption ( pool, "DLL_OPTION");
  table_dll_output_static_lib                        = new CompilerWrapperListOption ( pool, "DLL_OUTPUT_STATIC_LIB");
  table_dyn_lib_ext                                  = new CompilerWrapperListOption ( pool,"DYN_LIB_EXT" );
  table_exit_func_ext                                = new CompilerWrapperListOption ( pool, "EXIT_FUNCTIONS");
  table_force_dll_export_option                      = new CompilerWrapperListOption ( pool, "FORCE_DLL_EXPORT_OPTION");
  table_hide_option_1_arg                            = new CompilerWrapperListOption ( pool, "HIDE_OPTION_ONE_ARG");
  table_hide_option_no_arg                           = new CompilerWrapperListOption ( pool, "HIDE_OPTION_NO_ARG");
  table_library_path_option                          = new CompilerWrapperListOption ( pool, "LIBRARY_PATH_OPTION");
  table_link_library_option                          = new CompilerWrapperListOption ( pool, "LINK_LIBRARY_OPTION");
  table_link_output_option                           = new CompilerWrapperListOption ( pool, "LINK_OUTPUT_OPTION");
  table_linker_hide_option_1_arg                     = new CompilerWrapperListOption ( pool,"LINKER_HIDE_OPTION_ONE_ARG" );
  table_linker_hide_option_no_arg                    = new CompilerWrapperListOption ( pool, "LINKER_HIDE_OPTION_NO_ARG");
  table_obj_ext                                      = new CompilerWrapperListOption ( pool,"OBJ_EXT" );
  table_pdb_ext                                      = new CompilerWrapperListOption ( pool, "PDB_EXT");
  table_pdb_output_option                            = new CompilerWrapperListOption ( pool, "PDB_OUTPUT_OPTION");
  preprocessor_define_separator_option               = new CompilerWrapperTextOption ( pool,"PREPROCESSOR_DEFINE_SEPARATOR_OPTION" );
  table_preprocessor_define_option                   = new CompilerWrapperListOption ( pool,"PREPROCESSOR_DEFINE_OPTION" );
  table_preprocessor_hide_option_1_arg               = new CompilerWrapperListOption ( pool,"PREPROCESSOR_HIDE_OPTION_ONE_ARG" );
  table_preprocessor_keep_option_1_arg               = new CompilerWrapperListOption ( pool,"PREPROCESSOR_KEEP_OPTION_ONE_ARG" );
  table_preprocessor_hide_option_no_arg              = new CompilerWrapperListOption ( pool,"PREPROCESSOR_HIDE_OPTION_NO_ARG" );
  table_response_file_ext                            = new CompilerWrapperListOption ( pool,"RESPONSE_FILE_EXT" );
  table_sbr_ext                                      = new CompilerWrapperListOption ( pool,"SBR_EXT" );
  table_sbr_output_option                            = new CompilerWrapperListOption ( pool, "SBR_OUTPUT_OPTION");
  table_skip_generate_coveragescanner_library_option = new CompilerWrapperListOption ( pool, "SKIP_GENERATE_COVERAGESCANNER_LIBRARY_OPTION");
  table_static_lib_ext                               = new CompilerWrapperListOption ( pool,"STATIC_LIB_EXT" );
  table_stdin_mri_script_option                      = new CompilerWrapperListOption ( pool,"STDIN_MRI_SCRIPT_OPTION" );
  tool_str                                           = new CompilerWrapperTextOption ( pool,"TOOL");
  unix_library_name                                  = new CompilerWrapperEnumOption ( pool,"UNIX_LIBRARY_NAME");
  table_c_language_option                            = new CompilerWrapperListOption ( pool,"C_LANGUAGE_OPTION" );
  table_cpp_language_option                          = new CompilerWrapperListOption ( pool,"CPP_LANGUAGE_OPTION" );
  table_language_selection_option                    = new CompilerWrapperListOption ( pool,"LANGUAGE_SELECTION_OPTION");
  language_selection_option_append                   = new CompilerWrapperEnumOption ( pool,"LANGUAGE_SELECTION_OPTION_APPEND");
  table_language_selection_option_cpp_keywords       = new CompilerWrapperListOption ( pool,"LANGUAGE_SELECTION_OPTION_CPP_KEYWORD");
  table_language_selection_option_c_keywords         = new CompilerWrapperListOption ( pool,"LANGUAGE_SELECTION_OPTION_C_KEYWORD");
  static_lib_as_default                              = new CompilerWrapperEnumOption ( pool,"STATIC_LIB_AS_DEFAULT");

  args_full_args.clear();
  args_additional_separator.clear();
  args_stdin_mri_script.clear();
  setArgsStdoutMriScript(option.param_args_mri());

  _profile_name=CompilerFactory::profile_name(option.toolName(),option.applicationPath());
  if (option.getLibgenOption())
    libgen_str=STRDUP(option.getLibgenOption());
  else
    libgen_str=STRDUP("");

  initDefault();
  if (!readProfile())
  {
    FATAL2("Reading profile '%s' failed\n",_profile_name.c_str());
  }
  if (!checkProfile())
  {
    FATAL2("Incorrect profile ('%s')\n",_profile_name.c_str());
  }
  replace_std_vars();
  stripArgs();
}

void CompilerWrapper::clear()
{
  return pool.clear();
}

void CompilerWrapper::setArgsStdoutMriScript(const Argv &args)
{
  FUNCTION_TRACE;
  args_stdin_mri_script=args;
  generateFullArgs();
}

void CompilerWrapper::setArgs(const Argv &args)
{
  FUNCTION_TRACE;
  CompilerInterface::setArgs(args);
  args_additional_separator.clear();
  int i;

  int additional_argument_separator_lg=0;
  if (additional_argument_separator->defined())
    additional_argument_separator_lg=strlen(additional_argument_separator->value());

  for (i=0;args[i];i++)
  {
    const char *pos1;
    const char *pos2=args[i];
    do
    {
      pos1=pos2;
      if (additional_argument_separator->defined())
        pos2=strstr(pos1,additional_argument_separator->value());
      else
        pos2=NULL;
      char *str=STRDUP(pos1);
      if (pos2)
      {
        str[pos2-pos1]='\0';
        pos2+=additional_argument_separator_lg;
      }
      args_additional_separator.append(str);
      FREE(str);
    }
    while (pos2);
  }
  generateFullArgs();
  languageAnalysis();
}

CompilerWrapper::~CompilerWrapper()
{
  FUNCTION_TRACE;
  FREE(libgen_str);
  FREE(profile_path);
  delete     language_supported                                 ;
  delete     additional_argument_separator                      ;
  delete     ar_command_format_option                           ;
  delete     compiler_cmd_str                                   ;
  delete     compiler_hide_option_1_arg_append                  ;
  delete     compiler_output_default_str                        ;
  delete     compiler_output_option_append                      ;
  delete     coveragescanner_library_object                     ;
  delete     custom_setup                                       ;
  delete     deactivate_coveragescanner                         ;
  delete     deactivate_coveragescanner_option_1_arg_append     ;
  delete     dll_export_str                                     ;
  delete     dll_import_str                                     ;
  delete     dll_output_static_lib_append                       ;
  delete     error_format                                       ;
  delete     force_dll_export                                   ;
  delete     function_attribute_str                             ;
  delete     generate_coveragescanner_library                   ;
  delete     hide_option_1_arg_append                           ;
  delete     library_path_option_append                         ;
  delete     link_library_option_append                         ;
  delete     link_output_default_str                            ;
  delete     link_additional_args                               ;
  delete     link_output_option_append                          ;
  delete     linker_hide_option_1_arg_append                    ;
  delete     pdb_output_option_append                           ;
  delete     enable_coveragescanner_per_default                 ;
  delete     coveragescanner_coverage_on                        ;
  delete     coveragescanner_coverage_hit                       ;
  delete     coveragescanner_coverage_count                     ;
  delete     coveragescanner_coverage_nolock_csexe              ;
  delete     coveragescanner_coverage_lock_csexe                ;
  delete     coveragescanner_coverage_branch                    ;
  delete     coveragescanner_coverage_decision                  ;
  delete     coveragescanner_coverage_mdcs                      ;
  delete     coveragescanner_coverage_condition                 ;
  delete     coveragescanner_coverage_no_exceptions             ;
  delete     coveragescanner_coverage_partial_instrumentation   ;
  delete     coveragescanner_coverage_full_instrumentation      ;
  delete     coveragescanner_coverage_no_assignment             ;
  delete     preprocessor_cmd_str                               ;
  delete     preprocessor_define_option_append                  ;
  delete     preprocessor_hide_option_1_arg_append              ;
  delete     preprocessor_keep_option_1_arg_append              ;
  delete     response_file_append                               ;
  delete     use_response_file                                  ;
  delete     minimum_command_line_size_for_response_file        ;
  delete     response_file_str                                  ;
  delete     sbr_output_option_append                           ;
  delete     table_c_ext                                        ;
  delete     table_ignore_ext                                   ;
  delete     table_compile_option                               ;
  delete     table_compiler_hide_option_1_arg                   ;
  delete     table_compiler_hide_option_no_arg                  ;
  delete     table_compiler_output_option                       ;
  delete     table_cpp_ext                                      ;
  delete     table_cs_ext                                       ;
  delete     table_deactivate_coveragescanner_option_1_arg      ;
  delete     table_deactivate_coveragescanner_option_no_arg     ;
  delete     table_default_lib_path                             ;
  delete     table_dll_option                                   ;
  delete     table_dll_output_static_lib                        ;
  delete     table_dyn_lib_ext                                  ;
  delete     table_exit_func_ext                                ;
  delete     table_force_dll_export_option                      ;
  delete     table_hide_option_1_arg                            ;
  delete     table_hide_option_no_arg                           ;
  delete     table_library_path_option                          ;
  delete     table_link_library_option                          ;
  delete     table_link_output_option                           ;
  delete     table_linker_hide_option_1_arg                     ;
  delete     table_linker_hide_option_no_arg                    ;
  delete     table_obj_ext                                      ;
  delete     table_pdb_ext                                      ;
  delete     table_pdb_output_option                            ;
  delete     preprocessor_define_separator_option               ;
  delete     table_preprocessor_define_option                   ;
  delete     table_preprocessor_hide_option_1_arg               ;
  delete     table_preprocessor_keep_option_1_arg               ;
  delete     table_preprocessor_hide_option_no_arg              ;
  delete     table_response_file_ext                            ;
  delete     table_sbr_ext                                      ;
  delete     table_sbr_output_option                            ;
  delete     table_skip_generate_coveragescanner_library_option ;
  delete     table_static_lib_ext                               ;
  delete     table_stdin_mri_script_option                      ;
  delete     tool_str                                           ;
  delete     unix_library_name                                  ;
  delete     static_lib_as_default                              ;
  delete     table_c_language_option                            ;
  delete     table_cpp_language_option                          ;
  delete     table_language_selection_option                    ;
  delete     language_selection_option_append                   ;
  delete     table_language_selection_option_cpp_keywords       ;
  delete     table_language_selection_option_c_keywords         ;
}

void CompilerWrapper::generateFullArgs()
{
  FUNCTION_TRACE;
  args_full_args = args_additional_separator;
  args_full_args.append(args_stdin_mri_script);
}

void CompilerWrapper::initDefault()
{
  FUNCTION_TRACE;
  profile_path=NULL;
  if (!_profile_name.empty())
  {
    char filename_abs[MAX_PATH];
    realPath(_profile_name.c_str(),filename_abs);
    profile_path=STRDUP(filename_abs);
    int lg=strlen(profile_path);
    for (int i=lg-1;i>=0;i--)
    {
      if (profile_path[i]==FILE_SEPARATOR_CHAR || profile_path[i]==FILE_SEPARATOR_CHAR_BIS)
        break;
      profile_path[i]='\0';
    }
  }
}

bool CompilerWrapper::checkOptionDefined(const CompilerWrapperOption * option) const
{
  FUNCTION_TRACE;
  if (!option->defined())
  {
    static  const char *err_msg="'%s' not defined into the profile\n";
    ERROR2(err_msg,option->name());
    return false;
  }
  return true;
}

void CompilerWrapper::replace_std_vars()
{
  FUNCTION_TRACE;
  replace_std_var(tool_str);
  replace_std_var(preprocessor_cmd_str);
  replace_std_var(compiler_cmd_str);
  replace_std_var(compiler_output_default_str);
  replace_std_var(link_output_default_str);
  replace_std_var(dll_import_str);
  replace_std_var(dll_export_str);
  replace_std_var(function_attribute_str);
}

bool CompilerWrapper::checkProfile() const
{
  FUNCTION_TRACE;
  bool valid;
  valid = checkOptionDefined(tool_str)                                                   ;
  valid = checkOptionDefined(table_stdin_mri_script_option)                      && valid;
  valid = checkOptionDefined(table_preprocessor_define_option)                   && valid;
  valid = checkOptionDefined(table_deactivate_coveragescanner_option_no_arg)     && valid;
  valid = checkOptionDefined(table_compile_option)                               && valid;
  valid = checkOptionDefined(table_response_file_ext)                            && valid;
  valid = checkOptionDefined(response_file_str)                                  && valid;
  valid = checkOptionDefined(table_obj_ext)                                      && valid;
  valid = checkOptionDefined(table_pdb_ext)                                      && valid;
  valid = checkOptionDefined(table_sbr_ext)                                      && valid;
  valid = checkOptionDefined(table_cpp_ext)                                      && valid;
  valid = checkOptionDefined(table_exit_func_ext)                                && valid;
  valid = checkOptionDefined(table_cs_ext)                                       && valid;
  valid = checkOptionDefined(table_c_ext)                                        && valid;
  valid = checkOptionDefined(table_ignore_ext)                                   && valid;
  valid = checkOptionDefined(table_static_lib_ext)                               && valid;
  valid = checkOptionDefined(table_dyn_lib_ext)                                  && valid;
  valid = checkOptionDefined(additional_argument_separator)                      && valid;
  valid = checkOptionDefined(function_attribute_str)                             && valid;
  valid = checkOptionDefined(dll_export_str)                                     && valid;
  valid = checkOptionDefined(dll_import_str)                                     && valid;
  valid = checkOptionDefined(table_default_lib_path)                             && valid;
  valid = checkOptionDefined(table_dll_option)                                   && valid;
  valid = checkOptionDefined(table_skip_generate_coveragescanner_library_option) && valid;
  valid = checkOptionDefined(table_force_dll_export_option)                      && valid;
  valid = checkOptionDefined(table_c_language_option)                            && valid;
  valid = checkOptionDefined(coveragescanner_coverage_on)                        && valid;
  valid = checkOptionDefined(coveragescanner_coverage_hit)                       && valid;
  valid = checkOptionDefined(coveragescanner_coverage_count)                     && valid;
  valid = checkOptionDefined(coveragescanner_coverage_nolock_csexe)              && valid;
  valid = checkOptionDefined(coveragescanner_coverage_lock_csexe)                && valid;
  valid = checkOptionDefined(coveragescanner_coverage_branch)                    && valid;
  valid = checkOptionDefined(coveragescanner_coverage_decision)                  && valid;
  valid = checkOptionDefined(coveragescanner_coverage_mdcs)                      && valid;
  valid = checkOptionDefined(coveragescanner_coverage_condition)                 && valid;
  valid = checkOptionDefined(coveragescanner_coverage_no_exceptions)             && valid;
  valid = checkOptionDefined(coveragescanner_coverage_partial_instrumentation)   && valid;
  valid = checkOptionDefined(coveragescanner_coverage_full_instrumentation)      && valid;
  valid = checkOptionDefined(coveragescanner_coverage_no_assignment)             && valid;
  valid = checkOptionDefined(table_cpp_language_option)                          && valid;
  valid = checkOptionDefined(table_language_selection_option)                    && valid;
  valid = checkOptionDefined(language_selection_option_append)                   && valid;
  valid = checkOptionDefined(table_language_selection_option_cpp_keywords)       && valid;
  valid = checkOptionDefined(table_language_selection_option_c_keywords)         && valid;
  valid = checkOptionDefined(table_library_path_option)                          && valid;
  valid = checkOptionDefined(table_dll_output_static_lib)                        && valid;
  valid = checkOptionDefined(link_output_default_str)                            && valid;
  valid = checkOptionDefined(link_additional_args)                               && valid;
  valid = checkOptionDefined(compiler_output_default_str)                        && valid;
  valid = checkOptionDefined(table_link_library_option)                          && valid;
  valid = checkOptionDefined(table_compiler_hide_option_no_arg)                  && valid;
  valid = checkOptionDefined(table_compiler_hide_option_1_arg)                   && valid;
  valid = checkOptionDefined(table_linker_hide_option_no_arg)                    && valid;
  valid = checkOptionDefined(table_linker_hide_option_1_arg)                     && valid;
  valid = checkOptionDefined(table_deactivate_coveragescanner_option_1_arg)      && valid;
  valid = checkOptionDefined(table_preprocessor_hide_option_no_arg)              && valid;
  valid = checkOptionDefined(table_preprocessor_hide_option_1_arg)               && valid;
  valid = checkOptionDefined(table_preprocessor_keep_option_1_arg)               && valid;
  valid = checkOptionDefined(table_hide_option_no_arg)                           && valid;
  valid = checkOptionDefined(table_hide_option_1_arg)                            && valid;
  valid = checkOptionDefined(table_pdb_output_option)                            && valid;
  valid = checkOptionDefined(table_sbr_output_option)                            && valid;
  valid = checkOptionDefined(table_compiler_output_option)                       && valid;
  valid = checkOptionDefined(table_link_output_option)                           && valid;
  valid = checkOptionDefined(error_format)                                       && valid;
  valid = checkOptionDefined(language_supported)                                 && valid;
  valid = checkOptionDefined(use_response_file)                                  && valid;
  valid = checkOptionDefined(static_lib_as_default)                              && valid;
  return valid;
}

bool CompilerWrapper::readProfile()
{
  FUNCTION_TRACE;
  bool res = pool.parseprofile(_profile_name,option.getArchitectureOption(),param_args());
  if (res)
  {
    if (!coveragescanner_library_object->defined())
    {
#if defined(OS_WIN32)
      std::string lib=TmpFile::object().generate("__cs_library_$ID$.obj",true);
#elif defined(OS_LINUX) ||  defined(OS_MACX)
      std::string lib=TmpFile::object().generate("__cs_library_$ID$.o",true);
#endif
      coveragescanner_library_object->setValue(lib);
    }
    return true;
  }
  return false;
}


void CompilerWrapper::languageAnalysis()
{
  FUNCTION_TRACE;
  int i;
  for (i=1;i<param_args().argc();i++)
  {
    const char *param=param_args()[i];
    const char * c_language_option= table_c_language_option->findOptionTable(param,false)  ;
    if (c_language_option)
      setLanguage(LANGUAGE_C);
    const char * cpp_language_option= table_cpp_language_option->findOptionTable(param,false)  ;
    if (cpp_language_option)
      setLanguage(LANGUAGE_CPP);
    const char *language_selection_option= table_language_selection_option->findOptionTable(param,language_selection_option_append->value()!=CompilerWrapperEnumOption::OPT_NO)  ;

    if (language_selection_option)
    {
      const char *arg=NULL;
      if ((language_selection_option_append->value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(param,language_selection_option)==0)
      {
        if (i+1<param_args().argc())
          arg=param_args()[i+1];
      }
      else if ((language_selection_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(param,language_selection_option,strlen(language_selection_option))==0)
        arg=&language_selection_option[strlen(language_selection_option)];
      const char *language_selection_option_cpp_keywords= table_language_selection_option_cpp_keywords->findOptionTable(arg,false);
      if (language_selection_option_cpp_keywords)
        setLanguage(LANGUAGE_CPP);
      const char *language_selection_option_c_keywords= table_language_selection_option_c_keywords->findOptionTable(arg,false);
      if (language_selection_option_c_keywords)
        setLanguage(LANGUAGE_C);
    }
  }
}

CompilerInterface::type_t CompilerWrapper::type() const
{
  FUNCTION_TRACE;
  int i;
  bool sources = false;
  for (i=1;i<param_args().argc();i++)
  {
    const char *param=param_args()[i];
    const char * compile_option_str= table_compile_option->findOptionTable(param,false)  ;
    if (compile_option_str)
      return CompilerInterface::COMPILER;

    if (isSource(param))
      sources=true;
  }

  if (sources)
    return CompilerInterface::COMPILER_LINKER;
  else
    return CompilerInterface::LINKER;
}

bool  CompilerWrapper::preprocessor (const char *filename,char **command) const
{
  FUNCTION_TRACE;
  Argv args=param_args();
  //RemoveCoverageScannerDefine(args);
  int i;
  bool replace_found;
  std::string str=Source::csmes_c_filename(filename);
  char *filename_str=System::quote_argument(filename);
  char *destination_str=System::quote_argument(str.c_str());
  bool append_coveragescanner_define=true;

  std::string preprocessor_cmd=replace_var(std::string(preprocessor_cmd_str->value()),"$SOURCE$",filename_str,replace_found);
  preprocessor_cmd=replace_var(preprocessor_cmd,"$OUTPUT$",preprocessor_temp_filename(filename).c_str(),replace_found);
  FREE(filename_str); filename_str=NULL;
  FREE(destination_str); destination_str=NULL;

  char *options_str=STRDUP("");
  if (append_coveragescanner_define)
  {
    options_str=(char*)REALLOC(options_str,5+strlen(options_str)+COVERAGESCANNER_DEFINE_LG+strlen(table_preprocessor_define_option->value()[0]));
    strcat(options_str,table_preprocessor_define_option->value()[0]);
    if (preprocessor_define_option_append->value()==CompilerWrapperEnumOption::OPT_NO)
      strcat(options_str," ");
    strcat(options_str,COVERAGESCANNER_DEFINE);
  }

  for (i=1;i<args.argc();i++)
  {
    bool add_option=false;
    const char *param=args[i];
    /* find the arguments */
    const char * compiler_output_option_str= table_compiler_output_option->findOptionTable(param,compiler_output_option_append->value()!=CompilerWrapperEnumOption::OPT_NO)  ;
    const char * link_output_option_str= table_link_output_option->findOptionTable(param,link_output_option_append->value()!=CompilerWrapperEnumOption::OPT_NO)  ;
    const char * compile_option_str= table_compile_option->findOptionTable(param,false)  ;
    const char * link_library_option_str=table_link_library_option->matchBooleanOption(args[i],args[i+1],*link_library_option_append);
    const char * preprocessor_hide_option_1_arg_str= table_preprocessor_hide_option_1_arg->findOptionTable(param,preprocessor_hide_option_1_arg_append->value()!=CompilerWrapperEnumOption::OPT_NO);
    const char * preprocessor_keep_option_1_arg_str= table_preprocessor_keep_option_1_arg->findOptionTable(param,preprocessor_keep_option_1_arg_append->value()!=CompilerWrapperEnumOption::OPT_NO);
    const char * preprocessor_define_option_str= table_preprocessor_define_option->findOptionTable(param,preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_NO)  ;

    /* check the arguments */
    if (compile_option_str && strcmp(param,compile_option_str)==0)
      ;
    else if (preprocessor_define_option_str && (preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(args[i],preprocessor_define_option_str)==0)
    {
      std::string quoted_param=System::quoteArgument(args[i]);
      options_str=(char*)REALLOC(options_str,2+strlen(options_str)+quoted_param.length());
      strcat(options_str," ");
      strcat(options_str,quoted_param.c_str());
      i++; /* add the next entry */
      add_option=true;
    }
    else if (preprocessor_define_option_str && (preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(args[i],preprocessor_define_option_str,strlen(preprocessor_define_option_str))==0)
      add_option=true;
    else if (table_preprocessor_hide_option_no_arg->findOptionTable(param,false))
      ; /* skip this option */
    else if (preprocessor_hide_option_1_arg_str && (preprocessor_hide_option_1_arg_append->value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(args[i],preprocessor_hide_option_1_arg_str)==0)
      i++; /* skip the next entry */
    else if (preprocessor_hide_option_1_arg_str && (preprocessor_hide_option_1_arg_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(args[i],preprocessor_hide_option_1_arg_str,strlen(preprocessor_hide_option_1_arg_str))==0)
      ;
    else if (preprocessor_keep_option_1_arg_str && (preprocessor_keep_option_1_arg_append->value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(args[i],preprocessor_keep_option_1_arg_str)==0)
    {
      std::string quoted_param=System::quoteArgument(args[i]);
      options_str=(char*)REALLOC(options_str,2+strlen(options_str)+quoted_param.length());
      strcat(options_str," ");
      strcat(options_str,quoted_param.c_str());
      i++; /* add the next entry */
      add_option=true;
    }
    else if (preprocessor_keep_option_1_arg_str && (preprocessor_keep_option_1_arg_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(args[i],preprocessor_keep_option_1_arg_str,strlen(preprocessor_keep_option_1_arg_str))==0)
      add_option=true;
    else if (link_output_option_str && (link_output_option_append->value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(args[i],link_output_option_str)==0)
      i++; /* skip the next entry */
    else if (link_output_option_str && (link_output_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(args[i],link_output_option_str,strlen(link_output_option_str))==0)
      ;
    else if (compiler_output_option_str && (compiler_output_option_append->value()!=CompilerWrapperEnumOption::OPT_YES)  && strcmp(args[i],compiler_output_option_str)==0)
      i++; /* skip the next entry */
    else if (compiler_output_option_str && (compiler_output_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(args[i],compiler_output_option_str,strlen(compiler_output_option_str))==0)
      ;
    else if (extension(args[i])==CompilerInterface::EXT_STATIC_LIB)
      ;
    else if (extension(args[i])==CompilerInterface::EXT_DYN_LIB)
      ;
    else if (extension(args[i])==CompilerInterface::EXT_OBJECT)
      ;
    else if (isSource(args[i]))
      ;
    else if (link_library_option_append->value()!=CompilerWrapperEnumOption::OPT_YES && link_library_option_str)
      i++; /* skip the next entry */
    else if (link_library_option_append->value()!=CompilerWrapperEnumOption::OPT_NO && link_library_option_str)
      ;
    else
      add_option=true;

    if (add_option)
    {
      std::string quoted_param=System::quoteArgument(args[i]);
      options_str=(char*)REALLOC(options_str,2+strlen(options_str)+quoted_param.length());
      strcat(options_str," ");
      strcat(options_str,quoted_param.c_str());
    }
  }

  *command=STRDUP(replace_var(preprocessor_cmd,"$OPTIONS$",options_str,replace_found).c_str());
  return true;
}

bool  CompilerWrapper::compiler_option_sbr_pdb(std::list<std::string> &commands, int &i, const CompilerWrapperListOption &option_table,const CompilerWrapperEnumOption &option_append,bool append_name,const char *ext) const
{
  const char *param=param_args()[i];
  const char * option_str= option_table.findOptionTable(param,option_append.value()!=CompilerWrapperEnumOption::OPT_NO)  ;
  if  (option_str && (option_append.value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(param,option_str)==0)
  { 
    commands.push_back(std::string());
    commands.push_back(param);

    i++;
    param=param_args()[i];
    const char *f=param;
    int pos=strlen(f)-1;
    std::string p;
    if ( (append_name) && (f[0]=='\0' || (pos>=0 && (f[pos]==FILE_SEPARATOR_CHAR || f[pos]==FILE_SEPARATOR_CHAR_BIS) ) ) )
    {
      char *basename=extractBasename();
      int f_lg=strlen(f);
      char* a=(char*)MALLOC(f_lg+strlen(basename)+1+strlen(ext));
      strcpy(a,f);
      strcat(a,basename);
      strcat(a,ext);
      FREE(basename);
      p=std::string(a);
      FREE(a);
    }
    else
    {
      p=param;
    }
    commands.push_back(std::string());
    commands.push_back(p);
    return true;
  }
  else if  (option_str && (option_append.value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(param,option_str,strlen(option_str))==0)
  { 
    const char *f=param;
    int pos=strlen(f)-1;
    std::string p;
    if ( (append_name) && (f[0]=='\0' || (pos>=0 && (f[pos]==FILE_SEPARATOR_CHAR || f[pos]==FILE_SEPARATOR_CHAR_BIS) ) ) )
    {
      char *basename=extractBasename();
      int f_lg=strlen(f);
      char* a=(char*)MALLOC(f_lg+strlen(basename)+1+strlen(ext));
      strcpy(a,f);
      strcat(a,basename);
      strcat(a,ext);
      FREE(basename);
      p=a;
      FREE(a);
    }
    else
    {
      p=param;
    }
    commands.push_back(std::string());
    commands.push_back(p);
    return true;
  }
  return false;
}

bool  CompilerWrapper::compiler(char **command_ret_p,bool hide_arguments) const
{
  FUNCTION_TRACE;
  bool in_cmd_line;
  char *dest;
  destination(&dest,in_cmd_line);
  int i;
  FileName source;
  sources(&source);

  std::list<std::string> commands;
  if (!in_cmd_line)
  {
    /* append the output */
    const char *output_option_str=NULL;
    switch (type())
    {
      case CompilerInterface::COMPILER_LINKER:
      case CompilerInterface::LINKER:
        output_option_str=(*table_link_output_option)[0];
        break;
      case CompilerInterface::COMPILER:
        output_option_str=(*table_compiler_output_option)[0];
        break;
      case CompilerInterface::UNDEFINED:
        FATAL1("Internal Error");
        break;
    }
    if (output_option_str)
    {
      commands.push_back(std::string(output_option_str));
      if (compiler_output_option_append->value()==CompilerWrapperEnumOption::OPT_NO)
        commands.push_back(std::string());
    }
    commands.push_back(dest);
  }
  for (i=1;i<param_args().argc();i++)
  {
    const char*param=param_args()[i];
    if (source.find(param))
    {
      std::string str;
      if (ignoreSource(param))
        str=std::string(param);
      else
        str = Source::csmes_c_filename(param);
      commands.push_back(std::string());
      commands.push_back(str);
    }
    else
    {
      /* find the correct argument */
      const char * compiler_output_option_str= table_compiler_output_option->findOptionTable(param,compiler_output_option_append->value()!=CompilerWrapperEnumOption::OPT_NO)  ;
      const char * preprocessor_define_option_str= table_preprocessor_define_option->findOptionTable(param,preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_NO)  ;
      const char * compiler_hide_option_1_arg_str= table_compiler_hide_option_1_arg->findOptionTable(param,compiler_hide_option_1_arg_append->value()!=CompilerWrapperEnumOption::OPT_NO);


      if (compiler_output_option_str && !in_cmd_line && (compiler_output_option_append->value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(param,compiler_output_option_str)==0)
        i++; /* skip next param */
      else if (table_compiler_hide_option_no_arg->findOptionTable(param,false))
        ; /* skip this option */
      else if (hide_arguments && compiler_hide_option_1_arg_str && (compiler_hide_option_1_arg_append->value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(param_args()[i],compiler_hide_option_1_arg_str)==0)
        i++; /* skip the next entry */
      else if (hide_arguments && compiler_hide_option_1_arg_str && (compiler_hide_option_1_arg_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(param_args()[i],compiler_hide_option_1_arg_str,strlen(compiler_hide_option_1_arg_str))==0)
        ;
      else if (compiler_output_option_str && !in_cmd_line && (compiler_output_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(param,compiler_output_option_str,strlen(compiler_output_option_str))==0)
        ;
      else if (compiler_option_sbr_pdb(commands,i,*table_sbr_output_option,*sbr_output_option_append,SBR_APPEND_NAME,sbr_ext()[0]))
        ;
      else if (compiler_option_sbr_pdb(commands,i,*table_pdb_output_option,*pdb_output_option_append,PDB_APPEND_NAME,pdb_ext()[0]))
        ;
      else if (hide_arguments && preprocessor_define_option_str && (preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(param,preprocessor_define_option_str)==0)
        i++; /* skip next param */
      else if (hide_arguments && preprocessor_define_option_str && (preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(param,preprocessor_define_option_str,strlen(preprocessor_define_option_str))==0)
        ;
      else
      {
        commands.push_back(std::string());
        commands.push_back(param);
      }
    }
  }

  FREE(dest);
  std::string command=generateCommand(std::string(tool_str->value()),commands);
  *command_ret_p=STRDUP(command.c_str());
  DEBUG2("compiler_cmd=%s\n",*command_ret_p);

  return true;
}

bool  CompilerWrapper::compile_file(char **command_p,const char *source, const char* destination) const
{
  FUNCTION_TRACE;
  bool replace_found;
  std::string source_str=System::quoteArgument(source);
  std::string destination_str=System::quoteArgument(destination);
  std::string tmp1=replace_var(std::string(compiler_cmd_str->value()),"$SOURCE$",source_str.c_str(),replace_found);
  std::string tmp2=replace_var(tmp1,"$LIBGEN$",libgen_str,replace_found);
  *command_p=STRDUP(replace_var(tmp2,"$DESTINATION$",destination_str.c_str(),replace_found).c_str());

  return true;
}

bool  CompilerWrapper::destination(char **dest_p,bool &in_cmd_line) const
{
  FUNCTION_TRACE;
  int i=0;

  if (ar_command_format_option->value()==CompilerWrapperEnumOption::OPT_YES)
  {
    in_cmd_line=true;
    int cmd_count=0;
    for (i=1;i<param_args().argc();i++)
    {
      if (param_args()[i][0]!='-')
        cmd_count++;

      if (cmd_count==2)
      {
        *dest_p=STRDUP(param_args()[i]);
        DEBUG2("Destination:%s\n",*dest_p);
        return true;
      }
    }
  }

  in_cmd_line=false;

  /* find first the implicit statistic library */
  for (i=1;i<param_args().argc();i++)
  {
    const char * dll_output_static_lib=table_dll_output_static_lib->matchBooleanOption(param_args()[i],param_args()[i+1],*dll_output_static_lib_append);
    /* find the correct argument */
    if (dll_output_static_lib)
    {
      const char *f=dll_output_static_lib;
      int pos=strlen(f)-1;
      if (f[0]=='\0' || (pos>=0 && (f[pos]==FILE_SEPARATOR_CHAR || f[pos]==FILE_SEPARATOR_CHAR_BIS) ) )
      {
        std::string fn=replace_basename_src(compiler_output_default_str->value());
        int f_lg=strlen(f);
        *dest_p=(char*)MALLOC(f_lg+fn.length()+1);
        strcpy(*dest_p,f);
        strcat(*dest_p,fn.c_str());
        in_cmd_line=false;
        DEBUG2("Destination:%s\n",*dest_p);
        return true;
      }
      *dest_p=STRDUP(f);
      in_cmd_line=true;
      DEBUG2("Destination:%s\n",*dest_p);
      return true;
    }
  }

  switch(type())
  {
    case COMPILER:
      {
        for (i=1;i<param_args().argc();i++)
        {
          const char * compiler_output_option_str=table_compiler_output_option->matchBooleanOption(param_args()[i],param_args()[i+1],*compiler_output_option_append);
          /* find the correct argument */
          if (compiler_output_option_str)
          {
            const char *f=compiler_output_option_str;
            int pos=strlen(f)-1;
            if (f[0]=='\0' || (pos>=0 && (f[pos]==FILE_SEPARATOR_CHAR || f[pos]==FILE_SEPARATOR_CHAR_BIS) ) )
            {
              std::string fn=replace_basename_src(compiler_output_default_str->value());
              int f_lg=strlen(f);
              *dest_p=(char*)MALLOC(f_lg+fn.length()+1);
              strcpy(*dest_p,f);
              strcat(*dest_p,fn.c_str());
              in_cmd_line=false;
              DEBUG2("Destination:%s\n",*dest_p);
              return true;
            }
            *dest_p=STRDUP(f);
            in_cmd_line=true;
            DEBUG2("Destination:%s\n",*dest_p);
            return true;
          }
        }
      }

      *dest_p=STRDUP(replace_basename_src(compiler_output_default_str->value()).c_str());
      DEBUG2("Destination:%s\n",*dest_p);
      return true;
    default:
      {
        DEBUG2("table_link_output_option=%s\n",table_link_output_option->description());
        for (i=1;i<param_args_full_args().argc();i++)
        {
          const char * link_output_option_str= table_link_output_option->matchBooleanOption(param_args_full_args()[i],param_args_full_args()[i+1],*link_output_option_append)  ;
          DEBUG2("param=%s\n",param_args_full_args()[i]);
          if (link_output_option_str)
          {
            if (static_lib_as_default->value()==CompilerWrapperEnumOption::OPT_YES)
            {
              std::string dest=std::string(link_output_option_str);
              std::string ext;
              suppressExtension(dest,ext);
              ext="."+ext;
              bool found=false;
              for (int j=0;((*table_dyn_lib_ext)[j]) && !found;j++)
                found=ext_strcmp(ext.c_str(),(*table_dyn_lib_ext)[j])==0;
              if (found)
              {
                dest+=std::string(static_lib_ext()[0]);
                in_cmd_line=false;
                *dest_p=STRDUP(dest.c_str());
              }
              else
              {
                *dest_p=STRDUP(link_output_option_str);
                in_cmd_line=true;
              }
            }
            else
            {
              *dest_p=STRDUP(link_output_option_str);
              in_cmd_line=true;
            }
            DEBUG2("Destination:%s\n",*dest_p);
            return true;
          }
        }
      }

      *dest_p=STRDUP(replace_basename_src(link_output_default_str->value()).c_str());
      DEBUG2("Destination:%s\n",*dest_p);
     return true;
  }
}

const char *const * CompilerWrapper::dyn_lib_ext() const
{
  FUNCTION_TRACE;
  return table_dyn_lib_ext->value();
}
const char *const * CompilerWrapper::static_lib_ext() const
{ 
  FUNCTION_TRACE;
  return table_static_lib_ext->value();
}
const char *const * CompilerWrapper::sbr_ext() const
{ 
  FUNCTION_TRACE;
  return table_sbr_ext->value();
}
const char *const * CompilerWrapper::pdb_ext() const
{ 
  FUNCTION_TRACE;
  return table_pdb_ext->value();
}
const char *const * CompilerWrapper::obj_ext() const
{ 
  FUNCTION_TRACE;
  return table_obj_ext->value();
}
const char *const * CompilerWrapper::cpp_ext() const
{ 
  FUNCTION_TRACE;
  return table_cpp_ext->value();
}
const char *const * CompilerWrapper::c_ext() const
{ 
  FUNCTION_TRACE;
  return table_c_ext->value();
}

const char *const * CompilerWrapper::cs_ext() const
{ 
  FUNCTION_TRACE;
  return table_cs_ext->value();
}

const char *const * CompilerWrapper::ignore_ext() const
{ 
  FUNCTION_TRACE;
  return table_ignore_ext->value();
}


Expression::code_style_t CompilerWrapper::codeStyle() const
{
  FUNCTION_TRACE;
  return language_supported->value();
}


std::string CompilerWrapper::replace_var(const std::string &str,const char*var,const char*val,bool &found)
{
  FUNCTION_TRACE;
  found=false;
  if (val==NULL)
    return str;
  char *ret=NULL;
  const char *pos1=str.c_str();
  const char *pos2=str.c_str();
  int ret_lg=1;
  int val_lg=strlen(val);
  int var_lg=strlen(var);

  while ((pos2=strstr(pos1,var))!=NULL)
  {
    DEBUG4("Replacing variable '%s'->'%s' in '%s'\n",var,val,str.c_str());
    int lg=pos2-pos1;
    ret=(char*)REALLOC(ret,ret_lg+val_lg+lg);
    strncpy(&ret[ret_lg-1],pos1,lg);
    ret_lg+=lg;
    strcpy(&ret[ret_lg-1],val);
    ret_lg+=val_lg;
    pos1=pos2+var_lg;
    found=true;
  }
  int lg=strlen(pos1);
  ret=(char*)REALLOC(ret,ret_lg+lg);
  strcpy(&ret[ret_lg-1],pos1);
  std::string retstr=std::string(ret);
  FREE(ret);
  return retstr;
}


void CompilerWrapper::replace_std_var(CompilerWrapperTextOption *v)
{
  FUNCTION_TRACE;
  if (!v->defined())
    return ;
  bool replace_performed;
  std::string var=std::string(v->value());
  if (profile_path)
    var=replace_var(var,"$PROFILE_PATH$",profile_path,replace_performed); 
  var=replace_var(var,"$TOOL$",tool_str->value(),replace_performed); 
  var=replace_var(var,"$PROFILE$",option.toolName().c_str(),replace_performed); 
  var=replace_var(var,"$COMPILER_CMD$",compiler_cmd_str->value(),replace_performed); 
  var=replace_var(var,"$COMPILER_OUTPUT_DEFAULT$",compiler_output_default_str->value(),replace_performed); 
  //var=replace_var(var,"$COMPILER_OUTPUT_OPTION$",table_compiler_output_option[0],replace_performed); 
  for (int i=1;i<10;i++)
  {
    char tmp_name[CHAINE_LEN];
    char tmp_var[CHAINE_LEN];
    sprintf(tmp_name,"cs_$ID$_%i.tmp",i);
    sprintf(tmp_var,"$TMP%i$",i);
    std::string tmp_file=TmpFile::object().generate(tmp_name,true);

    var=replace_var(var,tmp_var,tmp_file.c_str(),replace_performed); 
    if (!replace_performed)
      TmpFile::object().removeUnusedFile(tmp_file);
  }
  v->setValue(var.c_str());
}


void CompilerWrapper::display_temporary_files() const
{
  FUNCTION_TRACE;
  DEBUG1("Displaying temporary files\n");
  for (int i=1;i<10;i++)
  {
    char tmp_name[CHAINE_LEN];
    sprintf(tmp_name,"cs_$ID$_%i.tmp",i);
    std::string tmp_file=TmpFile::object().generate(tmp_name,true);

    DEBUG2("\n----- begin %s -----\n",tmp_file.c_str());
    FILE *f=fopen(tmp_file.c_str(),"r");
    if (f)
    {
      char tmp[CHAINE_LEN];
      while (fgets(tmp,CHAINE_LEN,f)!=NULL)
      {
        fputs(tmp,stdout);
        DEBUG2("%s",tmp);
      }
    }
    DEBUG2("\n----- end %s -----\n",tmp_file.c_str());
  }
}

std::string CompilerWrapper::replace_basename_src(const std::string &var) const
{
  FUNCTION_TRACE;
  bool replace_performed;
  char *basename_src=extractBasename();
  if (basename_src)
  {
    std::string tmp=replace_var(var,"$BASENAME_SRC$",System::quoteArgument(basename_src).c_str(),replace_performed);
    return tmp;
  }
  return var;
}

void CompilerWrapper::suppressExtension(std::string &s,std::string &e)
{
  char *_s=STRDUP(s.c_str());
  char *_e;
  suppressExtension(_s,_e);
  s=std::string(_s);
  e=std::string(_e);
  FREE(_s);
}

void CompilerWrapper::suppressExtension(char *s,char *&e)
{
  int lg=strlen(s);
  e=&s[lg];
  for (int i=lg-1;i>=0;i--)
  {
    if (s[i]=='.')
    {
      s[i]='\0';
      e=&s[i+1];
      break;
    }
  }
}

char * CompilerWrapper::extractBasename() const
{
  FUNCTION_TRACE;
  int i;
  char *basename_src=NULL;
  int argc=param_args().argc();
  for (i=1;i<argc;i++)
  {
    if (isSource(param_args()[i]))
    {
        const char*param=param_args()[i];
        /* strip the directory */
        for (i=strlen(param);i>=0;i--)
        {
            if ( (param[i]==FILE_SEPARATOR_CHAR) || (param[i]==FILE_SEPARATOR_CHAR_BIS) )
            {
                param=&param[i+1];
                break;
            }
        }

        /* strip the extension */
      basename_src=STRDUP(param);
      char *e;
      suppressExtension(basename_src,e);
      return basename_src;
    }
  }
  return basename_src;
}

const char *const *CompilerWrapper::exit_funct_ext() const
{
  FUNCTION_TRACE;
  return table_exit_func_ext->value();
}

const char* CompilerWrapper::responseFile(int index,int &arg_index_min,int &arg_index_max) const
{
  FUNCTION_TRACE;
  int argc_orig=param_args_org().argc();

  int lg_rsp=strlen(response_file_str->value());
  for (arg_index_min=0;arg_index_min<argc_orig;arg_index_min++)
  {
    bool response_file=false;
    const char *response_file_option_str;
    const char *arg=param_args_org()[arg_index_min];
    int lg_arg=strlen(arg);
    for (int j=0;(response_file_option_str=(*table_response_file_ext)[j]);j++)
    {
      int pos;
      if ( ((pos=lg_arg-strlen((*table_response_file_ext)[j]))>0) 
          && strcmp(&arg[pos],(*table_response_file_ext)[j])==0 )
        response_file=true;
    }

    
    if (strcmp(arg,response_file_str->value())==0)
    {
      arg_index_max=arg_index_min+1;
      const char *rsp_file=param_args_org()[arg_index_min+1];
      if (index--==0) { return rsp_file; }
      arg_index_min++; /* skip the next entry */
    }
    else if ((response_file_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(arg,response_file_str->value(),lg_rsp)==0)
    {
      arg_index_max=arg_index_min;
      const char *rsp_file=&(arg[lg_rsp]);
      if (index--==0) { return rsp_file; }
    }
	else if (response_file)
    {
      arg_index_max=arg_index_min;
      const char *rsp_file=arg;
      if (index--==0) { return rsp_file; }
    }
  }
  return NULL;
}


bool CompilerWrapper::forceDllExport() const
{
  FUNCTION_TRACE;
  int i;

  if (force_dll_export->value()==CompilerWrapperEnumOption::OPT_YES)
    return true;

  for (i=0; i<param_args().argc() ; i++)
  {
    const char *param=param_args()[i];

    if (table_force_dll_export_option->findOptionTable(param,false))
      return true;
  }
  return false;
}
      
void CompilerWrapper::stripArgs()
{
  FUNCTION_TRACE;
  int i;
  int argc_orig=param_args_org().argc();
  Argv args;

  for (i=0; i<argc_orig ; i++)
  {
    const char *param=param_args_org()[i];

    if (table_hide_option_no_arg->findOptionTable(param,false))
      ; /* skip this option */
    else if (table_hide_option_1_arg->findOptionTable(param,hide_option_1_arg_append->value()!=CompilerWrapperEnumOption::OPT_NO))
    {
      if (hide_option_1_arg_append->value()!=CompilerWrapperEnumOption::OPT_NO)
        ; /* skip this option */
      else
        i++; /* skip 2 arguments */
    }
    else
    {
        /* append the argument */
      args.append(param_args_org()[i]);
    }
  }

  setArgs(args);
}



const char* CompilerWrapper::function_attribute() const
{
  FUNCTION_TRACE;
  return function_attribute_str->value();
} 

const char* CompilerWrapper::dll_export() const
{
  FUNCTION_TRACE;
  return dll_export_str->value();
} 

const char* CompilerWrapper::dll_import() const
{
  FUNCTION_TRACE;
  return dll_import_str->value();
} 

bool CompilerWrapper::unixLibraryName() const
{
  FUNCTION_TRACE;
  return unix_library_name->value()==CompilerWrapperEnumOption::OPT_YES;
}

bool CompilerWrapper::setupCpp() const
{
  FUNCTION_TRACE;
  return custom_setup->value()==CompilerWrapperEnumOption::OPT_CPP;
}

bool CompilerWrapper::setupMSCE() const
{
  FUNCTION_TRACE;
  return custom_setup->value()==CompilerWrapperEnumOption::OPT_MSCE;
}

bool CompilerWrapper::setupMS() const
{
  FUNCTION_TRACE;
  return custom_setup->value()==CompilerWrapperEnumOption::OPT_MS;
}

bool CompilerWrapper::setupGNU() const
{
  FUNCTION_TRACE;
  return custom_setup->value()==CompilerWrapperEnumOption::OPT_GNU;
}

bool CompilerWrapper::setupPOSIX() const
{
  FUNCTION_TRACE;
  return custom_setup->value()==CompilerWrapperEnumOption::OPT_POSIX;
}

bool CompilerWrapper::customSetup() const
{
  FUNCTION_TRACE;
  return custom_setup->value()==CompilerWrapperEnumOption::OPT_NONE;
}


void CompilerWrapper::BuildLibraryPath(FileName &library_path) const
{
  FUNCTION_TRACE;
  const char *item;
  library_path.clear();
  library_path.append("");
  /* extract the path from the command line */
  for (int  i=0;i<param_args().argc();i++)
  {
    const char * library_path_str=table_library_path_option->matchBooleanOption(param_args()[i],param_args()[i+1],*library_path_option_append);
    if (library_path_str)
      library_path.append(library_path_str);
  }

  /* add the default library */
  for (int j=0;(item=(*table_default_lib_path)[j]);j++)
    library_path.append(item);
}

char * CompilerWrapper::libraryPath(const char *filename,const FileName &library_path) const
{
  FUNCTION_TRACE;
  if (filename[0]==FILE_SEPARATOR_CHAR || filename[0]==FILE_SEPARATOR_CHAR_BIS)
  {
    return STRDUP(filename);
  }

  int file_lg = 0;
  int filename_lg=strlen(filename);
  int library_path_count=library_path.count();
  int lg;
  int i;

  /* calculating the maximum size */
  for (i=0;i<library_path_count;i++)
    if (file_lg<1+1+filename_lg+(lg=strlen(library_path.at(i))))
      file_lg=1+1+filename_lg+lg;

  const char *item; int lg_tmp; lg=0;
  int j;
  for (j=0;(item=(*table_static_lib_ext)[j]);j++)
    if (lg<(lg_tmp=strlen((*table_static_lib_ext)[j])))
      lg=lg_tmp;
  for (j=0;(item=(*table_dyn_lib_ext)[j]);j++)
    if (lg<(lg_tmp=strlen((*table_dyn_lib_ext)[j])))
      lg=lg_tmp;
  file_lg+=lg;

  file_lg+=3 /*"lib"*/ + CHAINE_LEN;

  char *file=(char*)MALLOC(file_lg);
  char *filepath=(char*)MALLOC(file_lg);
  for (i=0;i<library_path_count;i++)
  {
    DEBUG4("Analysing path (%i/%i) '%s'\n",i+1,library_path_count,library_path.at(i));
    struct stat info_file;
    strcpy(filepath,library_path.at(i));
    if (filepath[0]!='\0')
    {
      lg=strlen(filepath);
      filepath[lg]=FILE_SEPARATOR_CHAR ;
      filepath[lg+1]='\0' ;
    }
    strcpy(file,filepath);
    strcat(file,filename);

    /* first try the file without extension */
    if (stat(file,&info_file)==0)
    {
      FREE(filepath);
      return file;
    }

    lg=strlen(file);
    /* try the file without dynamic library extension */
    for (j=0;(item=(*table_dyn_lib_ext)[j]);j++)
    {
      strcpy(&file[lg],(*table_dyn_lib_ext)[j]);
      if (stat(file,&info_file)==0)
      {
        FREE(filepath);
        return file;
      }
    }

    /* try the file without dynamic static extension */
    for (j=0;(item=(*table_static_lib_ext)[j]);j++)
    {
      strcpy(&file[lg],(*table_static_lib_ext)[j]);
      if (stat(file,&info_file)==0)
      {
        FREE(filepath);
        return file;
      }
    }

    if (unixLibraryName())
    {
      DIR *dp;
      struct dirent *entry;
      DEBUG2("Analysing path '%s'\n",library_path.at(i));

      if((dp = opendir(library_path.at(i))) == NULL)
      {
        DEBUG2("Error openning directory:%s\n",library_path.at(i));
      }
      else 
      {
        char *libfilename=(char*)MALLOC(file_lg);
        strcpy(libfilename,"lib");
        strcat(libfilename,filename);
        strcat(libfilename,".");
        int libfilename_lg=strlen(libfilename);
        while((entry = readdir(dp)) != NULL)
        {
          if (strncmp(libfilename,entry->d_name,libfilename_lg)==0)
          {
            /* check if the shared object extension is present */
            bool found=false;
            for (j=0;((*table_dyn_lib_ext)[j]) && !found;j++)
              found=ext_strncmp(&(entry->d_name[libfilename_lg-1]),(*table_dyn_lib_ext)[j],strlen((*table_dyn_lib_ext)[j]))==0;
            for (j=0;((*table_static_lib_ext)[j]) && !found;j++)
              found=ext_strncmp(&(entry->d_name[libfilename_lg-1]),(*table_static_lib_ext)[j],strlen((*table_static_lib_ext)[j]))==0;

            if (found)
            {
              int entry_lg=strlen(entry->d_name);
              if (entry_lg>6 && ext_strcmp(".csmes",&(entry->d_name[entry_lg-6]))==0)
              { // .csmes extension detected
                DEBUG2("Library Found:%s\n",entry->d_name);
                FREE(libfilename);
                strcpy(file,filepath);
                FREE(filepath);
                strcat(file,entry->d_name);
                int file_lg=strlen(file);
                file[file_lg-6]='\0'; // removing .csmes
                return file;
              }
            }
          }
        }
        FREE(libfilename);
        closedir(dp);
      }
    }
  }
  FREE(filepath);
  FREE(file);
  return NULL;
}

bool CompilerWrapper::libraries(FileName *filenames) const
{
  FUNCTION_TRACE;
  DEBUG1("libraries()\n");
  FileName library_path;
  BuildLibraryPath(library_path);

  filenames->clear();
  for (int i=1;i<param_args_full_args().argc();i++)
  {
    const char * link_library_option_str=table_link_library_option->matchBooleanOption(param_args_full_args()[i],param_args_full_args()[i+1],*link_library_option_append);
    const char * dll_output_static_lib=table_dll_output_static_lib->matchBooleanOption(param_args_full_args()[i],param_args_full_args()[i+1],*dll_output_static_lib_append);
    const char * link_output_option_str= table_link_output_option->matchBooleanOption(param_args_full_args()[i],param_args_full_args()[i+1],*link_output_option_append)  ;
    /* find the correct argument */
    char *path=NULL;
    if (link_output_option_str)
    {
      if (link_output_option_str==param_args_full_args()[i+1]) i++;
    }
    else if (dll_output_static_lib)
    {
      if (dll_output_static_lib==param_args_full_args()[i+1]) i++;
    }
    else if (link_library_option_str)
    {
      path=libraryPath(link_library_option_str,library_path);
      if (link_library_option_str==param_args_full_args()[i+1]) i++;
    }
    else if (isLibrary(param_args_full_args()[i]))
      path=libraryPath(param_args_full_args()[i],library_path);

    if (path)
    {
      DEBUG2("Library found '%s'\n",path);
      filenames->append(path);
      FREE(path);
    }
  }
  return true;
}

/*! This function permits to identify if the command line 
 * argument is corresponding to the destination file 
 */
bool CompilerWrapper::isDestination(const char*argument) const
{
  FUNCTION_TRACE;
  bool res;
  bool in_cmd_line;
  char *dest;
  destination(&dest,in_cmd_line);
  const char * compiler_output_option_str ;
  const char * link_output_option_str ;

  if (filename_strcmp(dest,argument)==0)
    res=true;
  else if ( 
      (compiler_output_option_str = table_compiler_output_option->findOptionTable(argument,compiler_output_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) )
      && (compiler_output_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) 
      && strncmp(argument,compiler_output_option_str,strlen(compiler_output_option_str))==0)
    res=true;
  else if ( 
      ( link_output_option_str = table_link_output_option->findOptionTable(argument,link_output_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) )
      && (link_output_option_append->value()!=CompilerWrapperEnumOption::OPT_NO)
      && strncmp(argument,link_output_option_str,strlen(link_output_option_str))==0)
    res=true;
  else
    res=false;

  FREE(dest);
  return res;
}

bool CompilerWrapper::enableCoveragescannerPerDefault() const
{
  return (enable_coveragescanner_per_default->value()==CompilerWrapperEnumOption::OPT_YES);
}

bool CompilerWrapper::deactivateCoverageScanner(const Argv &args) const
{
  FUNCTION_TRACE;
  if (deactivate_coveragescanner->value()==CompilerWrapperEnumOption::OPT_YES)
    return true;
  for (int i=1;i<args.argc();i++)
  {
    const char *param=args[i];
    const char *param_next=args[i+1];
    /* find the arguments */
    const char * deactivate_option_no_arg_str= table_deactivate_coveragescanner_option_no_arg->findOptionTable(param,false)  ;
    /* check the arguments */
    if (deactivate_option_no_arg_str)
      return true;

    if (i!=args.argc()-1)
    {
      const char * deactivate_option_1_arg_str=table_deactivate_coveragescanner_option_1_arg->matchBooleanOption(param,param_next,*deactivate_coveragescanner_option_1_arg_append);

      /* check the arguments */
      if (deactivate_option_1_arg_str)
        return true;
    }
  }
  return false;
}

bool CompilerWrapper::generateCoverageScannerLibrary() const 
{
  FUNCTION_TRACE;
  if (generate_coveragescanner_library->value()==CompilerWrapperEnumOption::OPT_NO)
    return false;

  for (int i=0; i<param_args().argc() ; i++)
  {
    const char *param=param_args()[i];

    if (table_skip_generate_coveragescanner_library_option->findOptionTable(param,false))
      return false;
  }
  return true;
}

bool CompilerWrapper::objects(FileName *filenames) const
{
  FUNCTION_TRACE;
  int i;
  filenames->clear();
  for (i=1;i<param_args_full_args().argc();i++)
  {
    const char *file=param_args_full_args()[i];
    if (!isDestination(file))
      if (isObject(file))
        filenames->append(file);
  }

  return true;
}

bool CompilerWrapper::stdinMriScript(const Argv &args) const
{
  FUNCTION_TRACE;
  for (int i=1;i<args.argc();i++)
  {
    const char *param=args[i];
    /* find the arguments */
    const char * stdin_mri_script_option= table_stdin_mri_script_option->findOptionTable(param,false)  ;
    /* check the arguments */
    if (stdin_mri_script_option)
      return true;
  }
  return false;
}

bool CompilerWrapper::usePreprocessor() const 
{
  return preprocessor_cmd_str->defined();
}

bool CompilerWrapper::useCompilerToGenerateObjects() const
{
  return compiler_cmd_str->defined();
}

const char* CompilerWrapper::CoverageScannerDefineOptionInArguments() const
{
  FUNCTION_TRACE;
  int argc=param_args().argc();
  bool found_in_arg=false;
  for (int i=1;i<argc && !found_in_arg;i++)
  {
    const char *param=param_args()[i];
    /* find the arguments */
    const char * preprocessor_define_option_str= table_preprocessor_define_option->findOptionTable(param,(preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_NO))  ;

    const char *arg=NULL;
    /* check the arguments */
    if (preprocessor_define_option_str && (preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(param,preprocessor_define_option_str)==0)
    {
      i++; /* skip next param */
      if (i<argc)
        arg=param_args()[i];
    }
    else if (preprocessor_define_option_str && (preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(param,preprocessor_define_option_str,strlen(preprocessor_define_option_str))==0)
    {
      int index=strlen(preprocessor_define_option_str);
      arg=&param_args()[i][index];
    }
    if (arg)
    {
      const char *coveragescanner_define=COVERAGESCANNER_DEFINE"=";
      int coveragescanner_define_lg=COVERAGESCANNER_DEFINE_LG+1;
      if (strncmp(coveragescanner_define,arg,coveragescanner_define_lg)==0)
        return &arg[COVERAGESCANNER_DEFINE_LG+1];
    }
  }

  return NULL;
}

void CompilerWrapper::RemoveCoverageScannerDefine(Argv &args) const
{
  FUNCTION_TRACE;
  Argv new_param_args;

  const char *coveragescanner_define=COVERAGESCANNER_DEFINE;
  int coveragescanner_define_lg=COVERAGESCANNER_DEFINE_LG;
  for (int i=0;i<args.argc();i++)
  {
    const char *param=args[i];
    bool append_arg=true;
    /* find the arguments */
    const char * preprocessor_define_option_str= table_preprocessor_define_option->findOptionTable(param,(preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_NO))  ;

    /* check the arguments */
    if (preprocessor_define_option_str)
    {
      if ((preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(param,preprocessor_define_option_str)==0)
      {
        if (i+1<args.argc())
        {
          const char *arg=args[i+1];
          if (strncmp(coveragescanner_define,arg,coveragescanner_define_lg)==0)
          {
            append_arg=false;
            i++; /* skip next param */
          }
        }
      }
      if ((preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(param,preprocessor_define_option_str,strlen(preprocessor_define_option_str))==0)
      {
        int index=strlen(preprocessor_define_option_str);
        const char * arg=&args[i][index];
        if (strncmp(coveragescanner_define,arg,coveragescanner_define_lg)==0)
          append_arg=false;
      }
    }
    if (append_arg)
      new_param_args.append(param);
  }
  args=new_param_args;
}

bool CompilerWrapper::defineMatchCoverageOn(const char*arg) const 
{
  if (coveragescanner_coverage_on->defined())
    return  coveragescanner_coverage_on->match(arg);

  return false;
}


bool CompilerWrapper::defineMatchCoverageCount(const char*arg) const 
{
  if (coveragescanner_coverage_count->defined())
    return  coveragescanner_coverage_count->match(arg);

  return false;
}

bool CompilerWrapper::defineMatchCoverageHit(const char*arg) const 
{
  if (coveragescanner_coverage_hit->defined())
    return  coveragescanner_coverage_hit->match(arg);

  return false;
}

bool CompilerWrapper::defineMatchCoverageBranch(const char*arg) const 
{
  if (coveragescanner_coverage_branch->defined())
    return  coveragescanner_coverage_branch->match(arg);

  return false;
}

bool CompilerWrapper::defineMatchCoverageMDCS(const char*arg) const 
{
  if (coveragescanner_coverage_mdcs->defined())
    return  coveragescanner_coverage_mdcs->match(arg);

  return false;
}

bool CompilerWrapper::defineMatchCoverageDecition(const char*arg) const 
{
  if (coveragescanner_coverage_decision->defined())
    return  coveragescanner_coverage_decision->match(arg);

  return false;
}

bool CompilerWrapper::defineMatchCoverageCondition(const char*arg) const 
{
  if (coveragescanner_coverage_condition->defined())
    return  coveragescanner_coverage_condition->match(arg);

  return false;
}

bool CompilerWrapper::defineMatchCoverageLockCsexe(const char*arg) const 
{
  if (coveragescanner_coverage_lock_csexe->defined())
    return  coveragescanner_coverage_lock_csexe->match(arg);

  return false;
}

bool CompilerWrapper::defineMatchCoverageNoLockCsexe(const char*arg) const 
{
  if (coveragescanner_coverage_nolock_csexe->defined())
    return  coveragescanner_coverage_nolock_csexe->match(arg);

  return false;
}

bool CompilerWrapper::defineMatchCoveragePartialInstrumentation(const char*arg) const 
{
  if (coveragescanner_coverage_partial_instrumentation->defined())
    return  coveragescanner_coverage_partial_instrumentation->match(arg);

  return false;
}

bool CompilerWrapper::defineMatchCoverageFullInstrumentation(const char*arg) const 
{
  if (coveragescanner_coverage_full_instrumentation->defined())
    return  coveragescanner_coverage_full_instrumentation->match(arg);

  return false;
}

bool CompilerWrapper::defineMatchCoverageNoAssignment(const char*arg) const 
{
  if (coveragescanner_coverage_no_assignment->defined())
    return  coveragescanner_coverage_no_assignment->match(arg);

  return false;
}

bool CompilerWrapper::defineMatchCoverageNoExceptions(const char*arg) const 
{
  if (coveragescanner_coverage_no_exceptions->defined())
    return  coveragescanner_coverage_no_exceptions->match(arg);

  return false;
}

Defines CompilerWrapper::defineFromCommandLine() const
{
  FUNCTION_TRACE;
  Defines defs;

  for (int i=0;i<param_args().argc();i++)
  {
    const char *param=param_args()[i];
    const char *define=NULL;
    /* find the arguments */
    const char * preprocessor_define_option_str= table_preprocessor_define_option->findOptionTable(param,(preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_NO))  ;

    /* check the arguments */
    if (preprocessor_define_option_str)
    {
      if ((preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_YES) && strcmp(param,preprocessor_define_option_str)==0)
      {
        if (i+1<param_args().argc())
        {
          define=param_args()[i+1];
          i++; /* skip next param */
        }
      }
      if ((preprocessor_define_option_append->value()!=CompilerWrapperEnumOption::OPT_NO) && strncmp(param,preprocessor_define_option_str,strlen(preprocessor_define_option_str))==0)
      {
        int index=strlen(preprocessor_define_option_str);
        define=&param_args()[i][index];
      }
    }
    if (define)
    {
      if (preprocessor_define_separator_option->defined())
      {
        const std::string defines(define);
        std::string::size_type pos1=0;
        std::string::size_type pos2=0;
        int lg=strlen(preprocessor_define_separator_option->value());
        for (;;)
        {
          pos2=defines.find(preprocessor_define_separator_option->value(),pos1);
          if (pos2==std::string::npos)
          {
            defs.define(defines.substr(pos1).c_str());
            break;
          }
          defs.define(defines.substr(pos1,pos2-pos1).c_str());
          pos1=pos2+lg;
        }
      }
      else
        defs.define(define);
    }
  }
  return defs;
}

bool CompilerWrapper::preprocessor_use_temp_filename() const
{
  ASSERT(usePreprocessor());

  return (std::string(preprocessor_cmd_str->value()).find("$OUTPUT$")!=std::string::npos);
}

std::string CompilerWrapper::preprocessor_temp_filename(const std::string &name) const
{
  return Source::csmes_c_filename(name.c_str());
}

bool CompilerWrapper::append_cslibrary(char **new_command_p,const char *compiler_command,const char *libname) const
{
  *new_command_p=(char*)MALLOC(strlen(linkAdditionalArguments().c_str())+strlen(libname)+8+2*strlen(compiler_command));
  strcpy(*new_command_p,compiler_command);
  strcat(*new_command_p," ");
  strcat(*new_command_p,System::quoteArgument(libname).c_str());
  strcat(*new_command_p," ");
  strcat(*new_command_p,linkAdditionalArguments().c_str());
  
  return true;
}

std::string CompilerWrapper::generateCommandWithoutResponseFile(const std::string &executable,const std::list<std::string> &params) const
{
  std::string ret=executable+" ";
  std::string a;
  bool last_was_empty=true;
  for (std::list<std::string>::const_iterator it=params.begin();it!=params.end();++it)
  {
    if (it->empty())
    {
      if (!last_was_empty)
        ret+=" ";
      last_was_empty=true;
    }
    else
    {
      ret+=System::quoteArgument(*it);
      last_was_empty=false;
    }
  }
  return ret;
}

std::string CompilerWrapper::generateCommand(const std::string &executable,const std::list<std::string> &params) const
{
  if (useResponseFile())
  {
    std::string response_file=responseFileName();

    FILE*f=fopen(response_file.c_str(),"w");
    if (f)
    {
      bool insert_space=false;
      bool last_was_empty=true;
      for (std::list<std::string>::const_iterator it=params.begin();it!=params.end();++it)
      {
        if (it->empty())
        {
          if (!last_was_empty)
            if (insert_space)
              fputs(" ",f);
            else
              fputs("\n",f);
          last_was_empty=true;
        }
        else
        {
          fputs(System::quoteArgument(*it).c_str(),f);
          last_was_empty=false;
          char first_char=(*it)[0];
          insert_space = ( first_char=='/' || first_char=='-' ) ;
        }
      }
      fclose(f);
      std::string command=executable;
      command+=" ";
      command+=response_file_str->value();
      if (response_file_append->value()==CompilerWrapperEnumOption::OPT_NO)
        command+=" ";
      command+=System::quoteArgument(response_file);
      return command;
    }
  }

  return generateCommandWithoutResponseFile(executable,params);
}

const std::string &CompilerWrapper::responseFileName() const
{
  if (useResponseFile())
  {
    if (_response_file_name.empty())
    {
      _response_file_name="__cs_response_$ID$";
      if ((*table_response_file_ext)[0])
        _response_file_name+=(*table_response_file_ext)[0];
      else
        _response_file_name+=".tmp";
      _response_file_name=TmpFile::object().generate(_response_file_name,true);
    }
  }
  else
    _response_file_name.clear();
  return _response_file_name;
}

int CompilerWrapper::callNativeTool()
{
  Argv arg(option.param_args());
  std::string file=CompilerFactory::findNativeTool(option.profileToolName(),arg);
  std::string execfile=System::quoteArgument(file);
  std::list<std::string> params;
  for (int i=1;i<arg.argc();i++)
  {
    params.push_back(arg[i]);
    params.push_back(std::string());
  }
  std::string command=generateCommand(execfile,params);
  DEBUG2("Executing native tool:%s\n",file.c_str());
  return System::exec(command.c_str(),NULL);
}

bool CompilerWrapper::useResponseFile() const 
{
  if ( ! (use_response_file->value()==CompilerWrapperEnumOption::OPT_YES && option.responseFileUsed()) )
    return false;

  int lg=option.param_args().argc()-1;
  for (int i=0;i<option.param_args().argc();i++)
    lg+=strlen(option.param_args()[i]);
  return lg>minimum_command_line_size_for_response_file->value();
}
