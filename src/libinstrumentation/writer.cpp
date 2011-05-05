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

#include "writer.h"
#include <QtGlobal>

Writer::Writer (QIODevice *) 
{
  tags_to_ignore.clear();
  tags_to_ignore_contain.clear();
  ignore_count=0;
}

void Writer::begin(section_type_t s)
{
#ifndef NO_DEBUG
  sections.push(s);
#endif
  bool ignore=ignoreOutput();
  if (tags_to_ignore_contain.contains(s))
    ignore_count++;
  if (tags_to_ignore.contains(s))
    ignore=true;
  if (!ignore)
    _begin(s);
}

void Writer::end(section_type_t s)
{
#ifndef NO_DEBUG
  ASSERT(!sections.isEmpty());
  ASSERT( s == sections.pop()) ;
#endif
  if (tags_to_ignore_contain.contains(s))
    ignore_count--;
  bool ignore=ignoreOutput();
  if (tags_to_ignore.contains(s))
    ignore=true;
  if (!ignore)
    _end(s);
}

#define stringify_case(X) case X: return # X;
QString Writer::sectionName(section_type_t sec)
{
  switch(sec)
  {
    stringify_case(HtmlLevel);
    stringify_case(ExecutionList);
    stringify_case(CoverageBrowserExplanationTable);
    stringify_case(ExecutionListItem);
    stringify_case(HtmlExecutionListContainComment);
    stringify_case(ExecutionListName);
    stringify_case(ExecutionListStatusFailed);
    stringify_case(ExecutionListStatusPassed);
    stringify_case(ExecutionStateUnderLicense);
    stringify_case(Note);
    stringify_case(NoteTitle);
    stringify_case(ToLineOrg);
    stringify_case(ToLinePre);
    stringify_case(ToColumnPre);
    stringify_case(FromLineOrg);
    stringify_case(FromLinePre);
    stringify_case(FromColumnPre);
    stringify_case(HtmlStatisticValue);
    stringify_case(ExecutionListStatusToBeCheckManually);
    stringify_case(ExecutionListStatusUnknown);
    stringify_case(HtmlExecutionListHeader);
    stringify_case(HtmlExecutionListHeaderItem);
    stringify_case(HtmlExecutionListTitle);
    stringify_case(HtmlFragmentsManuallyValidatedTitle);
    stringify_case(HtmlFragmentsUnexecutedTitle);
    stringify_case(HtmlFragmentsExecutedTitle);
    stringify_case(FragmentsManuallyValidated);
    stringify_case(FragmentsUnexecuted);
    stringify_case(FragmentsExecuted);
    stringify_case(FunctionInfo);
    stringify_case(FunctionPrototype);
    stringify_case(HtmlMethodRelativeSourceFileName);
    stringify_case(HtmlMethodScopedName);
    stringify_case(HtmlGlobalCoverage);
    stringify_case(GlobalInformation);
    stringify_case(GlobalInformationCoverageMethod);
    stringify_case(HtmlInformationCoverageMethodDescription);
    stringify_case(HtmlInformationCoverageMethodValue);
    stringify_case(GlobalInformationCoverageMode);
    stringify_case(HtmlInformationCoverageModeDescription);
    stringify_case(HtmlInformationCoverageModeValue);
    stringify_case(HtmlInformation);
    stringify_case(HtmlSource);
    stringify_case(HtmlSourceCode);
    stringify_case(HtmlStatisticSourcesTitle);
    stringify_case(HtmlStatisticExecutionTitle);
    stringify_case(HtmlStatisticMethodTitle);
    stringify_case(MethodStatistics);
    stringify_case(GlobalStatistic);
    stringify_case(ExecutionStatistics);
    stringify_case(SourcesStatistics);
    stringify_case(HtmlCodeFragment);
    stringify_case(Explanation);
    stringify_case(FragmentExplanation);
    stringify_case(SourceCode);
    stringify_case(Comment);
    stringify_case(HtmlExecutionState);
    stringify_case(ExecutionStateExecutionCountTooLow);
    stringify_case(ExecutionStateExecuted);
    stringify_case(ExecutionStateNone);
    stringify_case(ExecutionStateNotExecuted);
    stringify_case(ExecutionStateHidden);
    stringify_case(ExecutionStateUnknown);
    stringify_case(Line);
    stringify_case(LineFrom);
    stringify_case(LineTo);
    stringify_case(ColFrom);
    stringify_case(ColTo);
    stringify_case(HtmlLineInfo);
    stringify_case(ExecutedBy);
    stringify_case(HtmlExecutedByTitle);
    stringify_case(HtmlExecutedByList);
    stringify_case(ExecutedByItem);
    stringify_case(HtmlExecutionCountTitle);
    stringify_case(HtmlExecutionCountValue);
    stringify_case(ExecutionCount);
    stringify_case(FragmentExplanationTrueFalseTable);
    stringify_case(HtmlExplanationTrueFalseTableHeader);
    stringify_case(HtmlExplanationTrueFalseTableHeaderItem);
    stringify_case(HtmlExplanationTrueFalseTableContain);
    stringify_case(ExplanationTrueCase);
    stringify_case(ExplanationFalseCase);
    stringify_case(ExecutionStateNotInstrumented);
    stringify_case(StatusPartiallyExecuted);
    stringify_case(StatusUnexecuted);
    stringify_case(StatusExecuted);
    stringify_case(StatusManuallyMarktAsExecuted);
    stringify_case(StatusUnderLicense);
    stringify_case(StatusUnknown);
    stringify_case(StatusNone);
    stringify_case(StatusHidden);
    stringify_case(CodeFragment);
    stringify_case(HtmlCodeFragmentsEmpty);
    stringify_case(CodeFragmentItemSource);
    stringify_case(CodeFragmentItem);
    stringify_case(HtmlCodeFragmentItemExplanationItemOdd);
    stringify_case(CodeFragmentItemExplanationItem);
    stringify_case(HtmlCodeFragmentItemExplanationItemEven);
    stringify_case(CodeFragmentItemExplanation);
    stringify_case(HtmlTocLevel1);
    stringify_case(HtmlTocLevel1Item);
    stringify_case(HtmlTocLevel2);
    stringify_case(HtmlTocLevel2Entry);
    stringify_case(HtmlTocLinkExecution);
    stringify_case(HtmlTocLinkGlobalCoverage);
    stringify_case(HtmlTocLinkManuallyValidated);
    stringify_case(HtmlTocLinkMethod);
    stringify_case(HtmlTocLinkSource);
    stringify_case(HtmlTocLinkUnexecuted);
    stringify_case(HtmlTocLinkExecuted);
    stringify_case(HtmlItemEven);
    stringify_case(HtmlItemOdd);
    stringify_case(Item);
    stringify_case(ItemFunction);
    stringify_case(ItemTitle);
    stringify_case(HtmlTableFormat);
    stringify_case(ItemStatistic);
    stringify_case(ItemStatisticLevel);
    stringify_case(HtmlItemTitle);
    stringify_case(ItemStatisticValue);
    stringify_case(ItemSource);
    stringify_case(ExecutionName);
    stringify_case(ItemTitleLevel);
    stringify_case(HtmlTableFormatScopedName);
    stringify_case(HtmlTableFormatSourceRelative);
    stringify_case(HtmlTableFormatPrototype);
    stringify_case(HtmlTableFormatExecutionStatus);
    stringify_case(ItemFunctionScopedName);
    stringify_case(ItemFunctionSource);
    stringify_case(ItemFunctionPrototype);
  }
  return QString();
}

Writer & Writer::operator<<(const QString &data)
{
  if (ignoreOutput())
    return *this;
  _write(data);
  return *this;
}

Writer & Writer::operator<<(long data)
{
  if (ignoreOutput())
    return *this;
  _write((int)data);
  return *this;
}

Writer & Writer::operator<<(int data)
{
  if (ignoreOutput())
    return *this;
  _write(data);
  return *this;
}

Writer & Writer::operator<<(double data)
{
  if (ignoreOutput())
    return *this;
  _write(data);
  return *this;
}
