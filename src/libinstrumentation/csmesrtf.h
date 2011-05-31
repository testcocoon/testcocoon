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

#ifndef CSMES_RTF_H
#define CSMES_RTF_H
#include "csmesio.h"
#include <QCache>
#include <QTextDocument>
class Writer;

class CSMesRTF : public CSMesIO
{
  protected:
    CSMesRTF();
  public:
    enum source_type_t { SOURCE_UNDEF, ORIGINAL, CSMES };
  protected:
    void toRTFSyntax(ModuleFile module,SourceFile source,int level,Instrumentation::coverage_method_t ,source_type_t,bool tooltip,int executed_by_limit,QTextDocument &out) const;
    void toRTFSyntax(ModuleFile module,SourceFile source,Instrumentation::coverage_method_t ,source_type_t,QString &out) const;

    const QVector<Instrumentation> instrumentationList(ModuleFile mod,SourceFile src) const;
    QString extractPlainTextPreprocessed(ModuleFile module,SourceFile source,int line1,int col1,int line2,int col2) const;
    bool loadCSMes(const QString &file);
    void  statusLinesPre(ModuleFile mod,SourceFile src,int level,Instrumentation::coverage_method_t ,int number_of_lines,QVector<Instrumentation::status_t> &lines) const;
    void  statusLinesOrg(ModuleFile mod,SourceFile src,int level,Instrumentation::coverage_method_t ,int number_of_lines,QVector<Instrumentation::status_t> &lines) const;
    void explanationFromIndex (Writer &, const ModuleFile &module,const SourceFile &source,int index,source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t,int executed_by_limit) const;
    void explanationFragmentFromIndex (Writer &, const ModuleFile &module,const SourceFile &source,int index,source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t,int executed_by_limit, bool table_fragment) const;
    QString explanationFragmentTextFromIndex (const QString &format, const ModuleFile &module,const SourceFile &source,int index,source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t) const;
    void explanationFromLineOrg (Writer &, const ModuleFile &module,const SourceFile &source,int line,int coverage_level,Instrumentation::coverage_method_t,int executed_by_limit) const;
    void explanationFromLinePre (Writer &, const ModuleFile &module,const SourceFile &source,int line,int coverage_level,Instrumentation::coverage_method_t,int executed_by_limit) const;
    void toPLAIN(ModuleFile module,SourceFile source,source_type_t type,QString &out) const;
    void linesCoverageCountOrg(ModuleFile mod,SourceFile src,QVector<int> &min,QVector<int> &max) const;
    void linesCoverageCountPre(ModuleFile mod,SourceFile src,QVector<int> &min,QVector<int> &max) const;
  protected:
    int instrumentationSourceIndex(const CSMesInstrumentations::Instrumentations &inst,int &index_start,int &index_end,bool &start);
    int instrumentationStartIndex(const CSMesInstrumentations::Instrumentations &inst,int index);
    int instrumentationEndIndex(const CSMesInstrumentations::Instrumentations &inst,int index);
    Instrumentation::status_t lineStatusOrg(const CSMesInstrumentations::Instrumentations &inst,int level,Instrumentation::coverage_method_t ,int line) const;
    Instrumentation::status_t lineStatusPre(const CSMesInstrumentations::Instrumentations &inst,int level,Instrumentation::coverage_method_t ,int line) const;
    void read_source(const ModuleFile &module,const SourceFile &source, CSMESFile::type_t t,QString &qbuf) const;
 public:
    static QString extractText(const QString &text,int line1,int col1,int line2,int col2) ;
    LIBINSTRUMENTATION_API    void getSourceCacheStatistics(int &cache_hit,int &cache_miss,int &max_cost,int &cost) const;
    LIBINSTRUMENTATION_API    void setSourceCacheMaxCost(long value);
  private:
    void toRTFcsmes(const ModuleFile &module,const SourceFile &source,QTextDocument &ret) const;
    void toRTFtemplate(ModuleFile module,SourceFile source,source_type_t type,QTextDocument &out) const;
    void toRTForiginal(const ModuleFile &module,const SourceFile &source,QTextDocument &ret) const;
    void linesCoverageCountOrg(const CSMesInstrumentations::Instrumentations &inst,QVector<int> &min,QVector<int> &max) const;
    void linesCoverageCountPre(const CSMesInstrumentations::Instrumentations &inst,QVector<int> &min,QVector<int> &max) const;
    void toRTF(ModuleFile module,SourceFile source,source_type_t,QString &out) const;
    void toPLAINoriginal(const ModuleFile &module,const SourceFile &source,QString &ret) const;
    void toPLAINcsmes(const ModuleFile &module,const SourceFile &source,QString &ret) const;
    void toRTFtemplate(ModuleFile module,SourceFile source,source_type_t,bool instrumentation,QString &out) const;
    void toRTForiginal(const ModuleFile &module,const SourceFile &source,bool instrumentation,QString &out) const;
    void toRTFcsmes(const ModuleFile &module,const SourceFile &source,bool instrumentation,QString &out) const;

    class SourceCacheKey
    {
      private:
        friend inline uint qHash(const CSMesRTF::SourceCacheKey &f) ;
        ModuleFile _module;
        SourceFile _source;
        Instrumentation::coverage_method_t _method;
        source_type_t _source_type;
        CSMESFile::type_t  _type;
        bool _selector;
      public:
        SourceCacheKey(const ModuleFile &module,const SourceFile &source,CSMESFile::type_t type)
        {
          _source=source;
          _module=module;
          _method=Instrumentation::METHOD_UNKNOWN;
          _source_type=SOURCE_UNDEF;
          _type=type ;
          _selector=false;
        }
        SourceCacheKey(const ModuleFile &module,const SourceFile &source,Instrumentation::coverage_method_t method,source_type_t source_type)
        {
          _source=source;
          _module=module;
          _method=method;
          _source_type=source_type;
          _type=CSMESFile::NONE ;
          _selector=true;
        }
        bool operator== ( const SourceCacheKey & x ) const
        {
          if (_selector)
            return _source==x._source &&
              _module==x._module &&
              _method==x._method &&
              _source_type==x._source_type;
          else
            return _source==x._source &&
              _module==x._module &&
              _type==x._type;
        }
    };
    friend inline uint qHash(const CSMesRTF::SourceCacheKey &f) ;
    void clearSourceCache();

    mutable QMutex _source_cache_mutex;
    mutable QCache<SourceCacheKey,QString> _source_cache ;
    mutable int source_cache_miss,source_cache_hit;
    inline void colorBackgroundLineOrg(const ModuleFile & module,const SourceFile & source,int level,Instrumentation::coverage_method_t ,bool tooltip,int executed_by_limit,QTextDocument &out) const ;
    inline void colorBackgroundInstrumentationPre(const ModuleFile & module,const SourceFile & source,int level,Instrumentation::coverage_method_t ,bool tooltip,int executed_by_limit,QTextDocument &out) const;
    void remove_tabs (const QString &input,QString &output,int tab_size) const;
} ;
#endif
