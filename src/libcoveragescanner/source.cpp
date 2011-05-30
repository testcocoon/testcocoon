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
#include "libgen.h"
#include "source.h"
#include "tmpfile.h"
#include "filtersource.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <unistd.h>
#include "expressions.h"
#include "expression.h"
#include "parser.h"
#include "parsemisc.h"
#include "filename.h"
#include "csmesfile.h"
#include "fileinfo.h"
#include "crc.h"
#define SOURCE_POOL_ALLOC_SIZE 100000

static FileName filenames;

/*!
 * \param size the parameter is needed for preallocation of the memory.
 *             If the prealloc  is done correctly, the pointer
 *             to some text entries can be used
 */
Source::Source()
{
  FUNCTION_TRACE;
  nb_source_pool=0;
  source_pool=NULL;
  source=NULL;
  line_index=NULL;
  clear();
}

Source::~Source()
{
  FUNCTION_TRACE;
  FREE (source);
  FREE(line_index);
  source_pool_clear();
}

void Source::source_pool_clear()
{
  FUNCTION_TRACE;
  for (int i=0;i<nb_source_pool;i++)
    FREE(source_pool[i].data);
  nb_source_pool=0;
  FREE(source_pool);
  source_pool=NULL;
}

void Source::clear()
{
  FUNCTION_TRACE;
  source_pool_clear();
  source_pool_insert("");
  FREE(source);
  source=NULL;
  source_lg = 0;
  FREE(line_index);
  nb_line_index=0;
}

std::string Source::extract_substring(long start_line,long start_column, long end_line, long end_column) const
{
  char *str_p=NULL;
  _extract_substring(start_line,start_column, end_line, end_column, &str_p);
  std::string str;
  if (str_p)
  {
    str=str_p;
    FREE(str_p);
  }
  return str;
}

/*! \brief extract string from the source
 *
 * \note the string is allocated, the deallocation must be performed
 * by the caller
 */
void Source::_extract_substring(long start_line,long start_column, long end_line, long end_column,char **str_p) const
{
  FUNCTION_TRACE;
  if (nb_line_index==0)
  {
    if (source==NULL) generate_source_from_pool(&source,&source_lg);
    generate_line_index(source,source_lg,&line_index,&nb_line_index);
  }
  extract_string(source,source_lg,line_index,nb_line_index,start_line,start_column,end_line,end_column,str_p);
}

/*!
 * \retval pointer to the text element
 * \note   The pointer if only valid if a reallocation of the size is not needed
 */
const char* Source::append(const char *text)
{
  FUNCTION_TRACE;
  return source_pool_insert(text);;
}

void Source::save(const char *filename)
{
  FUNCTION_TRACE;
  long i;
  FILE *f;

  if (filename==NULL)
    f=stdout;
  else
    f=fopen(filename,"w");

  if (f==NULL)
  {
    FATAL2("Could not open %s with write access\n",filename);
  }
  char buffer[CHAINE_LEN*10];
  int nb_buffer=0;

  for (int ipool=0;ipool<nb_source_pool;ipool++)
  {
    long _source_lg=source_pool[ipool].lg;
    const char* _source=source_pool[ipool].data;
    for (i=0;i<_source_lg;i++)
    {
      if (_source[i]=='\0')
        continue;

      if (nb_buffer==sizeof(buffer))
      {
        fwrite(buffer,1,nb_buffer,f);
        nb_buffer=0;
      }
      buffer[nb_buffer++]=_source[i];
    }
  }
  if (nb_buffer!=0)
    fwrite(buffer,1,nb_buffer,f);
  if (filename!=NULL)
    fclose(f);
}

#if LOG
void Source::PDEBUG(const char * title) const
{
  FUNCTION_TRACE;
  if (title)
  {
    DEBUG2("%c",'\n');
    DEBUG2("===== begin %s ====",title);
    DEBUG2("%c",'\n');
  }
  if (source==NULL) generate_source_from_pool(&source,&source_lg);
  long i;
  long line=1;
  for (i=0;i<source_lg;i++)
  {
    if (source[i]=='\0')
      continue;
    DEBUG2("%c",source[i]);
    if (source[i]=='\n')
    {
      DEBUG2("%4li:",line++);
    }
  }
  if (title)
  {
    DEBUG2("%c",'\n');
    DEBUG2("===== end %s ====",title);
    DEBUG2("%c",'\n');
  }
}
#endif

void Source::removeEscape(std::string &s)
{
  FUNCTION_TRACE;
  char * ss=STRDUP(s.c_str());
  removeEscape(ss);
  s=std::string(ss);
  FREE(ss);
}

void Source::removeEscape(char *s)
{
  FUNCTION_TRACE
  int j=0;
  for (int i=0;s[i]!='\0';i++,j++)
  {
    s[j]=s[i];
    if (s[i]=='\\' && s[i+1]=='\\')
      i++;
  }
  s[j]='\0';
}

void Source::save_all_sources(Expression::code_style_t code_style,CSMESFile &f,const char *module_name,const Expressions *expressions_p, unsigned long signature) const
{
  FUNCTION_TRACE;
  bool include_preprocessed_source=false;
  bool include_original_source=false;
  switch (code_style)
  {
    case Expression::CODESTYLE_CPP:
      include_preprocessed_source=true;
      include_original_source=false;
      break;
    case Expression::CODESTYLE_CS:
      include_preprocessed_source=false;
      include_original_source=true;
      break;
    default:
      FATAL1("Internal error");
  }
  DEBUG2("Begin saving all sources, module_name='%s'\n",module_name);
  int i;
  char last_char='\n';
  char current_filename[CHAINE_LEN];
  strcpy(current_filename,module_name);
  struct source_t
  {
    char * filename;
    char *source;
    int source_lg;
  } ;
  source_t *source_tab=(source_t*)MALLOC(sizeof(source_t)*0x1003);
  int nb_source_tab=1;
  source_tab[0].source_lg=0;
  source_tab[0].source=NULL;
  source_tab[0].filename=STRDUP(current_filename);
  int current_file_id=0;

  if (include_preprocessed_source)
  {
    DEBUG1("Begin analyzing preprocessed source code\n");
    for (i=0;i<source_lg;i++)
    {
      if (source[i]=='\0')
        continue;
      bool process_source_line=true;
      if ( source[i]=='#' && ((last_char=='\n') || (last_char=='\r')) )
      { /* line information */
#if LOG
        DEBUG1("Begin analyzing line information:");
        for (int ii=i;ii<source_lg && source[ii]!='\n';ii++)
        {
          if (source[ii]!='\0')
          {
            DEBUG2("%c",source[ii]);
          }
        }
        DEBUG1("\n");
#endif

        long j,k,l;
        char line_str[CHAINE_LEN];
        long extracted_line=-1;
        int pos=0;
        int iOffset=1;
        for (;source[i+iOffset]=='\0';iOffset++) ;
        if (source[i+iOffset]==' ' || source[i+iOffset]=='\t' || strncmp(&source[i+iOffset],"line",4)==0)
        {
          j=i+iOffset;
          while ( ! (source[j]==' ' || source[j]=='\t') )
            j++;

          for (; j<source_lg && source[j]!='\n' && source[j]!='"';j++)
            if (source[j]!=' ' && source[j]!='\t')
              line_str[pos++]=source[j];
          line_str[pos]='\0';
          extracted_line=strtol(line_str,NULL,10);
          DEBUG2("Line number=%i\n",extracted_line);
          if (extracted_line!=LONG_MAX && extracted_line!=LONG_MIN)
          {
            for (k=j+1;  k<source_lg &&  (source[k]!='"');k++)
              ;
            for (l=0;l<k-j-1;l++)
              current_filename[l] = source[j+1+l] ;
            current_filename[k-j-1]=0;

            removeEscape(current_filename);
            current_file_id=-1;
            for (int isrc=0;isrc<nb_source_tab && current_file_id==-1;isrc++)
            {
              DEBUG2("current_filename='%s'\n",current_filename);
              DEBUG3("source_tab[%i].filename='%s'\n",isrc,source_tab[isrc].filename);
              if (strcmp(current_filename,source_tab[isrc].filename)==0)
                current_file_id=isrc;
            }
            if (current_file_id==-1)
            {
              if ((nb_source_tab&0xFFF)==0)
                source_tab=(source_t*)REALLOC(source_tab,sizeof(source_t)*(0xFFF+2+nb_source_tab));
              current_file_id=nb_source_tab;
              nb_source_tab++;
              source_tab[current_file_id].filename=STRDUP(current_filename);
              source_tab[current_file_id].source=NULL;
              source_tab[current_file_id].source_lg=0;
            }
          }
          else
          {
            DEBUG1 ("extracted_line==LONG_MAX || extracted_line==LONG_MIN:ignoring preprocessor line directive");
          }
          process_source_line=false;
        }
        if (!process_source_line)
        {
          /* skip the line */
          for (j=i; j<source_lg &&  (source[j]!='\n');j++)
            ;
          i=j;
          DEBUG3("End analyzing line information, current_file_id=%i, filename='%s'\n",current_file_id,source_tab[current_file_id].filename);
        }
      }
      if (process_source_line)
      {
        if ((source_tab[current_file_id].source_lg&0xFFF)==0)
          source_tab[current_file_id].source=(char*)REALLOC(source_tab[current_file_id].source,sizeof(char)*(0xFFF+2+source_tab[current_file_id].source_lg));
        source_tab[current_file_id].source[source_tab[current_file_id].source_lg++]=source[i];
      }
      if (source[i]!=' ' && source[i]!='\t')
        last_char=source[i];
    }
    DEBUG1("End analyzing preprocessed source code\n");
  }

  for (i=0;i<nb_source_tab;i++)
  {
    const char*file=source_tab[i].filename;
    DEBUG3("source_tab[%i].source_lg=%i\n",i,source_tab[i].source_lg);
    DEBUG3("source_tab[%i].filename='%s'\n",i,source_tab[i].filename);
    if (expressions_p->instrument_exists(file))
    {
      char module_abs[MAX_PATH],file_abs[MAX_PATH];
      realPath(module_name,module_abs);
      realPath(file,file_abs);
      if (include_original_source)
      {
        DEBUG2("Begin recording source code, file='%s'\n",file);
        FILE *original_file;
        if ( (original_file=fopen(file,"r"))!=NULL)
        {
          char str[CHAINE_LEN];
          f.openSection(module_abs,module_name,file_abs,file,"",CSMESFile::ORIGINAL,signature,CSMESFile::NEW_RW,0);
          int nb_read=0;
          while ( (nb_read=fread(str,1,CHAINE_LEN,original_file)) > 0 )
          {
            f.writeCharTab(str,nb_read);
          }
          f.closeSection();
          fclose(original_file);
        }
        DEBUG2("End recording source code, file='%s'\n",file);
      }

      if (include_preprocessed_source)
      {
        DEBUG2("Begin recording preprocessed  source code, file='%s'\n",file);
        f.openSection(module_abs,module_name,file_abs,file,"",CSMESFile::PREPROCESSED,signature,CSMESFile::NEW_RW,0);
        f.writeCharTab(source_tab[i].source,source_tab[i].source_lg);
        f.closeSection();
        DEBUG2("End recording preprocessed  source code, file='%s'\n",file);
      }
    }
  }

  for (i=0;i<nb_source_tab;i++)
  {
    FREE(source_tab[i].filename);
    FREE(source_tab[i].source);
  }
  FREE(source_tab);

  DEBUG2("End saving all sources, module_name='%s'\n",module_name);
}

void Source::savemeasures(Expression::code_style_t code_style,const char *module_name,const FileName *filenames_p,const Expressions *expressions_p,unsigned long signature,CSMESFile &f) const
{
  FUNCTION_TRACE
  char signature_str[CHAINE_LEN];
  long i;
  long file_id;
  long file_count;
  const char *file;
  DEBUG2("Begin saving measurements instrumentation, module_name='%s'\n",module_name);
  if (source==NULL) generate_source_from_pool(&source,&source_lg);
  sprintf(signature_str,"%lu",signature);

  save_all_sources(code_style,f,module_name,expressions_p,signature);

  char module_abs[MAX_PATH],file_abs[MAX_PATH];
  realPath(module_name,module_abs);

  file_count =  filenames_p->count();
  for (file_id=0;file_id<file_count;file_id++)
  {
    file = filenames_p->at(file_id);
    realPath(file,file_abs);

    if (expressions_p->instrument_exists(file))
    {
      DEBUG3("Begin saving instrumentation, module_abs='%s', file_abs='%s'\n",module_abs,file_abs);
      f.openSection(module_abs,module_name,file_abs,file,"",CSMESFile::INSTRUMENTATION,signature,CSMESFile::NEW_RW,0);
      int nb_total_instrumentation= expressions_p->count();
      for (i=0;i<nb_total_instrumentation;i++)
      {
        const Expression *expr_p=&expressions_p->at(i);
        DEBUG2("Process expression %i\n",i);
        if (expr_p->is_filename(file))
        {
          if (expr_p->indexes()>0)
          {
            CSMESFile::instrumentation_t instrumentationType                    = expr_p->instrumentationType();
            std::vector<CSMESFile::instrumentation_part_t> instrumentation_part = instrumentationParts(*expr_p);
            CSMESFile::instrumentation_childrens_t childrens                    = expr_p->childrensCSMESFile();
            long index                                                          = expr_p->startIndex();
            long startLineOrg                                                   = expr_p->startLineOrg();
            long startLinePre                                                   = expr_p->startLinePre();
            long startColumnPre                                                 = expr_p->startColumnPre();
            long startColumnOrg                                                 = expr_p->startColumnOrg();
            long endLineOrg                                                     = expr_p->endLineOrg();
            long endLinePre                                                     = expr_p->endLinePre();
            long endColumnOrg                                                   = expr_p->endColumnOrg();
            long endColumnPre                                                   = expr_p->endColumnPre();
            DEBUG4("Record expression %i: source:%s line=%i\n",i,file,startLineOrg);

            f.writeInstrumentation(reinterpret_cast<_I64>(expr_p),index, instrumentationType, startLineOrg, startColumnOrg, startLinePre, startColumnPre, endLineOrg, endColumnOrg, endLinePre, endColumnPre, instrumentation_part,childrens);
          }
        }
      }
      f.closeSection();
      DEBUG3("End saving instrumentation, module_abs='%s', file_abs='%s'\n",module_abs,file_abs);
    }
  }
  DEBUG2("End saving measurements instrumentation, module_name='%s'\n",module_name);
}


void Source::instrumentation_header(Expression::code_style_t code_style,const char *module,const Expressions &expressions,Source &result,const CompilerInterface &compiler_wrapper) const
{
  FUNCTION_TRACE
  switch (code_style)
  {
    case Expression::CODESTYLE_CPP:
      instrumentation_header_cplusplus(module,expressions,result,compiler_wrapper);
      break;
    case Expression::CODESTYLE_CS:
      instrumentation_header_cs(module,expressions,result,compiler_wrapper);
      break;
    default:
      FATAL1("Internal error");
  }
}

void Source::instrumentation_header_cs(const char *module,const Expressions & /*expressions_p*/,Source &result,const CompilerInterface &/*compiler_wrapper*/) const
{
  FUNCTION_TRACE
  result.append("#line hidden\n");
  result.append("#define __COVERAGESCANNER__\n");
  result.append("#line 1 \"");
  result.append(module);
  result.append("\"\n");
}

void Source::instrumentation_footer_cs(const char *module,const Expressions &expressions,Source &result,const CompilerInterface &/*compiler_wrapper*/) const
{
  FUNCTION_TRACE
  char tmp[CHAINE_LEN] ;
  char table_name[INSTRUMENTATION_CODE_MAX_LENGTH] ;
  char module_abs[MAX_PATH];

  realPath(module,module_abs);
  instrumentation_table(module_abs,table_name);

  result.append("\n#line hidden\n");
  result.append("#pragma warning disable\n");

  DEBUG2("SOURCE:Module %s\n",module);

  int nb_indexes=expressions.indexes();

    sprintf(tmp,"public class %s {\n",table_name);
  result.append(tmp);
  result.append( "static public bool SetFalse(int index) { val[index]=1 ; return false; }\n");
  result.append( "static public bool SetTrue(int index) { val[index]=1 ; return true; }\n");
  result.append( "static public bool IncrementFalse(int index) { val[index]++ ; return false; }\n");
  result.append( "static public bool IncrementTrue(int index) { val[index]++ ; return true; }\n");
  if (nb_indexes!=0)
    sprintf(tmp,"static public int[] val=new int[%i];\n",nb_indexes);
  else
    sprintf(tmp,"static public int[] val=null;\n");
  result.append(tmp);
  sprintf(tmp,"static public int nb=%i;\n",nb_indexes);
  result.append(tmp);
  result.append("}\n");
}

void Source::instrumentation_header_cplusplus(const char *module,const Expressions &expressions,Source &result,const CompilerInterface &compiler_wrapper) const
{
  FUNCTION_TRACE
  char tmp[CHAINE_LEN] ;
  char table_name[INSTRUMENTATION_CODE_MAX_LENGTH] ;
  char module_abs[MAX_PATH];

  realPath(module,module_abs);
  instrumentation_table(module_abs,table_name);

  result.append("#line 1 \"coveragescanner\"\n");
  result.append("#ifdef __cplusplus\n");
  sprintf(tmp,"extern \"C\" { \n");
  result.append(tmp);
  result.append("#endif\n");

  DEBUG2("SOURCE:Module %s\n",module);

  int nb_indexes=expressions.indexes();

  if (nb_indexes!=0)
    sprintf(tmp,"static volatile int %s[%i];\n",table_name,nb_indexes);
  else
    sprintf(tmp,"static int *%s=0;\n",table_name);
  result.append(tmp);
  sprintf(tmp,"%s void %s_(int *size_p,volatile int **table_p) %s;\n",compiler_wrapper.dll_export(),table_name,compiler_wrapper.function_attribute());
  result.append(tmp);
  sprintf(tmp,"void %s_(int *size_p,volatile int **table_p) ",table_name);
  result.append(tmp);
  sprintf(tmp,"{ *table_p=%s; *size_p=%i; }\n",table_name,nb_indexes);
  result.append(tmp);

  if (!compiler_wrapper.customSetup())
  {
    result.append("void ");
    result.append(" __coveragescanner_install(const char *) ");
    result.append(compiler_wrapper.function_attribute());
    result.append(" ;\n");
  }
  result.append("void ");
  result.append(" __coveragescanner_set_custom_io(");
  result.append("char *(*cs_fgets)(char *s, int size, void *stream),");
  result.append("int (*cs_fputs)(const char *s, void *stream),");
  result.append("void *(*cs_fopenappend)(const char *path),");
  result.append("void *(*cs_fopenread)(const char *path),");
  result.append("void *(*cs_fopenwrite)(const char *path),");
  result.append("int (*cs_fclose)(void *fp),");
  result.append("int (*cs_remove)(const char *name)");
  result.append(") ");
  result.append(compiler_wrapper.function_attribute());
  result.append(" ;\n");
  result.append("void ");
  result.append(" __coveragescanner_filename(const char *) ");
  result.append(compiler_wrapper.function_attribute());
  result.append(" ;\n");
  result.append("void ");
  result.append(" __coveragescanner_clear(void) ");
  result.append(" ;\n");
  result.append(compiler_wrapper.function_attribute());
  result.append("void ");
  result.append(" __coveragescanner_teststate(const char *) ");
  result.append(compiler_wrapper.function_attribute());
  result.append(" ;\n");
  result.append("void ");
  result.append(" __coveragescanner_testname(const char *) ");
  result.append(compiler_wrapper.function_attribute());
  result.append(" ;\n");
  result.append("void ");
  result.append(" __coveragescanner_save(void) ");
  result.append(compiler_wrapper.function_attribute());
  result.append(" ;\n");

  result.append("#ifdef __cplusplus\n");
  result.append("}\n");
  result.append("#endif\n");
}

void Source::instrumentation_footer_cplusplus(const char * /*module*/,const Expressions & /*expressions_p*/,Source & /*result_p*/,const CompilerInterface & /*compiler_wrapper*/) const
{
}

void Source::instrumentation_footer(Expression::code_style_t code_style,const char *module,const Expressions &expressions,Source &result,const CompilerInterface &compiler_wrapper) const
{
  FUNCTION_TRACE
  switch (code_style)
  {
    case Expression::CODESTYLE_CPP:
      instrumentation_footer_cplusplus(module,expressions,result,compiler_wrapper);
      break;
    case Expression::CODESTYLE_CS:
      instrumentation_footer_cs(module,expressions,result,compiler_wrapper);
      break;
    default:
      FATAL1("Internal error");
  }
}

void Source::instrument(Expression::code_style_t code_style,const char *module,const Expressions &expressions,Source &result,const CompilerInterface &compiler_wrapper) const
{
  FUNCTION_TRACE;
  if (source==NULL) generate_source_from_pool(&source,&source_lg);
  std::list<const Patch *>::const_iterator itPatch=expressions.patches().begin();
  long i;
  long line=1;
  long column=1;
  char table_name[INSTRUMENTATION_CODE_MAX_LENGTH] ;
  char module_abs[MAX_PATH];
  long next_line=-1;
  long next_column=-1;

  instrumentation_header(code_style,module,expressions,result,compiler_wrapper);

  realPath(module,module_abs);
  instrumentation_table(module_abs,table_name);

  if (itPatch!=expressions.patches().end())
  {
    next_line=(*itPatch)->line();
    next_column=(*itPatch)->column();
  }

  DEBUG1("begin source code instrumentation\n");
  for (i=0;i<source_lg;i++)
  {
    if (source[i]=='\0')
      continue;
    while (line==next_line && column==next_column)
    {
      std::string patch_code=(*itPatch)->code(table_name);
      DEBUG6("instrument %s index=%li line=%li column=%li depth=%li\n",(*itPatch)->name().c_str(),(*itPatch)->offset(),line,column,(*itPatch)->depth());
      result.append(patch_code.c_str());
      itPatch++;
      if (itPatch!=expressions.patches().end())
      {
        next_line=(*itPatch)->line();
        next_column=(*itPatch)->column();
      }
      else
      {
        next_line=-1;
        next_column=-1;
      }
    }

    result.append(source[i]);

    if (source[i]=='\n')
    {
      column=0;
      line++;
    }
    column++;
  }
  while (line==next_line && column==next_column)
  {
    std::string patch_code=(*itPatch)->code(table_name);
    DEBUG6("instrument %s index=%li line=%li column=%li depth=%li\n",(*itPatch)->name().c_str(),(*itPatch)->offset(),line,column,(*itPatch)->depth());
    result.append(patch_code.c_str());
    itPatch++;
    if (itPatch!=expressions.patches().end())
    {
      next_line=(*itPatch)->line();
      next_column=(*itPatch)->column();
    }
    else
    {
      next_line=-1;
      next_column=-1;
    }
  }

  DEBUG1("end source code instrumentation\n");
  instrumentation_footer(code_style,module,expressions,result,compiler_wrapper);
}

void  Source::instrumentation_table(const char *module,char *table_name)
{
    long i,lg;
    long offset;
    char enc_tmp[INSTRUMENTATION_CODE_MAX_LENGTH];

    offset=strlen(INSTRUMENTATION_TABLE_STR);
    if (module!=NULL)
    {
        lg = strlen(module);
        unsigned long crc=tm_crc32((const unsigned char*)(module),lg);
        strcpy(table_name,INSTRUMENTATION_TABLE_STR);
        for (i=0;i<8;i++)
          table_name[i+offset]='a'+ static_cast<char>((crc>>(4*i))&0xF);
        table_name[offset+8]='_';
        offset+=9;
        table_name[offset]='\0';

        for (i=lg-1;i>=0;i--)
        {
            instrumentation_table_encode(module[i],enc_tmp);
            if (strlen(table_name)+strlen(enc_tmp)>=INSTRUMENTATION_CODE_MAX_LENGTH-1)
                break;
            strcat(table_name,enc_tmp);
        }
    }
    else
        strncpy(table_name,INSTRUMENTATION_TABLE_STR,INSTRUMENTATION_CODE_MAX_LENGTH);
}

void  Source::instrumentation_table_encode(char c,char res[4])
{
  FUNCTION_TRACE
    unsigned char  r;
    if (   ( c>='a' && c<='z' )
            || ( c>='A' && c<='Z' )
            || ( c>='0' && c<='9' ) )
    {
        res[0]=c;
        res[1]=0;
        return ;
    }
    res[0]='_';
    r=(0xF0 & c) >> 4;
    if (r<10)
        res[1]='0'+r;
    else
        res[1]='A'+r-10;
    r=(0xF & c);
    if (r<10)
        res[2]='0'+r;
    else
        res[2]='A'+r-10;
    res[3]=0;

}

std::string Source::csmes_c_filename(const char *source)
{
  FUNCTION_TRACE
  long i;
  long lg = strlen(source);
  std::string instrumentation_name=std::string(source);
  instrumentation_name+=".csmes_$ID$.";

  for (i=lg-1;i>=0;i--)
  {
    if (source[i]=='.')
      break;
  }
  instrumentation_name+=std::string(&source[i+1]);

  return TmpFile::object().generate(instrumentation_name,true);
}

std::string Source::csmes_filename(const char *source)
{
  FUNCTION_TRACE
  std::string instrumentation_name=std::string(source);
#if defined(OS_WIN32)
  int len=instrumentation_name.length();
  if (len>1)
  {
    if (instrumentation_name[0]=='\"')
      instrumentation_name=instrumentation_name.substr(1);
    len=instrumentation_name.length();
    if (instrumentation_name[len-1]=='\"')
      instrumentation_name=instrumentation_name.substr(0,len-1);
  }
#endif
  instrumentation_name+=".csmes";
  return instrumentation_name;
}

const char *Source::source_pool_insert(const char *str)
{
  FUNCTION_TRACE
  int str_lg=strlen(str)+1;
  if (nb_source_pool==0 || (str_lg+source_pool[nb_source_pool-1].lg>=source_pool[nb_source_pool-1].allocated))
  {
    DEBUG1("begin source pool allocation\n");
    nb_source_pool++;
    source_pool = (source_pool_t*) REALLOC(source_pool,sizeof(source_pool_t)*nb_source_pool);
    source_pool_t &_source_pool=source_pool[nb_source_pool-1];
    _source_pool.allocated=str_lg+SOURCE_POOL_ALLOC_SIZE;
    _source_pool.lg=0;
    _source_pool.data=(char*)MALLOC(_source_pool.allocated);
    if (nb_source_pool==1)
      _source_pool.pos=0;
    else
      _source_pool.pos=source_pool[nb_source_pool-2].pos+source_pool[nb_source_pool-2].lg;
    DEBUG4("end source pool allocation, index=%i, size=%i, pos=%i\n",nb_source_pool-1,_source_pool.lg,_source_pool.pos);
  }
  source_pool_t &_source_pool=source_pool[nb_source_pool-1];
  char *str_ret=&_source_pool.data[_source_pool.lg];
  memcpy(str_ret,str,str_lg);
  _source_pool.lg+=str_lg;
  ASSERT(strcmp(str_ret,str)==0);
  return str_ret;
}

void Source::source_pool_insert_char(char c)
{
  FUNCTION_TRACE
  char str[2];
  str[0]=c;
  str[1]='\0';
  source_pool_insert(str);
}

void Source::generate_source_from_pool(char **_source,long *_source_lg) const
{
  FUNCTION_TRACE
  FREE((*_source));
  (*_source)=NULL;
  (*_source_lg)=0;
  int ipool;
  for (ipool=0;ipool<nb_source_pool;ipool++)
    (*_source_lg)+=source_pool[ipool].lg;

  (*_source)=(char*)MALLOC((*_source_lg)+1);
  (*_source)[0]='\0';
  for (ipool=0;ipool<nb_source_pool;ipool++)
    memcpy(&(*_source)[source_pool[ipool].pos],source_pool[ipool].data,source_pool[ipool].lg);
}

std::vector<CSMESFile::instrumentation_part_t> Source::instrumentationParts(const Expression &expr) const
{
  std::vector<CSMESFile::instrumentation_part_t> instrumentation_parts;
  switch (expr.instrumentationType())
  {
    case CSMESFile::_INSTRUMENTATION_LABEL:
    case CSMESFile::_INSTRUMENTATION_COMPUTATION:
      {
        instrumentation_parts.resize(1);
        instrumentation_parts[0].description=("statement executed");
        instrumentation_parts[0].type=CSMESFile::PART_IS_EXECUTED;
        instrumentation_parts[0].items.resize(1);
        instrumentation_parts[0].items[0].s_line_org   = expr.startLineOrg();
        instrumentation_parts[0].items[0].s_column_org = expr.startColumnOrg();
        instrumentation_parts[0].items[0].e_line_org   = expr.endLineOrg();
        instrumentation_parts[0].items[0].e_column_org = expr.endColumnOrg();
        instrumentation_parts[0].items[0].s_line_pre   = expr.startLinePre();
        instrumentation_parts[0].items[0].s_column_pre = expr.startColumnPre();
        instrumentation_parts[0].items[0].e_line_pre   = expr.endLinePre();
        instrumentation_parts[0].items[0].e_column_pre = expr.endColumnPre();
        {
          char *sub=NULL;
          _extract_substring(expr.startLineAbsolute(),expr.startColumnPre(), expr.endLineAbsolute(),expr.endColumnPre(),&sub);
          if (sub)
            instrumentation_parts[0].items[0].description=std::string(sub);
          ASSERT(sub);
          FREE(sub);
        }
        instrumentation_parts[0].items[0].type=CSMESFile::PART_ITEM_IS_EXECUTED;
      }
      break;
    case CSMESFile::_INSTRUMENTATION_CONDITION_FULL:
      {
        instrumentation_parts.resize(2);

        instrumentation_parts[0].description=("expression was true");
        instrumentation_parts[0].type=CSMESFile::PART_IS_TRUE;
        instrumentation_parts[0].items.resize(1);
        instrumentation_parts[0].items[0].s_line_pre   = expr.startLinePre();
        instrumentation_parts[0].items[0].s_column_pre = expr.startColumnPre();
        instrumentation_parts[0].items[0].e_line_pre   = expr.endLinePre();
        instrumentation_parts[0].items[0].e_column_pre = expr.endColumnPre();
        instrumentation_parts[0].items[0].s_line_org   = expr.startLineOrg();
        instrumentation_parts[0].items[0].s_column_org = expr.startColumnOrg();
        instrumentation_parts[0].items[0].e_line_org   = expr.endLineOrg();
        instrumentation_parts[0].items[0].e_column_org = expr.endColumnOrg();
        {
          char *sub=NULL;
          _extract_substring(expr.startLineAbsolute(),expr.startColumnPre(), expr.endLineAbsolute(),expr.endColumnPre(),&sub);
          if (sub)
            instrumentation_parts[0].items[0].description=std::string(sub);
          ASSERT(sub);
          FREE(sub);
        }
        instrumentation_parts[0].items[0].type=CSMESFile::PART_ITEM_IS_TRUE;

        instrumentation_parts[1].description=("expression was false");
        instrumentation_parts[1].type=CSMESFile::PART_IS_FALSE;
        instrumentation_parts[1].items.resize(1);
        instrumentation_parts[1].items[0].s_line_pre   = expr.startLinePre();
        instrumentation_parts[1].items[0].s_column_pre = expr.startColumnPre();
        instrumentation_parts[1].items[0].e_line_pre   = expr.endLinePre();
        instrumentation_parts[1].items[0].e_column_pre = expr.endColumnPre();
        instrumentation_parts[1].items[0].s_line_org   = expr.startLineOrg();
        instrumentation_parts[1].items[0].s_column_org = expr.startColumnOrg();
        instrumentation_parts[1].items[0].e_line_org   = expr.endLineOrg();
        instrumentation_parts[1].items[0].e_column_org = expr.endColumnOrg();
        {
          char *sub=NULL;
          _extract_substring(expr.startLineAbsolute(),expr.startColumnPre(), expr.endLineAbsolute(),expr.endColumnPre(),&sub);
          if (sub)
            instrumentation_parts[1].items[0].description=std::string(sub);
          ASSERT(sub);
          FREE(sub);
        }
        instrumentation_parts[1].items[0].type=CSMESFile::PART_ITEM_IS_FALSE;
      }
      break;
    case CSMESFile::_INSTRUMENTATION_CONDITION_TRUE_ONLY:
      {
        instrumentation_parts.resize(1);
        instrumentation_parts[0].description=("expression was true");
        instrumentation_parts[0].type=CSMESFile::PART_IS_TRUE;
        instrumentation_parts[0].items.resize(1);
        instrumentation_parts[0].items[0].s_line_pre   = expr.startLinePre();
        instrumentation_parts[0].items[0].s_column_pre = expr.startColumnPre();
        instrumentation_parts[0].items[0].e_line_pre   = expr.endLinePre();
        instrumentation_parts[0].items[0].e_column_pre = expr.endColumnPre();
        instrumentation_parts[0].items[0].s_line_org   = expr.startLineOrg();
        instrumentation_parts[0].items[0].s_column_org = expr.startColumnOrg();
        instrumentation_parts[0].items[0].e_line_org   = expr.endLineOrg();
        instrumentation_parts[0].items[0].e_column_org = expr.endColumnOrg();
        {
          char *sub=NULL;
          _extract_substring(expr.startLineAbsolute(),expr.startColumnPre(), expr.endLineAbsolute(),expr.endColumnPre(),&sub);
          if (sub)
            instrumentation_parts[0].items[0].description=std::string(sub);
          ASSERT(sub);
          FREE(sub);
        }
        instrumentation_parts[0].items[0].type=CSMESFile::PART_ITEM_IS_TRUE;
      }
      break;
    case CSMESFile::_INSTRUMENTATION_CONDITION_FALSE_ONLY:
      {
        instrumentation_parts.resize(1);

        instrumentation_parts[0].description=("expression was false");
        instrumentation_parts[0].type=CSMESFile::PART_IS_FALSE;
        instrumentation_parts[0].items.resize(1);
        instrumentation_parts[0].items[0].s_line_pre   = expr.startLinePre();
        instrumentation_parts[0].items[0].s_column_pre = expr.startColumnPre();
        instrumentation_parts[0].items[0].e_line_pre   = expr.endLinePre();
        instrumentation_parts[0].items[0].e_column_pre = expr.endColumnPre();
        instrumentation_parts[0].items[0].s_line_org   = expr.startLineOrg();
        instrumentation_parts[0].items[0].s_column_org = expr.startColumnOrg();
        instrumentation_parts[0].items[0].e_line_org   = expr.endLineOrg();
        instrumentation_parts[0].items[0].e_column_org = expr.endColumnOrg();
        {
          char *sub=NULL;
          _extract_substring(expr.startLineAbsolute(),expr.startColumnPre(), expr.endLineAbsolute(),expr.endColumnPre(),&sub);
          if (sub)
            instrumentation_parts[0].items[0].description=std::string(sub);
          ASSERT(sub);
          FREE(sub);
        }
        instrumentation_parts[0].items[0].type=CSMESFile::PART_ITEM_IS_FALSE;
      }
      break;
  }
  return instrumentation_parts;
}

std::string Source::source_code() const
{
  FUNCTION_TRACE;
  std::string result;
  long i;


  for (int ipool=0;ipool<nb_source_pool;ipool++)
  {
    long _source_lg=source_pool[ipool].lg;
    const char* _source=source_pool[ipool].data;
    for (i=0;i<_source_lg;i++)
    {
      if (_source[i]!='\0')
         result+=_source[i];
    }
  }
  return result;
}
