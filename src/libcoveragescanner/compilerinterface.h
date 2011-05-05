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

#ifndef COMPILER_INTERFACE_H
#define COMPILER_INTERFACE_H
#include "expression.h"
#include "argv.h"
#include "defines.h"
class FileName;
class Option;

class CompilerInterface
{
public:
    enum type_t { COMPILER , LINKER, UNDEFINED, COMPILER_LINKER } ;
    enum extension_t { EXT_IGNORE, EXT_SOURCE_CPP, EXT_SOURCE_C , EXT_SOURCE_CS , EXT_UNKNOWN, EXT_OBJECT, EXT_STATIC_LIB, EXT_DYN_LIB , EXT_FILE_PDB, EXT_FILE_SBR } ;
public:

    CompilerInterface(const Option &);
    virtual ~CompilerInterface();
    bool isExitFunction(const char *) const ;

    /*! generate the compiler command line */
    virtual bool compiler(char **command/*!< command line (allocated by this function) */, bool hide_arguments) const =0;

    virtual bool usePreprocessor() const =0;
    virtual bool useCompilerToGenerateObjects() const=0;
public:
    virtual const char* CoverageScannerDefineOptionInArguments() const=0;
    virtual void RemoveCoverageScannerDefine(Argv &args) const=0;
    virtual void setArgsStdoutMriScript(const Argv &)=0;
    virtual bool responseFileContainsToArgumentList (const std::string &text,Argv &args) const;
	virtual std::string preprocessor_temp_filename(const std::string &) const=0;
    virtual bool preprocessor_use_temp_filename() const=0;
    virtual type_t type() const=0;
    virtual bool preprocessor(const char* filename,char **command) const=0;
    /*! generate the linker command line */
    bool sources(FileName *filenames) const ;
    virtual bool objects(FileName *filenames) const ;
    virtual bool libraries(FileName *filenames) const =0;
    virtual bool  destination(char **dest,bool &in_cmd_line) const =0;
    virtual bool compile_file(char **command_p,const char *source,const char *destination) const=0;
    virtual bool append_cslibrary(char **new_command_p,const char *compiler_command,const char *libname) const=0;
    virtual std::string csmes_library_object_name() const =0;
    bool isSource(const char *) const ;
    bool isObject(const char *) const ;
    bool isLibrary(const char *filename) const;
    virtual void display_temporary_files() const =0;
    virtual bool generateCoverageScannerLibrary() const=0 ;
    virtual Expression::code_style_t codeStyle() const=0;
    virtual const char *const * dyn_lib_ext() const =0;
    virtual const char *const * static_lib_ext() const =0;
    virtual const char *const * obj_ext() const=0;
    virtual const char *const * sbr_ext() const=0;
    virtual const char *const * pdb_ext() const=0;
    virtual const char *const * cpp_ext() const=0;
    virtual const char *const * c_ext() const=0;
    virtual const char *const * cs_ext() const=0;
    virtual const char *const * ignore_ext() const=0;
    virtual const char *const * exit_funct_ext() const=0;
    virtual const char* responseFile(int index,int &arg_index_min,int &arg_index_max) const=0;
    virtual bool responseFileToArgumentList (const std::string&rsp_file,Argv &args) const;
    virtual const char* additionalArgumentSeparator() const=0;
    bool restrictArgs(const char* selected_source,Argv &backup_args);
    virtual void setArgs(const Argv &);
    virtual const char *function_attribute() const=0;
    virtual const char *dll_export() const=0;
    virtual const char *dll_import() const=0;
    virtual bool customSetup() const=0;
    virtual bool isDestination(const char*argument) const=0;
    virtual bool deactivateCoverageScanner(const Argv &) const=0;
    virtual bool setupCpp() const=0;
    virtual bool setupPOSIX() const=0;
    virtual bool setupMSCE() const=0;
    virtual bool setupMS() const=0;
    virtual bool setupGNU() const=0;
    virtual bool enableCoveragescannerPerDefault() const =0 ;
    virtual const char* toolName() const=0;
    const Argv &  param_args() const { return args; } 
    virtual bool stdinMriScript(const Argv & ) const=0;
    extension_t extension(const char *) const ;
    bool ignoreSource(const char *s) const { return extension(s)==EXT_IGNORE; }
    static void disableCoverageScannerPerEnvironmentVariable();
    static bool isCoverageScannerDisabledPerEnvironmentVariable();
    virtual Defines defineFromCommandLine() const=0;
    virtual const char *errorFormat() const =0;
    virtual bool forceDllExport() const=0;
    virtual bool defineMatchCoverageOn(const char *) const =0;
    virtual bool defineMatchCoverageCount(const char *) const =0;
    virtual bool defineMatchCoverageHit(const char *) const =0;
    virtual bool defineMatchCoverageBranch(const char *) const =0;
    virtual bool defineMatchCoverageDecition(const char *) const =0;
    virtual bool defineMatchCoverageMDCS(const char *) const =0;
    virtual bool defineMatchCoverageCondition(const char *) const =0;
    virtual bool defineMatchCoverageNoLockCsexe(const char *) const =0;
    virtual bool defineMatchCoverageLockCsexe(const char *) const =0;
    virtual bool defineMatchCoveragePartialInstrumentation(const char *) const =0;
    virtual bool defineMatchCoverageFullInstrumentation(const char *) const =0;
    virtual bool defineMatchCoverageNoExceptions(const char *) const =0;
    virtual bool defineMatchCoverageNoAssignment(const char *) const =0;
    virtual int callNativeTool()=0;
private:
    void init();
    Argv args,args_org;
    virtual void setArgs(const Argv &,Argv &);
protected:
    enum language_t
    {
      LANGUAGE_AUTO,
      LANGUAGE_C,
      LANGUAGE_CPP
    };

    const Argv &  param_args_org() const { return args_org; } 
    /*! generate the compiler command line */
    char preprocessorfile[CHAINE_LEN];
    char *tmpFile();
    const Option *options_p;
    void setLanguage(language_t l) ;
private:
    language_t _language;
};

#endif
