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

#ifndef CSMES_FUNCTION_INFO_H
#define CSMES_FUNCTION_INFO_H
#include "libinstrumentationpdef.h"
#include "csmesinstrumentation.h"

class CSMesFunctionInfo : public CSMesInstrumentation
{
  protected:
    CSMesFunctionInfo() : CSMesInstrumentation() {}
  public:
    class functionskey_t
    {
      public:
        FunctionInfo method;
        SourceFile source;
        bool operator== ( const functionskey_t & x ) const
        {
          return method==x.method
            &&
            source==x.source;
        }
    };
  protected:
    QList<functionskey_t> Functions() const ;

    QVector<FunctionInfo> FunctionInfoSource( ModuleFile module, SourceFile source) const;
    const FunctionInfo * FindFunction(const QString &source,const FunctionInfo &k) const ;
    bool instrumentationListFunctionPre(ModuleFile module,SourceFile source,long start_line,long start_column,long end_line,long end_column,QList<int> &instrumentation_list) const;

};

inline uint qHash(const CSMesFunctionInfo::functionskey_t &f) { return qHash(f.method.getScopedName()); }

#endif
