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

#include "htmlwriter.h"
#include "csmes.h"
#include <QTextDocument>
#include <QDateTime>

void HTMLWriter::initTags()
{
  excludeTagAndContain(ItemStatisticLevel);
  excludeTagAndContain(FromLineOrg);
  excludeTagAndContain(FromLinePre);
  excludeTagAndContain(FromColumnPre);
  excludeTagAndContain(ToLineOrg);
  excludeTagAndContain(ToLinePre);
  excludeTagAndContain(ToColumnPre);
  excludeTag(ItemStatisticValue);

  styles.clear();
  tags.clear();
  no_ident_tags.clear();

  styles.insert(HtmlItemTitle,QStringList() << "font-weight:bold" << "background-color:#AAAAAA");
  styles.insert(HtmlItemOdd,QStringList() << "background-color:#EEEEEE");
  styles.insert(HtmlItemEven,QStringList() << "background-color:#FFFFFF");
  styles.insert(ExecutionListStatusUnknown,QStringList() << "background-color:" + CSMes::executionStatusColor(Executions::EXECUTION_STATUS_UNKNOWN).name() );
  styles.insert(ExecutionListStatusPassed,QStringList() << "background-color:" + CSMes::executionStatusColor(Executions::EXECUTION_STATUS_PASSED).name() );
  styles.insert(ExecutionListStatusFailed,QStringList() << "background-color:" + CSMes::executionStatusColor(Executions::EXECUTION_STATUS_FAILED).name() );
  styles.insert(ExecutionListStatusToBeCheckManually,QStringList() << "background-color:" + CSMes::executionStatusColor(Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY).name() );
  styles.insert(Line,QStringList() << "color:"+COL_DARK_GRAY.name() << "font-style:italic" );

  no_ident_this_section=false;
  section_count=0;
  tags.insert(HtmlSource,QStringList() << "TT");
  tags.insert(MethodStatistics,QStringList() << "TABLE" );
  tags.insert(GlobalStatistic,QStringList() << "TABLE" );
  tags.insert(ExecutionStatistics,QStringList() << "TABLE" );
  tags.insert(SourcesStatistics,QStringList() << "TABLE" );
  tags.insert(HtmlTocLevel1,QStringList() << "UL" );
  tags.insert(HtmlTocLevel1Item,QStringList() << "LI");
  tags.insert(HtmlTocLevel2,QStringList() << "UL" );
  tags.insert(HtmlTocLevel2Entry,QStringList() << "LI");
  tags.insert(HtmlItemTitle,QStringList() << "TR" );
  tags.insert(HtmlItemOdd,QStringList() << "TR" );
  tags.insert(HtmlItemEven,QStringList() << "TR" );
  tags.insert(HtmlTocLinkManuallyValidated,QStringList() << "A" << "href=\"#manually_validated\"");
  tags.insert(HtmlTocLinkUnexecuted,QStringList() << "A" << "href=\"#unexecuted\"");
  tags.insert(HtmlTocLinkExecuted,QStringList() << "A" << "href=\"#executed\"");
  tags.insert(HtmlTocLinkExecution,QStringList() << "A" << "href=\"#execution\"");
  tags.insert(HtmlTocLinkMethod,QStringList() << "A" << "href=\"#method\"");
  tags.insert(HtmlTocLinkSource,QStringList() << "A" << "href=\"#source\"");
  tags.insert(HtmlTocLinkGlobalCoverage,QStringList() << "A" << "href=\"#global_coverage\"");
  tags.insert(HtmlInformation,QStringList() << "H2");
  tags.insert(HtmlInformation,QStringList() << "A" << "name=\"information\"" );

  tags.insert(HtmlFragmentsExecutedTitle,QStringList() << "H2");
  tags.insert(HtmlFragmentsExecutedTitle,QStringList() << "A" << "name=\"executed\"" );
  tags.insert(HtmlFragmentsUnexecutedTitle,QStringList() << "H2");
  tags.insert(HtmlFragmentsUnexecutedTitle,QStringList() << "A" << "name=\"unexecuted\"" );

  tags.insert(HtmlFragmentsManuallyValidatedTitle,QStringList() << "H2");
  tags.insert(HtmlFragmentsManuallyValidatedTitle,QStringList() << "A" << "name=\"manually_validated\"" );

  tags.insert(HtmlStatisticExecutionTitle,QStringList() << "H2");
  tags.insert(HtmlStatisticExecutionTitle,QStringList() << "A" << "name=\"execution\"" );

  tags.insert(HtmlStatisticMethodTitle,QStringList() << "H2");
  tags.insert(HtmlStatisticMethodTitle,QStringList() << "A" << "name=\"method\"" );

  tags.insert(HtmlGlobalCoverage,QStringList() << "H2");
  tags.insert(HtmlGlobalCoverage,QStringList() << "A" << "name=\"global_coverage\"" );

  tags.insert(HtmlStatisticSourcesTitle,QStringList() << "H2");
  tags.insert(HtmlStatisticSourcesTitle,QStringList() << "A" << "name=\"source\"" );

  tags.insert(HtmlStatisticValue,QStringList() << "TABLE" << "summary=\"statistic\"");
  tags.insert(HtmlStatisticValue,QStringList() << "TR" );
  tags.insert(HtmlStatisticValue,QStringList() << "TD" );

  tags.insert(GlobalInformation,QStringList() << "TABLE" << "summary=\"global information\"");
  styles.insert(GlobalInformation,QStringList() << "border-width:0" );

  tags.insert(GlobalInformationCoverageMethod,QStringList() << "TR" );
  tags.insert(GlobalInformationCoverageMode,QStringList() << "TR" );
  tags.insert(HtmlInformationCoverageMethodDescription,QStringList() << "TD" );
  styles.insert(HtmlInformationCoverageMethodDescription,QStringList() << "font-style:italic"  );
  tags.insert(HtmlInformationCoverageMethodValue,QStringList() << "TD" );
  tags.insert(HtmlInformationCoverageModeDescription,QStringList() << "TD" );
  styles.insert(HtmlInformationCoverageModeDescription,QStringList() << "font-style:italic"  );
  tags.insert(HtmlInformationCoverageModeValue,QStringList() << "TD" );

  tags.insert(HtmlExecutionListTitle,QStringList() << "H3" );
  tags.insert(ExecutionList,QStringList() << "TABLE" << "summary=\"executions\"");
  tags.insert(HtmlExecutionListHeader,QStringList() << "TR" );
  tags.insert(HtmlExecutionListHeaderItem,QStringList() << "TD" );
  styles.insert(HtmlExecutionListHeaderItem,QStringList() << "background-color:#AAAAAA" << "font-weight:bold" );
  tags.insert(ExecutionListItem,QStringList() << "TR" );
  tags.insert(ExecutionListName,QStringList() << "TD" );
  tags.insert(HtmlExecutionListContainComment,QStringList() << "BLOCKQUOTE" );
  tags.insert(ExecutionListStatusUnknown,QStringList() << "TD" );
  tags.insert(ExecutionListStatusPassed,QStringList() << "TD" );
  tags.insert(ExecutionListStatusFailed,QStringList() << "TD" );
  tags.insert(ExecutionListStatusToBeCheckManually,QStringList() << "TD" );

  tags.insert(Line,QStringList() << "TD" );

  tags.insert(SourceCode,QStringList() << "CODE" );
  tags.insert(FragmentExplanation,QStringList() << "TD" );
  tags.insert(HtmlCodeFragment,QStringList() << "PRE" );
  tags.insert(CodeFragment,QStringList() << "UL" );
  tags.insert(HtmlCodeFragmentsEmpty,QStringList() << "SPAN" );
  styles.insert(HtmlCodeFragmentsEmpty,QStringList() << "font-style:italic"  );
  tags.insert(CodeFragmentItem,QStringList() << "LI" );
  tags.insert(CodeFragmentItemSource,QStringList() << "DIV" );
  tags.insert(CodeFragmentItemExplanation,QStringList() << "TABLE" << "summary=\"explanation\"");
  tags.insert(HtmlCodeFragmentItemExplanationItemOdd,QStringList() << "TR" );
  styles.insert(HtmlCodeFragmentItemExplanationItemOdd,QStringList() << "background-color:#EEEEEE");
  tags.insert(HtmlCodeFragmentItemExplanationItemEven,QStringList() << "TR" );
  styles.insert(HtmlCodeFragmentItemExplanationItemEven,QStringList() << "background-color:#FFFFFF");

  tags.insert(Note,QStringList() << "DIV" );
  tags.insert(NoteTitle,QStringList() << "SPAN" );
  styles.insert(NoteTitle,QStringList() << "font-weight:bold" );

  tags.insert(HtmlExecutionState,QStringList() << "CENTER"  );
  tags.insert(ExecutionStateUnderLicense,QStringList() << "SPAN" );
  styles.insert(ExecutionStateUnderLicense,QStringList() << "color:"+COL_MAGENTA.name() );
  tags.insert(ExecutionStateExecutionCountTooLow,QStringList() << "SPAN" );
  styles.insert(ExecutionStateExecutionCountTooLow,QStringList() << "color:"+COL_ORANGE.name() );
  tags.insert(ExecutionStateExecuted,QStringList() << "SPAN" );
  styles.insert(ExecutionStateExecuted,QStringList() << "color:"+COL_GREEN.name() );
  tags.insert(ExecutionStateNotExecuted,QStringList() << "SPAN" );
  styles.insert(ExecutionStateNotExecuted,QStringList() << "color:"+COL_RED.name() );
  tags.insert(ExecutionStateNone,QStringList() << "SPAN" );
  styles.insert(ExecutionStateNone,QStringList() << "color:"+COL_GRAY.name() );
  tags.insert(ExecutionStateHidden,QStringList() << "SPAN" );
  styles.insert(ExecutionStateHidden,QStringList() << "color:"+COL_GRAY.name() );

  tags.insert(StatusUnexecuted,QStringList() << "SPAN" );
  styles.insert(StatusUnexecuted,QStringList() << "color:"+COL_RED.name() );
  tags.insert(StatusPartiallyExecuted,QStringList() << "SPAN" );
  styles.insert(StatusPartiallyExecuted,QStringList() << "color:"+COL_ORANGE.name() );
  tags.insert(StatusManuallyMarktAsExecuted,QStringList() << "SPAN" );
  styles.insert(StatusManuallyMarktAsExecuted,QStringList() << "color:"+COL_BLUE.name() );
  tags.insert(StatusExecuted,QStringList() << "SPAN" );
  styles.insert(StatusExecuted,QStringList() << "color:"+COL_GREEN.name() );
  tags.insert(StatusUnderLicense,QStringList() << "SPAN" );
  styles.insert(StatusUnderLicense,QStringList() << "color:"+COL_MAGENTA.name() );
  tags.insert(StatusUnknown,QStringList() << "SPAN" );
  styles.insert(StatusUnknown,QStringList() << "color:"+COL_GRAY.name() );
  tags.insert(StatusNone,QStringList() << "SPAN" );
  styles.insert(StatusNone,QStringList() << "color:"+COL_GRAY.name() );
  tags.insert(Comment,QStringList() << "BLOCKQUOTE" );
  styles.insert(Comment,QStringList() << "background-color:"+COL_YELLOW_LIGHT.name());
  tags.insert(FragmentExplanationTrueFalseTable,QStringList() << "TABLE" << "summary=\"true false table\"" << "border=\"1\"");
  styles.insert(FragmentExplanationTrueFalseTable,QStringList() << "border-color:#000" << "border-spacing:0px" << "padding:0px" );
  tags.insert(HtmlExplanationTrueFalseTableHeader,QStringList() << "TR" );
  tags.insert(HtmlExplanationTrueFalseTableHeaderItem,QStringList() << "TH" );
  styles.insert(HtmlExplanationTrueFalseTableHeaderItem,QStringList() << "text-align:center"<< "border:1px solid" << "border-color:#333" << "border-spacing:0px" << "padding:1px" );
  tags.insert(HtmlExplanationTrueFalseTableContain,QStringList() << "TR" );
  tags.insert(ExplanationTrueCase,QStringList() << "TD" );
  styles.insert(ExplanationTrueCase,QStringList() << "border:1px solid" << "border-spacing:0px" << "border-color:#333" << "padding:1px" );
  tags.insert(ExplanationFalseCase,QStringList() << "TD" );
  styles.insert(ExplanationFalseCase,QStringList() << "border:1px solid" << "border-spacing:0px" << "border-color:#333" << "padding:1px" );
  tags.insert(ExecutionCount,QStringList() << "DIV" );
  tags.insert(HtmlExecutionCountTitle,QStringList() << "SPAN" );
  styles.insert(HtmlExecutionCountTitle,QStringList() << "font-style:italic" );
  tags.insert(HtmlExecutionCountValue,QStringList() << "SPAN" );
  styles.insert(HtmlExecutionCountValue,QStringList() << "font-family:monospace" );
  tags.insert(ExecutedBy,QStringList() << "DIV" );
  tags.insert(HtmlExecutedByTitle,QStringList() << "SPAN" );
  styles.insert(HtmlExecutedByTitle,QStringList() << "font-style:italic" );
  tags.insert(HtmlExecutedByList,QStringList() << "UL" );
  tags.insert(ExecutedByItem,QStringList() << "LI" );
  styles.insert(ExecutedByItem,QStringList() << "font-family:monospace" );

  tags.insert( ItemFunction , QStringList() << "TD" );
  tags.insert(ItemTitle, QStringList() << "TD" );
  styles.insert(ItemTitle,QStringList() << "white-space:nowrap" );
  tags.insert(HtmlTableFormat, QStringList() << "TD" );
  styles.insert(HtmlTableFormat,QStringList() << "white-space:nowrap" );
  tags.insert(ItemStatistic, QStringList() << "TD" );
  styles.insert(ItemStatistic,QStringList() << "white-space:nowrap" );
  tags.insert(ItemSource, QStringList() << "TD" );
  styles.insert(ItemSource,QStringList() << "white-space:nowrap" );
  tags.insert(ExecutionName, QStringList() << "TD" );
  styles.insert(ExecutionName,QStringList() << "white-space:nowrap" );
  tags.insert(ItemTitleLevel, QStringList() << "TD" );
  styles.insert(ItemTitleLevel,QStringList() << "white-space:nowrap" );
  tags.insert(HtmlTableFormatScopedName, QStringList() << "TD" );
  styles.insert(HtmlTableFormatScopedName,QStringList() << "white-space:nowrap" );
  tags.insert(HtmlTableFormatSourceRelative, QStringList() << "TD" );
  tags.insert(HtmlTableFormatPrototype, QStringList() << "TD" );
  tags.insert(HtmlTableFormatExecutionStatus, QStringList() << "TD" );
  styles.insert(HtmlTableFormatExecutionStatus,QStringList() << "white-space:nowrap" );

  tags.insert(ItemFunctionScopedName,QStringList() << "DIV" );
  styles.insert(ItemFunctionScopedName,QStringList() << "margin:0" << "padding:0" << "font-size:normal" << "font-family:sans-serif" );
  tags.insert(ItemFunctionSource,QStringList() << "DIV" );
  styles.insert(ItemFunctionSource,QStringList() << "margin:0" << "padding:0"<< "font-size:xx-small" << "font-family:monospace" );
  tags.insert(ItemFunctionPrototype,QStringList() << "DIV" );
  styles.insert(ItemFunctionPrototype,QStringList() << "margin:0" << "padding:0"<< "font-size:xx-small" << "font-stretch:ultra-condensed" << "font-family:monospace" );

  tags.insert(CoverageBrowserExplanationTable,QStringList() << "TABLE" <<"width=\"100%\"" << "border=\"0\"" << "cellspacing=\"0\"" << "cellpadding=\"0\"" );

  no_ident_tags.insert(HtmlTocLinkManuallyValidated);
  no_ident_tags.insert(SourceCode);
  no_ident_tags.insert(HtmlTocLinkExecution);
  no_ident_tags.insert(HtmlTocLinkExecuted);
  no_ident_tags.insert(HtmlTocLinkSource);
  no_ident_tags.insert(HtmlTocLinkUnexecuted);
  no_ident_tags.insert(HtmlTocLinkGlobalCoverage);
  no_ident_tags.insert(HtmlTocLinkMethod);

  no_class.clear();

  setDisplayFooter(true);
}

inline QList<QStringList> HTMLWriter::tag(section_type_t s) const
{
  if (tags.contains(s))
    return tags.values(s);
  return QList<QStringList> () ;
}

void HTMLWriter::_begin(section_type_t s)
{
  QList<QStringList> values=tag(s); 
  if (!values.empty())
    indent_begin(s); 
  QList<QStringList>::const_iterator itValues=values.end();
  while (itValues!=values.begin())
  {
    --itValues;
    stream << "<" ;
    bool first=true;
    for (QStringList::const_iterator it=(*itValues).begin();it!=(*itValues).end();++it)
    {
      ASSERT(! (*it).contains('>'));
      if (!first)
        stream << " " ;
      stream << *it;
      first=false;
    }
    if (!no_class.contains(s))
      stream << " " << "class=\"" << sectionName(s) << "\"" ;
    stream  << ">" ;
  }
  no_ident_this_section=false;
}

void HTMLWriter::_end(section_type_t s)
{
  QList<QStringList> values=tag(s); 
  if (!values.empty())
    indent_end(s); 
  for (QList<QStringList>::const_iterator itValues=values.begin();itValues!=values.end();++itValues)
  {
    stream << "</" << (*itValues).first() << ">" ;
  }
}

void HTMLWriter::writeLine()
{
  stream << "<BR>"  ;
}

void HTMLWriter::_write(const QString &data)
{
  no_ident_this_section=true;
  stream<<Qt::escape(data);
}

void HTMLWriter::_write(int data)
{
  no_ident_this_section=true;
  stream<<data;
}

void HTMLWriter::bargraph(QColor c,double value_d)
{
  stream << "<table summary=\"bargraph\" style=\"border:2px solid "<< c.name() << "; width:100px;\" cellspacing=\"0\" cellpadding=\"0\"><tr style=\"border:0px solid "<< c.name() << ";\" >";
  stream << "<td style=\"height:15px;border:0px solid "<< c.name() << ";width:" << static_cast<int>(value_d*100) << "px;\" bgcolor=\""<<c.name()<<"\" >";
  stream << "</td><td style=\"height:15px;border:0px solid "<< c.name() << "; width:" << static_cast<int>(100-value_d*100) << "px;\" bgcolor=\"#CCCCCC\"></td>" ;
  stream << "</tr></table>";
  stream << "</td><td>";
}

void HTMLWriter::_write(double data)
{
  no_ident_this_section=true;
  switch(getNumericFormat())
  {
    case NUMERIC:
      stream<<data;
      break;
    case PERCENT:
      stream<<QString::number(data*100,5,2) <<Qt::escape("%");
      break;
    case BARGRAPH:
      bargraph(getColor(),data);
      break;
  }
}

void HTMLWriter::beginBody()
{
  stream << "<body>" ; 
  writeLineFormating() ;
  stream << "<h1>"<< getTitle() << "</h1>" ; 
  writeLineFormating() ;
}

void HTMLWriter::endBody()
{
  if (displayFooter())
  {
    stream << "<HR>" ; 
    writeLineFormating() ;
    stream << "<center>" ; 
    writeLineFormating() ;
    stream << "<font size=\"1\">" ;
    stream << "Generated by <a href=\"http://www.testcocoon.org\">TestCocoon</a>";
    stream << "</font>" ; 
    writeLineFormating() ;
    stream << "</center>" ; 
    writeLineFormating() ;
  }
  stream << "</body>" ; 
  writeLineFormating() ;
  stream << "</html>" ; 
  writeLineFormating() ;
}

void HTMLWriter::beginHeader()
{
  stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/transitional.dtd\">" ; 
  writeLineFormating() ;
  stream << "<html>" ; 
  writeLineFormating() ;
  stream << "<head>" ; 
  writeLineFormating() ;
  stream << "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">" ; 
  writeLineFormating() ;
  stream << "<meta name=\"description\" content=\"Code Coverage Report Generated by TestCocoon\">" ; 
  writeLineFormating() ;
}

void HTMLWriter::endHeader()
{
  if (!getIconFile().isEmpty())
  {
    stream << "<link rel=\"shortcut icon\" href=\""<< getIconFile() <<"\" type=\"image/x-icon\">" ; 
    writeLineFormating() ;
    stream << "<link rel=\"icon\" href=\""<< getIconFile() <<"\" type=\"image/x-icon\">" ; 
    writeLineFormating() ;
  }
  stream << "<title>"<< getTitle() << "</title>" ; 
  writeLineFormating() ;
  if (!getCssFile().isEmpty())
  {
    stream << "<link rel=\"stylesheet\" type=\"text/css\" href=\""<< getCssFile() << "\">" ; 
    writeLineFormating() ;
  }
  else
  {
    stream << "<style type=\"text/css\">" ;
    writeLineFormating() ;
    for (QMap<section_type_t,QStringList>::const_iterator it=styles.begin();it!=styles.end();++it)
    {
      QString html_class=sectionName(it.key());
      QStringList attributes=it.value();
      stream << "." << html_class << " {";
      for (QStringList::const_iterator itAttribute=attributes.begin();itAttribute!=attributes.end();++itAttribute)
      {
        stream << *itAttribute << "; ";
      }
      stream << "}" ;
      writeLineFormating() ;
    }
    stream << "</style>" ;
    writeLineFormating() ;
  }
  stream << "</head>" ; 
  writeLineFormating() ;
}

void HTMLWriter::indent_begin(section_type_t s)
{
  if (no_ident_tags.contains(s))
    return;

  stream << endl ;
  for (int i=0;i<section_count ; i++)
    stream << '\t' ;
  section_count++;
}

void HTMLWriter::indent_end(section_type_t s)
{
  if (no_ident_tags.contains(s))
    return;

  section_count--; 

  if (no_ident_this_section)
  {
    no_ident_this_section=false;
    return ;
  }
  stream << endl ;
  for (int i=0;i<section_count ; i++)
    stream << '\t' ;

}

void HTMLWriter::insertHtml(const QString &t)
{
  stream << t;
}

