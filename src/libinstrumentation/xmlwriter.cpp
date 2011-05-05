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

#include "xmlwriter.h"
#include "csmes.h"
#include <QTextDocument>
#include <QDateTime>

void XMLWriter::initTags()
{
  excludeTag(HtmlItemOdd);
  excludeTag(HtmlItemEven);
  excludeTag(HtmlStatisticValue);
  excludeTag(HtmlCodeFragmentItemExplanationItemOdd);
  excludeTag(HtmlCodeFragmentItemExplanationItemEven);
  excludeTag(HtmlInformationCoverageMethodValue);
  excludeTag(HtmlInformationCoverageModeValue);
  excludeTag(HtmlExecutionState);
  excludeTag(HtmlExecutionCountValue);
  excludeTag(HtmlExecutedByList);
  excludeTag(CodeFragmentItemExplanationItem);
  excludeTag(HtmlExplanationTrueFalseTableContain);

  excludeContain(StatusExecuted);
  excludeContain(StatusUnderLicense);
  excludeContain(StatusUnknown);
  excludeContain(StatusNone);
  excludeContain(StatusPartiallyExecuted);
  excludeContain(StatusManuallyMarktAsExecuted);
  excludeContain(StatusUnexecuted);
  excludeContain(StatusHidden);
  excludeContain(ExecutionStateNotInstrumented);
  excludeContain(ExecutionStateNotExecuted);
  excludeContain(ExecutionStateExecutionCountTooLow);
  excludeContain(ExecutionStateExecuted);
  excludeContain(ExecutionStateHidden);
  excludeContain(ExecutionStateNone);
  excludeContain(ExecutionStateUnknown);
  excludeContain(ExecutionStateUnderLicense);

  excludeTagAndContain(Note);
  excludeTagAndContain(HtmlExecutedByTitle);
  excludeTagAndContain(HtmlExplanationTrueFalseTableHeader);
  excludeTagAndContain(ItemTitle);
  excludeTagAndContain(HtmlFragmentsUnexecutedTitle);
  excludeTagAndContain(HtmlFragmentsExecutedTitle);
  excludeTagAndContain(HtmlExecutionCountTitle);
  excludeTagAndContain(HtmlExecutionListTitle);
  excludeTagAndContain(HtmlExecutionListHeader);
  excludeTagAndContain(HtmlGlobalCoverage);
  excludeTagAndContain(HtmlStatisticSourcesTitle);
  excludeTagAndContain(HtmlStatisticMethodTitle);
  excludeTagAndContain(HtmlStatisticExecutionTitle);
  excludeTagAndContain(HtmlFragmentsManuallyValidatedTitle);
  excludeTagAndContain(HtmlLineInfo);
  excludeTagAndContain(HtmlSourceCode);
  excludeTagAndContain(HtmlTocLevel1);
  excludeTagAndContain(HtmlInformation);
  excludeTagAndContain(HtmlInformationCoverageMethodDescription);
  excludeTagAndContain(HtmlInformationCoverageModeDescription);
  excludeTagAndContain(HtmlLevel);
  excludeTagAndContain(Line);
}

void XMLWriter::_begin(section_type_t s)
{
  stream.writeStartElement(sectionName(s));
}

void XMLWriter::_end(section_type_t)
{
  stream.writeEndElement();
}

void XMLWriter::_write(const QString &data)
{
  stream.writeCharacters(data);
}

void XMLWriter::_write(int data)
{
  stream.writeCharacters(QString::number(data));
}

void XMLWriter::_write(double data)
{
  switch(getNumericFormat())
  {
    case PERCENT:
      if (data>=0)
        stream.writeCharacters(QString::number(data));
      else
        stream.writeCharacters("-");
      break;
    case NUMERIC:
      stream.writeCharacters(QString::number(data));
      break;
    case BARGRAPH:
      break;
  }
}

void XMLWriter::beginBody()
{
  stream.setAutoFormatting(true);
  stream.writeStartDocument();
  stream.writeStartElement("TestCocoonReport");
}

void XMLWriter::endBody()
{
  stream.writeEndElement();
  stream.writeEndDocument();
}

void XMLWriter::beginHeader()
{
}

void XMLWriter::endHeader()
{
}

void XMLWriter::writeLine()
{
}


void XMLWriter::insertHtml(const QString &t)
{
  stream.writeCharacters(t);
}

