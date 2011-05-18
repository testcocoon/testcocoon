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

#include "functions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "coveragescannerpdef.h"
#include "function.h"
#include "expressions.h"
#include "filename.h"
#include "source.h"
#include "../libcsmes/fileinfo.h"


#if LOG
void Functions::PDEBUG() const
{
  FUNCTION_TRACE;
  for (std::list<Function>::const_iterator iterator=list.begin();iterator!=list.end();++iterator)
    iterator->PDEBUG();
}
#endif

void Functions::save(const char *module_name,const FileName *filenames_p,const Expressions *expressions_p,unsigned long signature,CSMESFile &f)
{
  FUNCTION_TRACE;
  char signature_str[CHAINE_LEN];
  long file_id;
  long file_count;
  const char *file;
  sprintf(signature_str,"%lu",signature);
  DEBUG1("Begin saving function list\n");


  char module_abs[MAX_PATH],file_abs[MAX_PATH];
  realPath(module_name,module_abs);

  file_count =  filenames_p->count();
  for (file_id=0;file_id<file_count;file_id++)
  {
    file = filenames_p->at(file_id);
    realPath(file,file_abs);
    DEBUG3("Begin saving function list, file='%s' absolute path='%s'\n",file,file_abs);

#ifndef NO_DEBUG
    // verify if the file is already treated
    bool exists=false;
    for (int j=0;j<file_id && !exists;j++)
    {
      if (strcmp(file,filenames_p->at(j))==0)
        exists=true;
    }
    if (exists)
    {
      FATAL1("Internal error");
    }
#endif

    if (expressions_p->instrument_exists(file))
    {
      DEBUG3("Begin recording function information, module=abs='%s', file_abs='%s'\n",module_abs,file_abs);
      f.openSection(module_abs,module_name,file_abs,file,"",CSMESFile::FUNCTIONS_INFO,signature,CSMESFile::NEW_RW,0);
      for (std::list<Function>::const_iterator iterator=list.begin();iterator!=list.end();++iterator)
      {
        if (strcmp(file,iterator->getFileName())==0)
        {
          long s_line_abs,e_line_abs,s_line_orig,s_column_orig,e_line_orig,e_column_orig,s_line_pre,s_column_pre,e_line_pre,e_column_pre;
          const char *scoped_name_p=iterator->getScopedName();
          const char *prototype_p=iterator->prototype();
          ASSERT(scoped_name_p);
          ASSERT(prototype_p);

          if (scoped_name_p==NULL)
            scoped_name_p="??";
          if (prototype_p==NULL)
            prototype_p="??";
          DEBUG3("function info, scoped_name='%s' prototype='%s'\n",scoped_name_p,prototype_p);

          iterator->getFunctionCode(s_line_abs,e_line_abs,s_line_orig,s_column_orig,e_line_orig,e_column_orig,s_line_pre,s_column_pre,e_line_pre,e_column_pre) ;
          if (expressions_p->instrument_exists_abs(s_line_abs,s_column_pre,e_line_abs,e_column_pre))
          {
            DEBUG3("record function info, scoped_name='%s' prototype='%s'\n",scoped_name_p,prototype_p);
            f.writeFunctionInfo (scoped_name_p,
                prototype_p,
                s_line_orig,s_column_orig,s_line_pre, s_column_pre, e_line_orig, e_column_orig,e_line_pre, e_column_pre);
          }
        }
      }
      f.writeFunctionInfo ("", "", 0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,   0xFFFFFFFF,  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
      f.closeSection();

      DEBUG3("End recording function information, module=abs='%s', file_abs='%s'\n",module_abs,file_abs);
    }
    DEBUG3("End saving function list, file='%s' absolute path='%s'\n",file,file_abs);
  }
  DEBUG1("End saving function list\n");
}

bool Functions::containsExpression(const std::list<Function>&functions, const Expression &e) 
{
  FUNCTION_TRACE;
  for (std::list<Function>::const_iterator iterator=functions.begin();iterator!=functions.end();++iterator)
  {
    if ( iterator->containsExpression(e) )
      return true;
  }
  return false;
}

void Functions::append(Function &f) 
{
  FUNCTION_TRACE;
  std::string expression_match;
  f.setToExclude (filter_functions_p->isExclude(f.getScopedName(),expression_match));
  if (f.getToExclude())
    exclude_list.push_back(f);
  list.push_back(f) ; 
}

bool Functions::isExcludeFunction(const Expression &e) const
{
  FUNCTION_TRACE;
  return containsExpression(exclude_list,e);
}

