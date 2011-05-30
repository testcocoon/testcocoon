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

#ifndef _WRITE_H
#define _WRITE_H
#include <QTextStream>
#include <QIODevice>
#include <QStack>
#include <QSet>
#include <QStringList>
#include <QColor>
#include "libinstrumentationpdef.h"

class Writer 
{
  public:
    enum section_type_t
    {
      ExecutionList,
      HtmlLevel,
      ExecutionListItem,
      HtmlExecutionListContainComment,
      ExecutionListName,
      ExecutionListStatusFailed,
      ExecutionListStatusPassed,
      HtmlStatisticValue,
      ExecutionListStatusToBeCheckManually,
      ExecutionListStatusUnknown,
      HtmlExecutionListHeader,
      HtmlExecutionListHeaderItem,
      HtmlExecutionListTitle,
      HtmlFragmentsManuallyValidatedTitle,
      HtmlFragmentsUnexecutedTitle,
      HtmlFragmentsExecutedTitle,
      FragmentsManuallyValidated,
      FragmentsUnexecuted,
      FragmentsExecuted,
      FromLineOrg,
      FromLinePre,
      ToLinePre,
      ToLineOrg,
      FromColumnPre,
      ToColumnPre,
      FunctionInfo,
      FunctionPrototype,
      HtmlMethodRelativeSourceFileName,
      HtmlMethodScopedName,
      HtmlGlobalCoverage,
      GlobalInformation,
      GlobalInformationCoverageMethod,
      HtmlInformationCoverageMethodDescription,
      HtmlInformationCoverageMethodValue,
      GlobalInformationCoverageMode,
      HtmlInformationCoverageModeDescription,
      HtmlInformationCoverageModeValue,
      HtmlInformation,
      HtmlSource,
      HtmlStatisticSourcesTitle,
      HtmlStatisticExecutionTitle,
      HtmlStatisticMethodTitle,
      MethodStatistics,
      GlobalStatistic,
      ExecutionStatistics,
      SourcesStatistics,
      HtmlCodeFragment,
      Explanation,
      FragmentExplanation,
      CoverageBrowserExplanationTable,
      SourceCode,
      Comment,
      HtmlExecutionState,
      ExecutionStateExecutionCountTooLow,
      ExecutionStateExecuted,
      ExecutionStateNone,
      ExecutionStateNotExecuted,
      ExecutionStateUnderLicense,
      ExecutionStateHidden,
      ExecutionStateUnknown,
      Note,
      NoteTitle,
      Line,
      LineFrom,
      ColFrom,
      ColTo,
      LineTo,
      HtmlLineInfo,
      HtmlSourceCode,
      ExecutedBy,
      HtmlExecutedByTitle,
      HtmlExecutedByList,
      ExecutedByItem,
      HtmlExecutionCountTitle,
      HtmlExecutionCountValue,
      ExecutionCount,
      FragmentExplanationTrueFalseTable,
      HtmlExplanationTrueFalseTableHeader,
      HtmlExplanationTrueFalseTableHeaderItem,
      HtmlExplanationTrueFalseTableContain,
      ExplanationFalseCase,
      ExecutionStateNotInstrumented,
      ExplanationTrueCase,
      StatusPartiallyExecuted,
      StatusUnexecuted,
      StatusExecuted,
      StatusManuallyMarktAsExecuted,
      StatusUnderLicense,
      StatusUnknown,
      StatusNone,
      StatusHidden,
      CodeFragment,
      HtmlCodeFragmentsEmpty,
      CodeFragmentItemSource,
      CodeFragmentItem,
      CodeFragmentItemExplanationItem,
      HtmlCodeFragmentItemExplanationItemOdd,
      HtmlCodeFragmentItemExplanationItemEven,
      CodeFragmentItemExplanation,
      HtmlTocLevel1,
      HtmlTocLevel1Item,
      HtmlTocLevel2,
      HtmlTocLevel2Entry,
      HtmlTocLinkExecution,
      HtmlTocLinkGlobalCoverage,
      HtmlTocLinkManuallyValidated,
      HtmlTocLinkMethod,
      HtmlTocLinkSource,
      HtmlTocLinkUnexecuted,
      HtmlTocLinkExecuted,
      HtmlItemEven,
      HtmlItemOdd,
      Item,
      ItemFunction,
      ItemTitle,
      HtmlTableFormat,
      ItemStatistic,
      ItemStatisticLevel,
      ItemStatisticValue,
      ItemSource,
      ExecutionName,
      ItemTitleLevel,
      HtmlTableFormatScopedName,
      HtmlTableFormatSourceRelative,
      HtmlTableFormatPrototype,
      HtmlTableFormatExecutionStatus,
      ItemFunctionScopedName,
      ItemFunctionSource,
      ItemFunctionPrototype,
      HtmlItemTitle,
    };

    enum numeric_format
    {
      PERCENT,
      NUMERIC,
      BARGRAPH,
    };

    Writer (QIODevice *);
    virtual ~Writer () {}

    Writer &operator<<(const QString &) ;
    Writer &operator<<(int data) ;
    Writer &operator<<(long data) ;
    Writer &operator<<(double data) ;
    Writer &operator<<(const char *v) { return operator<<(QString(v)); }
    Writer &operator<<(numeric_format data) { _numric_format=data; return *this; }
    Writer &operator<<(const QColor & data) { _color=data; return *this; }
    virtual void insertHtml(const QString &)=0;

    void begin(section_type_t);
    virtual void writeLine()=0;
    void end(section_type_t);
    virtual void beginBody()=0;
    virtual void endBody()=0;
    virtual void beginHeader()=0;
    virtual void endHeader()=0;
    void setTitle(const QString &s) { title=s; }
    void setIconFile(const QString &s) { icon_file=s; }
    void setCssFile(const QString &s) { css_file=s; }
    void setDisplayFooter(bool b) { display_footer=b; }
  private:
    QSet<section_type_t> tags_to_ignore_contain;
    QString title,icon_file,css_file;
    bool display_footer;
    numeric_format _numric_format;
    QColor _color;
    QSet<section_type_t> tags_to_ignore;
    int ignore_count;
    bool ignoreOutput() const { return ignore_count>0; }
    
  protected:
    void excludeTagAndContain(section_type_t t) { excludeContain(t); excludeTag(t) ; }
    void excludeContain(section_type_t t) { tags_to_ignore_contain.insert(t) ; }
    void excludeTag(section_type_t t) { tags_to_ignore.insert(t) ; }
    numeric_format getNumericFormat() const { return _numric_format; }
    QColor getColor() const { return _color; }
    virtual void _begin(section_type_t)=0;
    virtual void _end(section_type_t)=0;
    virtual void _write(int)=0;
    virtual void _write(const QString &)=0;
    virtual void _write(double)=0;
    QString getTitle() const { return title; }
    QString getIconFile() const { return icon_file; }
    QString getCssFile() const { return css_file; }
    bool displayFooter() const { return display_footer; }
    static QString sectionName(section_type_t);
} ;

class WriterSection
{
  public:
    WriterSection(Writer &writer,Writer::section_type_t section) : _writer(writer)
    {
      _section=section; 
      _writer.begin(_section); 
    }

    ~WriterSection()
    {
      _writer.end(_section);
    }

  private:
    Writer &_writer;
    Writer::section_type_t _section;
};

#endif
