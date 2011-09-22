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

#ifndef SOURCE_H
#define SOURCE_H
#include "coveragescannerpdef.h"
#include "expression.h"
#include "function.h"
#include "compilerinterface.h"
class Expressions;
class FileName;

class Source
{
public:
    Source();
    virtual ~Source();

    void instrument(Expression::code_style_t code_style,const char *module,const Expressions &, Source &,const CompilerInterface &) const;
    const char* append(const char*);
    std::string source_code() const;
    void save(const char*);
    void savemeasures(Expression::code_style_t code_style,const char *module_name,const FileName *filenames_p,const Expressions *, unsigned long signature,CSMESFile &f) const;
    void clear();
    static void instrumentation_table(const char *module,const char *module_rel,char *table_name);
    static std::string csmes_c_filename(const char *source);
    static std::string csmes_filename(const char *source);
#if LOG
    void PDEBUG(const char *title) const;
#endif
    static void removeEscape(char *);
    static void removeEscape(std::string &);
    friend class Function;
    inline void append(char c);
    std::string extract_substring(long start_line,long start_column, long end_line, long end_column) const;
private:
    std::vector<CSMESFile::instrumentation_part_t> instrumentationParts(const Expression &) const ;
    void generate_source_from_pool(char **_source,long *_source_lg) const;
    void operator=(const Source &src);
    mutable char * source;
    mutable long *line_index;
    mutable long nb_line_index;
    mutable long source_lg;
    void _extract_substring(long start_line,long start_column, long end_line, long end_column,char **str_p) const;
    static void instrumentation_table_encode(char ci,char res[4]);

    char csmes_buffer[CHAINE_LEN];
    int  nb_csmes_buffer;
    void save_all_sources(Expression::code_style_t code_style,CSMESFile &f,const char *module_name,const Expressions *, unsigned long signature) const;
    
    struct source_pool_t
    {
      int allocated;
      int lg;
      int pos;
      char *data;
    };
    source_pool_t *source_pool;
    int nb_source_pool;
    const char *source_pool_insert(const char *str);
    void source_pool_insert_char(char c);
    void source_pool_clear();
    void instrumentation_header(Expression::code_style_t code_style,const char *module,const Expressions &expressions,Source &result,const CompilerInterface &compiler_wrapper) const;
    void instrumentation_footer(Expression::code_style_t code_style,const char *module,const Expressions &expressions,Source &result,const CompilerInterface &compiler_wrapper) const;
    void instrumentation_header_cs(const char *module,const Expressions &expressions,Source &result,const CompilerInterface &compiler_wrapper) const;
    void instrumentation_footer_cplusplus(const char *module,const Expressions &expressions,Source &result,const CompilerInterface &compiler_wrapper) const;
    void instrumentation_footer_cs(const char *module,const Expressions &expressions,Source &result,const CompilerInterface &compiler_wrapper) const;
    void instrumentation_header_cplusplus(const char *module,const Expressions &expressions,Source &result,const CompilerInterface &compiler_wrapper) const;
};


void Source::append(char c)
{
  source_pool_insert_char(c);
}

#endif
