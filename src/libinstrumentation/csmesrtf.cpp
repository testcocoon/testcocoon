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

#include "csmesrtf.h"
#include "instrumentation.h"
#include "rtf.h"
#include "htmlwriter.h"
#include "writer.h"
#include <QFile>
#include <QBuffer>
#include <QTextStream>
#include <QTextDocument>
#include <QTextCursor>
#include "writer.h"

inline uint qHash(const CSMesRTF::SourceCacheKey &f)
{
  uint h=0;
  h = h ^ qHash(f._source);
  h = h ^ qHash(f._module);
  h = h ^ qHash(f._method);
  h = h ^ qHash(f._source_type);
  h = h ^ qHash(f._type);
  h = h ^ qHash(f._selector);
  return h;
}


CSMesRTF::CSMesRTF() : CSMesIO()
{
}

void CSMesRTF::toRTF(ModuleFile module,SourceFile source,source_type_t type,QString &out) const
{
  toRTFtemplate(module,source,type,false,out);
}

void CSMesRTF::toPLAIN(ModuleFile module,SourceFile source,source_type_t type,QString &out) const
{
  out= "";
  if (!findSourceModule(module,source))
    return ;
  switch (type)
  {
    case ORIGINAL:
      toPLAINoriginal(module,source,out);
      break;
    default:
      toPLAINcsmes(module,source,out);
      break;
  }
}

void CSMesRTF::toRTFtemplate(ModuleFile module,SourceFile source,source_type_t type,bool instrumentation,QString &out) const
{
  out= "";
  if (!findSourceModule(module,source))
    return ;
  switch (type)
  {
    case ORIGINAL:
      toRTForiginal(module,source,instrumentation,out);
      break;
    default:
      toRTFcsmes(module,source,instrumentation,out);
      break;
  }
}

void CSMesRTF::toRTFtemplate(ModuleFile module,SourceFile source,source_type_t type,QTextDocument &out) const
{
  out.clear();
  if (!findSourceModule(module,source))
    return ;
  switch (type)
  {
    case ORIGINAL:
      toRTForiginal(module,source,out);
      break;
    default:
      toRTFcsmes(module,source,out);
      break;
  }
}

void CSMesRTF::toRTFSyntax(ModuleFile module,SourceFile source,Instrumentation::coverage_method_t method,source_type_t t,QString &textcpp) const
{
  SourceCacheKey cache_key(module,source,method,t);
  QString *source_text_p=NULL;

  QMutexLocker locker(&_source_cache_mutex);
  source_text_p=_source_cache.object(cache_key);
  if (source_text_p==NULL)
  { // cache miss
    source_cache_miss++;
    QString text;
    QString sourcetext;
    toRTFtemplate(module,source,t,true,sourcetext);
    text.reserve(sourcetext.length()+100);
    text="<html>";
    text+="<body>";
    text+=sourcetext;
    text+="</html>";
    text+="</body>";
    RTF::CPPSyntax(text,textcpp);
    source_text_p=new QString(textcpp);
    if (!_source_cache.insert(cache_key,source_text_p,1))
    { // item too big
      //delete source_text_p;
    }
  }
  else
  { // cache hit
    source_cache_hit++;
    textcpp=*source_text_p;
  }
}

void CSMesRTF::read_source(const ModuleFile &module,const SourceFile &source, CSMESFile::type_t t,QString &qbuf) const
{
  SourceCacheKey cache_key(module,source,t);
  QString *source_text_p=NULL;

  QMutexLocker locker(&_source_cache_mutex);
  source_text_p=_source_cache.object(cache_key);
  if (source_text_p==NULL)
  { // cache miss
    source_cache_miss++;
    source_text_p=new QString();
    CSMesIO::read_source(module,source, t,*source_text_p);
    qbuf=*source_text_p;
    if (!_source_cache.insert(cache_key,source_text_p,1))
    { // item too big
      //delete source_text_p;
    }
  }
  else
  { // cache hit
    source_cache_hit++;
    qbuf=*source_text_p;
  }

}

void CSMesRTF::toPLAINcsmes(const ModuleFile &module,const SourceFile &source,QString &ret) const
{
  read_source(module,source,CSMESFile::PREPROCESSED,ret);
}

void CSMesRTF::toPLAINoriginal(const ModuleFile &module,const SourceFile &source,QString &ret) const
{
  read_source(module,source,CSMESFile::ORIGINAL,ret);
}

void CSMesRTF::toRTFcsmes(const ModuleFile &module,const SourceFile &source,bool instrumentation,QString &ret) const
{
  QString text_with_tab;
    read_source(module,source,CSMESFile::PREPROCESSED,text_with_tab);
  if (!instrumentation)
  {
    ret=text_with_tab;
    return ;
  }

  int sz=text_with_tab.length();

  ret.reserve(sz*5);
  ret+="<pre><p>";
  ret+=Qt::escape(text_with_tab).replace('\n'," </p><p>");
  ret+=" </p></pre>";
}

void CSMesRTF::toRTFcsmes(const ModuleFile &module,const SourceFile &source,QTextDocument &ret) const
{
  QString text_with_tab;
  read_source(module,source,CSMESFile::PREPROCESSED,text_with_tab);
  ret.setPlainText(text_with_tab);
}

void CSMesRTF::toRTForiginal(const ModuleFile &module,const SourceFile &source,bool instrumentation,QString &ret) const
{
  QString text_with_tab;

    read_source(module,source,CSMESFile::ORIGINAL,text_with_tab);
  if (!instrumentation)
  {
     ret = text_with_tab;
    return;
  }

  int sz=text_with_tab.length();
  ret.reserve(sz*5);
  ret ="<pre><p>";
  ret+=Qt::escape(text_with_tab).replace('\n'," </p><p>");
  ret+=" </p></pre>";
  ret.squeeze();
}

void CSMesRTF::toRTForiginal(const ModuleFile &module,const SourceFile &source,QTextDocument &ret) const
{
  QString text_with_tab;

  read_source(module,source,CSMESFile::ORIGINAL,text_with_tab);
  ret.setPlainText(text_with_tab);
}

int CSMesRTF::instrumentationSourceIndex(const CSMesInstrumentations::Instrumentations &inst,int &index_start,int &index_end,bool &start)
{
  int new_index;
  if (index_start==-1)
  {
    start=false;
    new_index = index_end;
    index_end=instrumentationEndIndex(inst,index_end);
  }
  else if (index_end==-1)
  {
    start=true;
    new_index = index_start;
    index_start=instrumentationStartIndex(inst,index_start);
  }
  else if ( (inst[index_start].startLinePre()<inst[index_end].endLinePre())
      || ( (inst[index_start].startLinePre()==inst[index_end].endLinePre())
        && (inst[index_start].startColumnPre())<inst[index_end].endColumnPre()) )
  {
    start=true;
    new_index = index_start;
    index_start=instrumentationStartIndex(inst,index_start);
  }
  else
  {
    start=false;
    new_index = index_end;
    index_end=instrumentationEndIndex(inst,index_end);
  }
  return new_index;
}

int CSMesRTF::instrumentationStartIndex(const CSMesInstrumentations::Instrumentations &inst,int index)
{
  int new_index=-1;
  int inst_max=inst.size();
  long cur_line;
  long cur_column;
  if (index<0)
  {
    cur_line=-1;
    cur_column=-1;
  }
  else
  {
    cur_line=inst[index].startLinePre();
    cur_column=inst[index].startColumnPre();
  }
  for (int i=0;i<inst_max;i++)
  {
    if ( (cur_line<inst[i].startLinePre())
        || ( (cur_line==inst[i].startLinePre())
          && (cur_column<inst[i].startColumnPre()) ) )
    {
      if (new_index==-1)
        new_index=i;
      else if ( (inst[i].startLinePre()<inst[new_index].startLinePre())
      || ( (inst[i].startLinePre()==inst[new_index].startLinePre())
      && (inst[i].startColumnPre())<inst[new_index].startColumnPre()) )
        new_index=i;
    }
  }
  return new_index;
}

int CSMesRTF::instrumentationEndIndex(const CSMesInstrumentations::Instrumentations &inst,int index)
{
  int new_index=-1;
  int inst_max=inst.size();
  long cur_line;
  long cur_column;
  if (index<0)
  {
    cur_line=-1;
    cur_column=-1;
  }
  else
  {
    cur_line=inst[index].endLinePre();
    cur_column=inst[index].endColumnPre();
  }
  for (int i=0;i<inst_max;i++)
  {
    if ( (cur_line<inst[i].endLinePre())
        || ( (cur_line==inst[i].endLinePre())
          && (cur_column<inst[i].endColumnPre()) ) )
    {
      if (new_index==-1)
        new_index=i;
      else if ( (inst[i].endLinePre()<inst[new_index].endLinePre())
          || ( (inst[i].endLinePre()==inst[new_index].endLinePre())
            && (inst[i].endColumnPre())<inst[new_index].endColumnPre()) )
        new_index=i;
    }
  }
  return new_index;
}

void CSMesRTF::linesCoverageCountOrg(ModuleFile mod,SourceFile src,QVector<int> &min,QVector<int> &max) const
{
  if (!findSourceModule(mod,src))
  {
    min.clear();
    max.clear();
    return ;
  }
  const CSMesInstrumentations::Instrumentations &inst=instrumentations.modules[mod].sources[src].instrumentations;
  linesCoverageCountOrg(inst,min,max);
}

void CSMesRTF::linesCoverageCountPre(ModuleFile mod,SourceFile src,QVector<int> &min,QVector<int> &max) const
{
  if (!findSourceModule(mod,src))
  {
    min.clear();
    max.clear();
    return ;
  }
  const CSMesInstrumentations::Instrumentations &inst=instrumentations.modules[mod].sources[src].instrumentations;
  linesCoverageCountPre(inst,min,max);
}

void CSMesRTF::linesCoverageCountPre(const CSMesInstrumentations::Instrumentations &inst,QVector<int> &min,QVector<int> &max) const
{
  min.clear();
  max.clear();
  int inst_max=inst.size();
  for (int i=0;i<inst_max;i++)
  {
    for (int line=inst[i].startLinePre() ;line<=inst[i].endLinePre(); line++ )
    {
      if (min.size()<=line)
      {
        int oldsize=min.size();
        min.resize(line+1);
        max.resize(line+1);
        for (int i=oldsize;i<=line;i++)
        {
          min[i]=0x7FFFFFFF;
          max[i]=-1;
        }
      }
      if (min[line]>inst[i].executionCountMin())
        min[line]=inst[i].executionCountMin();
      if (max[line]<inst[i].executionCountMax())
        max[line]=inst[i].executionCountMax();
    }
  }
}

void CSMesRTF::linesCoverageCountOrg(const CSMesInstrumentations::Instrumentations &inst,QVector<int> &min,QVector<int> &max) const
{
  min.clear();
  max.clear();
  int inst_max=inst.size();
  for (int i=0;i<inst_max;i++)
  {
    for (int line=inst[i].startLineOrg() ;line<=inst[i].endLineOrg(); line++ )
    {
      if (min.size()<=line)
      {
        int oldsize=min.size();
        min.resize(line+1);
        max.resize(line+1);
        for (int i=oldsize;i<=line;i++)
        {
          min[i]=0x7FFFFFFF;
          max[i]=-1;
        }
      }
      if (min[line]>inst[i].executionCountMin())
        min[line]=inst[i].executionCountMin();
      if (max[line]<inst[i].executionCountMax())
        max[line]=inst[i].executionCountMax();
    }
  }
}

Instrumentation::status_t CSMesRTF::lineStatusOrg(const CSMesInstrumentations::Instrumentations &inst,int level,Instrumentation::coverage_method_t method,int line) const
{
  Instrumentation::status_t status=Instrumentation::STATUS_NONE;
  int inst_max=inst.size();
  for (int i=0;i<inst_max;i++)
  {
    if ( line>=inst[i].startLineOrg()
        &&  line<=inst[i].endLineOrg() )
    {
      if (method==Instrumentation::COVERAGE_BRANCH)
      {
        if (inst[i].isConditionInstrumentation())
          continue; // skip conditions
      }
      status = Instrumentation::combine(status,inst[i].status(level,method));
    }
  }
  return status;
}

Instrumentation::status_t CSMesRTF::lineStatusPre(const CSMesInstrumentations::Instrumentations &inst,int level,Instrumentation::coverage_method_t method,int line) const
{
  Instrumentation::status_t status=Instrumentation::STATUS_NONE;
  int inst_max=inst.size();
  for (int i=0;i<inst_max;i++)
  {
    if ( line>=inst[i].startLinePre()
        &&  line<=inst[i].endLinePre() )
    {
      if (method==Instrumentation::COVERAGE_BRANCH)
      {
        if (inst[i].isConditionInstrumentation())
          continue; // skip conditions
      }
      status = Instrumentation::combine(status,inst[i].status(level,method));
    }
  }
  return status;
}

const QVector<Instrumentation>  CSMesRTF::instrumentationList(ModuleFile mod,SourceFile src) const
{
  if (!findSourceModule(mod,src))
  {
    QVector<Instrumentation> null_inst;
    return null_inst;
  }
  return instrumentations.modules[mod].sources[src].instrumentations;
}

void CSMesRTF::remove_tabs (const QString &input,QString &output,int tab_size) const
{
  if (tab_size<=0)
  {
    output=input;
    return;
  }

  int col=0;
  int pos1=0;
  int pos2=0;
  int sz=input.length();
  output.clear();
  output.reserve(sz*tab_size);
  for (int i=0;i<sz;i++)
  {
    const QChar &c=input.at(i);
    if (c=='\n')
      col=0;
    else if (c=='\t')
    {
      col++;
      output+=input.mid(pos1,pos2-pos1);
      output+=' ';
      for (int j=0;( col%tab_size ) !=0;j++)
      {
        col++;
        output+=' ';
      }
      pos1=pos2+1;
    }
    else
      col++;
    pos2++;
  }
  output+=input.mid(pos1,pos2-pos1);
}

QString CSMesRTF::extractText(const QString &text,int line1,int col1,int line2,int col2)
{
  int pos1=-1;
  int pos2=-1;
  int ml=line1;
  if (ml<line2) ml=line2;
  int pos=0;
  for (int l=1;l<=ml;l++)
  {
    if (pos>=0)
    {
      if (l==line2)
        pos2=col2-1+pos;
      if (l==line1)
        pos1=col1-1+pos;
      pos=text.indexOf('\n',pos);
      if (pos>=0)
        pos++;
    }
    else
      break;
  }
  if (pos1==-1)
    return QString::null;
  if (pos2==-1)
    pos2=text.length();

  return text.mid(pos1,pos2-pos1);
}

QString CSMesRTF::extractPlainTextPreprocessed(ModuleFile module,SourceFile source,int line1,int col1,int line2,int col2) const
{
  QString qbuf;
  findSourceModule(module,source);
  read_source(module,source,CSMESFile::PREPROCESSED,qbuf);
  return extractText(qbuf,line1,col1,line2,col2);
}

void CSMesRTF::getSourceCacheStatistics(int &cache_hit,int &cache_miss,int &max_cost,int &cost) const
{
  cache_hit  = source_cache_hit;
  cache_miss = source_cache_miss;
  cost       = _source_cache.totalCost();
  max_cost   = _source_cache.maxCost();
}

void CSMesRTF::setSourceCacheMaxCost(long value)
{
  QMutexLocker locker(&_source_cache_mutex);
  _source_cache.setMaxCost(value);
}

bool CSMesRTF::loadCSMes(const QString &s)
{
  clearSourceCache();
  return CSMesIO::loadCSMes(s);
}

void CSMesRTF::clearSourceCache()
{
  source_cache_miss=0;
  source_cache_hit=0;
  QMutexLocker locker(&_source_cache_mutex);
  _source_cache.clear();
}

void  CSMesRTF::statusLinesOrg(ModuleFile mod,SourceFile src,int level,Instrumentation::coverage_method_t method,int number_of_lines,QVector<Instrumentation::status_t> &status) const
{
  status.clear();
  status.resize(number_of_lines);
  for (int j=0;j<number_of_lines;j++)
    status[j]=Instrumentation::STATUS_NONE;

  if (!findSourceModule(mod,src))
    return ;
  const CSMesInstrumentations::Instrumentations &inst=instrumentations.modules[mod].sources[src].instrumentations;

  int inst_max=inst.size();
  for (int i=0;i<inst_max;i++)
  {
    if (method==Instrumentation::COVERAGE_BRANCH)
    {
      if (inst[i].isConditionInstrumentation())
        continue; // skip conditions
    }
    for (int line=inst[i].startLineOrg();line<=inst[i].endLineOrg();line++)
    {
      if (line>=number_of_lines)
      {
        status.resize(line+100);
        for (int j=number_of_lines;j<line+100;j++)
          status[j]=Instrumentation::STATUS_NONE;
        number_of_lines=line+100;
      }
      status[line] = Instrumentation::combine(status[line],inst[i].status(level,method));
    }
  }
}

void  CSMesRTF::statusLinesPre(ModuleFile mod,SourceFile src,int level,Instrumentation::coverage_method_t method,int number_of_lines,QVector<Instrumentation::status_t> &status) const
{
  status.clear();
  status.resize(number_of_lines);
  for (int j=0;j<number_of_lines;j++)
    status[j]=Instrumentation::STATUS_NONE;

  if (!findSourceModule(mod,src))
    return ;
  const CSMesInstrumentations::Instrumentations &inst=instrumentations.modules[mod].sources[src].instrumentations;

  int inst_max=inst.size();
  for (int i=0;i<inst_max;i++)
  {
    if (method==Instrumentation::COVERAGE_BRANCH)
    {
      if (inst[i].isConditionInstrumentation())
        continue; // skip conditions
    }
    for (int line=inst[i].startLinePre();line<=inst[i].endLinePre();line++)
    {
      if (line>=number_of_lines)
      {
        status.resize(line+100);
        for (int j=number_of_lines;j<line+100;j++)
          status[j]=Instrumentation::STATUS_NONE;
        number_of_lines=line+100;
      }
      status[line] = Instrumentation::combine(status[line],inst[i].status(level,method));
    }
  }
}

void CSMesRTF::toRTFSyntax(ModuleFile module,SourceFile source,int level,Instrumentation::coverage_method_t method,source_type_t t,bool tooltip,int executed_by_limit,QTextDocument &out) const
{
  if (!findSourceModule(module,source))
  {
    out.clear();
    return;
  }

  QString html;
  toRTFSyntax(module,source,method,t,html);
  out.setHtml(html);
  switch (t)
  {
    case ORIGINAL:
      colorBackgroundLineOrg(module,source,level,method,tooltip,executed_by_limit,out);
      break;
    default:
      colorBackgroundInstrumentationPre(module,source,level,method,tooltip,executed_by_limit,out);
      break;
  }

}

void CSMesRTF::colorBackgroundLineOrg(const ModuleFile &module,const SourceFile &source,int level,Instrumentation::coverage_method_t method,bool tooltip,int executed_by_limit,QTextDocument &out) const
{
  QTextCursor cur(&out);
  cur.movePosition(QTextCursor::End);
  int number_of_lines=cur.blockNumber();
  QBrush background_format;
  background_format.setStyle(INSTRUMENTATION_BACKGROUND_STYLE);
  if (number_of_lines>=0)
  {
    QVector<Instrumentation::status_t> status_line;
    statusLinesOrg(module,source,level,method,number_of_lines+1,status_line);
    for (int i=1;i<number_of_lines+1;i++)
    {
      const Instrumentation::status_t &st=status_line.at(i);
      if (st!=Instrumentation::STATUS_NONE)
      {
        QColor c=Instrumentation::statusColor(st);
        QTextCharFormat char_format;
        background_format.setColor(c.light(INSTRUMENTATION_BACKGROUND_LIGHTING));
        char_format.setBackground(background_format);
        if (tooltip)
        {
          QBuffer buffer;
          HTMLWriter writer(&buffer);
          buffer.open(QIODevice::WriteOnly);
          explanationFromLineOrg(writer,module,source,i,level,method,executed_by_limit);
          buffer.close();
          char_format.setToolTip(buffer.data());
        }
        cur.movePosition(QTextCursor::Start);
        cur.movePosition(QTextCursor::NextBlock,QTextCursor::MoveAnchor,i-1);
        cur.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
        cur.mergeCharFormat(char_format);
      }
    }
  }
}

void CSMesRTF::colorBackgroundInstrumentationPre(const ModuleFile &module,const SourceFile &source,int level,Instrumentation::coverage_method_t method,bool tooltip,int executed_by_limit,QTextDocument &out) const
{
  QTextCursor cur(&out);
  cur.movePosition(QTextCursor::End);
  QBrush background_format;
  background_format.setStyle(INSTRUMENTATION_BACKGROUND_STYLE);
  const CSMesInstrumentations::Instrumentations &inst=instrumentations.modules[module].sources[source].instrumentations;
  int number_of_index=inst.count();
  for (int i=0;i<number_of_index;i++)
  {
    if (method==Instrumentation::COVERAGE_BRANCH)
    {
      if (inst[i].isConditionInstrumentation())
        continue; // skip conditions
    }
    QColor color=Instrumentation::statusColor(inst[i].status(level,method));
    int s_line=inst[i].startLinePre()-1;
    int e_line=inst[i].endLinePre()-1;
    int s_column=inst[i].startColumnPre()-1;
    int e_column=inst[i].endColumnPre()-1;

    QTextCharFormat char_format;
    if (tooltip)
    {
      QBuffer buffer;
      HTMLWriter writer(&buffer);
      buffer.open(QIODevice::WriteOnly);
      explanationFromIndex(writer,module,source,i,CSMES,level,method,executed_by_limit);
      buffer.close();
      char_format.setToolTip(buffer.data());
    }
    background_format.setColor(color.light(INSTRUMENTATION_BACKGROUND_LIGHTING));
    char_format.setBackground(background_format);
    cur.movePosition(QTextCursor::Start);
    cur.movePosition(QTextCursor::NextBlock,QTextCursor::MoveAnchor,s_line);
    cur.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,s_column);
    cur.movePosition(QTextCursor::Start,QTextCursor::KeepAnchor);
    cur.movePosition(QTextCursor::NextBlock,QTextCursor::KeepAnchor,e_line);
    cur.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,e_column);
    cur.mergeCharFormat(char_format);
  }
}


void CSMesRTF::explanationFromLineOrg (Writer &stream,const ModuleFile &module,const SourceFile &source,int line,int coverage_level,Instrumentation::coverage_method_t method, int executed_by_limit) const
{
  stream.beginHeader();
  stream.endHeader();
  stream.beginBody();
  const QVector<Instrumentation> &inst=instrumentationList(module,source);
  int sz=inst.size();
  QList<int> indexs;
  for (int i=0;i<sz;i++)
  {
    int s_line=inst.at(i).startLineOrg();
    int e_line=inst.at(i).endLineOrg();
    if ( line>=s_line &&  line<=e_line  )
    {
      if (!indexs.contains(i))
        indexs.append(i);
    }
  }
  for (QList<int>::const_iterator it=indexs.begin();it!=indexs.end();++it)
  {
    int index=*it;
    WriterSection sec(stream,Writer::HtmlCodeFragment);
    explanationFragmentFromIndex(stream,module,source,index,ORIGINAL,coverage_level,method,executed_by_limit,false);
  }
  stream.endBody();
}

void CSMesRTF::explanationFromLinePre (Writer &stream,const ModuleFile &module,const SourceFile &source,int line,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit) const
{
  stream.beginHeader();
  stream.endHeader();
  stream.beginBody();
  const QVector<Instrumentation> &inst=instrumentationList(module,source);
  int sz=inst.size();
  QList<int> indexs;
  for (int i=0;i<sz;i++)
  {
    int s_line=inst.at(i).startLinePre();
    int e_line=inst.at(i).endLinePre();
    if ( line>=s_line &&  line<=e_line  )
    {
      if (!indexs.contains(i))
        indexs.append(i);
    }
  }
  for (QList<int>::const_iterator it=indexs.begin();it!=indexs.end();++it)
  {
    int index=*it;
    WriterSection sec(stream,Writer::HtmlCodeFragment);
    explanationFragmentFromIndex(stream,module,source,index,CSMES,coverage_level,method,executed_by_limit,false);
  }
  stream.endBody();
}

void CSMesRTF::explanationFromIndex (Writer& stream,const ModuleFile &module,const SourceFile &source,int index,source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit) const
{
  stream.beginHeader();
  stream.endHeader();
  stream.beginBody();
  {
    WriterSection sec(stream,Writer::HtmlCodeFragment);
    explanationFragmentFromIndex(stream,module,source,index,source_type,coverage_level,method,executed_by_limit,false);
  }
  stream.endBody();
}

QString CSMesRTF::explanationFragmentTextFromIndex (const QString &format, const ModuleFile &module,const SourceFile &source,int index,source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method) const
{
  const QVector<Instrumentation> &inst=instrumentationList(module,source);
  if (inst.isEmpty())
    return QString();
  const Instrumentation &cur_inst=inst.at(index);
  int s_col=cur_inst.startColumnPre();
  int s_line=cur_inst.startLinePre();
  int e_col=cur_inst.endColumnPre();
  int e_line=cur_inst.endLinePre();

  QString message =cur_inst.explanationPlainText(extractPlainTextPreprocessed(module,source,s_line,s_col,e_line,e_col), coverage_level,method);

  QString explanation=format;
  explanation.replace("%f",source);
  explanation.replace("%l",QString::number(cur_inst.startLineOrg()));
  explanation.replace("%c",QString::number(cur_inst.startColumnOrg()>0?cur_inst.startColumnOrg():cur_inst.startColumnPre()));
  explanation.replace("%m",message);
  return explanation;
}

void CSMesRTF::explanationFragmentFromIndex (Writer &stream,const ModuleFile &module,const SourceFile &source,int index,source_type_t source_type,int coverage_level, Instrumentation::coverage_method_t method,int executed_by_limit,bool table_fragment) const
{
   ExecutionNames executedByExecution[2];
   const QVector<Instrumentation> &inst=instrumentationList(module,source);
   if (inst.isEmpty())
      return ;
   ExecutionNames executionList=selectedExecutions();
   const Instrumentation &cur_inst=inst.at(index);
   int s_col, s_line, e_col, e_line;
   if (source_type == ORIGINAL)
   {
     s_col=cur_inst.startColumnOrg();
     s_line=cur_inst.startLineOrg();
     e_col=cur_inst.endColumnOrg();
     e_line=cur_inst.endLineOrg();
   }
   else
   {
     s_col=cur_inst.startColumnPre();
     s_line=cur_inst.startLinePre();
     e_col=cur_inst.endColumnPre();
     e_line=cur_inst.endLinePre();
   }
   for (int i=cur_inst.getMinIndex();i<=cur_inst.getMaxIndex();i++)
   {
      ExecutionNames execByList=executedBy(module,source,i,true,executed_by_limit);
      for (ExecutionNames::const_iterator exeit=execByList.begin();
         exeit!=execByList.end();
         ++exeit)
      {
         if (executionList.contains(*exeit))
            executedByExecution[i-cur_inst.getMinIndex()]+=*exeit;
      }
      qStableSort(executedByExecution[i-cur_inst.getMinIndex()]);
   }

   if (table_fragment)
   {
     {
       stream.begin(Writer::Line);

       stream.begin(Writer::HtmlLineInfo);
       stream << QObject::tr("line");
       stream << " ";
       stream.end(Writer::HtmlLineInfo);

       if (s_line==e_line && e_col==s_col)
         stream << QString::number(s_line);
       else
       {
         stream.begin(Writer::LineFrom);
         stream << QString::number(s_line);
         stream.end(Writer::LineFrom);
         if (s_col>=0)
         {
           stream.begin(Writer::HtmlLineInfo);
           stream << ".";
           stream.end(Writer::HtmlLineInfo);
           stream.begin(Writer::ColFrom);
           stream << QString::number(s_col);
           stream.end(Writer::ColFrom);
         }
         stream.begin(Writer::HtmlLineInfo);
         stream << "-";
         stream.end(Writer::HtmlLineInfo);
         stream.begin(Writer::LineTo);
         stream << QString::number(e_line);
         stream.end(Writer::LineTo);
         if (e_col>=0)
         {
           stream.begin(Writer::HtmlLineInfo);
           stream << ".";
           stream.end(Writer::HtmlLineInfo);
           stream.begin(Writer::ColTo);
           stream << QString::number(e_col);
           stream.end(Writer::ColTo);
         }
       }

       stream.begin(Writer::HtmlLineInfo);
       stream << ": ";
       stream.end(Writer::HtmlLineInfo);

       stream.end(Writer::Line);
     }
     stream.begin(Writer::FragmentExplanation);
   }
   {
     int s_col=cur_inst.startColumnPre();
     int s_line=cur_inst.startLinePre();
     int e_col=cur_inst.endColumnPre();
     int e_line=cur_inst.endLinePre();
     cur_inst.explanation(stream,
         extractPlainTextPreprocessed(module,source,s_line,s_col,e_line,e_col),
         getComment(module,source,index),
         executedByExecution,coverage_level,method,executed_by_limit);
   }
   if (table_fragment)
     stream.end(Writer::FragmentExplanation);
}
