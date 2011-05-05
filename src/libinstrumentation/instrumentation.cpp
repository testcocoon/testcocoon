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

#include "instrumentation.h"
#include <qobject.h>
#include <QTextDocument>
#include "writer.h"

QBrush Instrumentation::statusBackgroundColor(status_t status,bool selected)
{
   QColor c=Instrumentation::statusColor(status);
   QBrush background_format;
   if (selected)
      background_format.setStyle(Qt::SolidPattern);
   else
      background_format.setStyle(INSTRUMENTATION_BACKGROUND_STYLE);
   background_format.setColor(c.light(INSTRUMENTATION_BACKGROUND_LIGHTING));
   return background_format ;
}
        
QColor Instrumentation::statusColor(status_t t)
{
  switch(t)
  {
    case STATUS_MANUALLY_VALIDATED :
      return COL_BLUE;
    case STATUS_VALIDATED:
      return COL_GREEN;
    case STATUS_UNVALIDATED:
      return COL_RED;
    case STATUS_PARTIALLY_VALIDATED:
      return COL_ORANGE;
    case STATUS_NONE:
      return COL_WHITE;
    case STATUS_HIDDEN:
      return COL_GRAY;
    case STATUS_UNKNOWN:
      return COL_GRAY;
  }
  return COL_GRAY;
}

Instrumentation::status_t Instrumentation::status(int level,coverage_method_t method) const 
{
  status_t ret=STATUS_UNVALIDATED;
  ASSERT(level>0);
  switch(getType())
  {
    case NOP:
      ret=STATUS_NONE;
      break;
    case CONDITION_TRUE_ONLY:
    case CONDITION_FALSE_ONLY:
      if (method==COVERAGE_BRANCH)
      {
        ret=STATUS_NONE;
        break;
      }
    case LABEL:
    case COMPUTATION:
      if (execution[0]==EXECUTION_STATE_HIDDEN)
        ret= STATUS_HIDDEN;
      else if (execution[0]==EXECUTION_STATE_NONE)
        ret= STATUS_NONE;
      else if (execution[0]==EXECUTION_STATE_UNKNOWN)
        ret= STATUS_UNKNOWN;
      else if (execution[0]>level-1)
        ret= STATUS_VALIDATED;
      else
        ret= STATUS_UNVALIDATED;
      break;
    case CONDITION_FULL:
      if (method==COVERAGE_BRANCH)
        ret=STATUS_NONE;
      else if ( (execution[0]>level-1) && (execution[1]==EXECUTION_STATE_NONE || execution[1]==EXECUTION_STATE_HIDDEN) )
        ret =  STATUS_PARTIALLY_VALIDATED;
      else if ( (execution[1]>level-1) && (execution[0]==EXECUTION_STATE_NONE || execution[0]==EXECUTION_STATE_HIDDEN) )
        ret =  STATUS_PARTIALLY_VALIDATED;
      else if ( (execution[0]==EXECUTION_STATE_HIDDEN) && (execution[1]==EXECUTION_STATE_NONE || execution[1]==EXECUTION_STATE_HIDDEN) )
        ret =  STATUS_HIDDEN;
      else if ( (execution[0]==EXECUTION_STATE_NONE) && execution[1]==EXECUTION_STATE_HIDDEN )
        ret =  STATUS_HIDDEN;
      else if ( (execution[0]==EXECUTION_STATE_NONE) && execution[1]==EXECUTION_STATE_NONE )
        ret =  STATUS_NONE;
      else if (execution[0]==EXECUTION_STATE_UNKNOWN)
        ret= STATUS_UNKNOWN;
      else if (execution[1]==EXECUTION_STATE_UNKNOWN)
        ret= STATUS_UNKNOWN;
      else if ( (execution[0]>level-1) &&  (execution[1]>level-1) )
        ret =  STATUS_VALIDATED;
      else if ( (execution[0]>level-1) ||  (execution[1]>level-1) )
        ret =  STATUS_PARTIALLY_VALIDATED;
      else
        ret =  STATUS_UNVALIDATED;
      break;
    case TYPE_NULL:
      ASSERT(false);
      break;
  }
  if ( (ret != STATUS_VALIDATED) && manually_validated )
    ret = STATUS_MANUALLY_VALIDATED;
    
  return ret;
}


void Instrumentation::setExecution(execution_state_t executed)
{
  int m=getMaxIndex();
  for (int i=getMinIndex();i<=m;i++)
    setExecution(i,executed);
}

static void executedByHTML(Writer &stream,const QStringList &executedBy,int executed_by_limit)
{
  if (!executedBy.isEmpty() && executed_by_limit!=0)
  {
    WriterSection sec(stream,Writer::ExecutedBy);
    {
      WriterSection sec(stream,Writer::HtmlExecutedByTitle);
      stream << QObject::tr("Executed by:");
    }
    {
      WriterSection sec(stream,Writer::HtmlExecutedByList);
      for (QStringList::const_iterator exeit=executedBy.begin();
          exeit!=executedBy.end();
          ++exeit)
      {
        WriterSection sec(stream,Writer::ExecutedByItem);
        if (executed_by_limit==0)
        {
           stream << "..." ;
           break;
        }
        stream << *exeit ;
        executed_by_limit--;
      }
    }
  }
}

static void executionCountHTML(Writer &stream,int count)
{
  if (count>=0)
  {
    WriterSection sec(stream,Writer::ExecutionCount);
    {
      WriterSection sec(stream,Writer::HtmlExecutionCountTitle);
      stream << QObject::tr( "Execution Count:");
    }
    {
      WriterSection sec(stream,Writer::HtmlExecutionCountValue);
      stream << QString::number(count);
    }
  }
}

QString Instrumentation::explanationPlainText(const QString &source_code,int level,coverage_method_t method) const
{
  if (getType()==NOP)
    return QString();
  if (method==COVERAGE_BRANCH && getType()!=COMPUTATION)
    return QObject::tr("Condition coverage are hidden");

  QString message;
  QTextStream stream(&message);
  status_t st=status(level,method);
  switch(st)
  {
    case STATUS_UNVALIDATED:
      {
        if (executionCountMax()>0)
          stream << QObject::tr("execution count too low");
        else
          stream << QObject::tr("never executed");
      }
      break;
    case STATUS_PARTIALLY_VALIDATED:
      {
        stream << QObject::tr("partially executed");
      }
      break;
    case STATUS_MANUALLY_VALIDATED:
      {
        stream << QObject::tr("manually set to be executed");
      }
      break;
    case STATUS_VALIDATED:
      {
        stream << QObject::tr("executed");
      }
      break;
    case STATUS_HIDDEN :
      {
        stream << QObject::tr("status hidden");
      }
      break;
    case STATUS_UNKNOWN :
      {
        stream << QObject::tr("unknown execution status");
      }
      break;
    case STATUS_NONE:
      {
        stream << "???";
      }
      break;
  }
  {
    stream << ":";
    stream << " ";
  }
  {
    stream << "'";
    stream << source_code;
    stream << "' ";
  }
  switch(st)
  {
    case STATUS_UNVALIDATED:
      if (getType()==CONDITION_FALSE_ONLY || getType()==CONDITION_TRUE_ONLY)
        ;
      else if (executionCountMax()<=0)
        break;
    case STATUS_MANUALLY_VALIDATED:
    case STATUS_PARTIALLY_VALIDATED:
    case STATUS_VALIDATED:
      switch (getType())
      { 
        case CONDITION_FALSE_ONLY:
          if (st==STATUS_MANUALLY_VALIDATED)
            ;
          else if (execution[0]>0 && execution[0]<level)
            stream << QObject::tr("was false but execution count too low");
          else if (execution[0]>0)
            ;
          else if (execution[0]==EXECUTION_STATE_NONE)
            ;
          else if (execution[0]==EXECUTION_STATE_HIDDEN)
            ;
          else
            stream << QObject::tr("was not false");
          break;
        case CONDITION_TRUE_ONLY:
          if (st==STATUS_MANUALLY_VALIDATED)
            ;
          else if (execution[0]>0 && execution[0]<level)
            stream << QObject::tr("was true but execution count too low");
          else if (execution[0]>0)
            ;
          else if (execution[0]==EXECUTION_STATE_HIDDEN)
            ;
          else if (execution[0]==EXECUTION_STATE_NONE)
            ;
          else
            stream << QObject::tr("was not true");
          break;
        case CONDITION_FULL:
          if (st==STATUS_PARTIALLY_VALIDATED )
          {
            bool use_and=false;
            if (st==STATUS_MANUALLY_VALIDATED)
              ;
            else if (execution[0]>0 && execution[0]<level)
            {
              stream << QObject::tr("was true but execution count too low");
              use_and=true;
            }
            else if (execution[0]>0)
              ;
            else if (execution[0]==EXECUTION_STATE_NONE)
              ;
            else if (execution[0]==EXECUTION_STATE_HIDDEN)
              ;
            else
            {
              stream << QObject::tr("was not true");
              use_and=true;
            }
            if (st==STATUS_MANUALLY_VALIDATED)
              ;
            else if (execution[1]>0 && execution[1]<level)
            {
              if (use_and)
                stream << " and ";
              stream << QObject::tr("was false but execution count too low");
            }
            else if (execution[1]>0)
              ;
            else if (execution[1]==EXECUTION_STATE_NONE)
              ;
            else if (execution[1]==EXECUTION_STATE_HIDDEN)
              ;
            else
            {
              if (use_and)
                stream << " and ";
              stream << QObject::tr("was not false");
            }
          }
          break;
        default:
          break;
      }
      break;
    case STATUS_UNKNOWN :
    case STATUS_HIDDEN :
    case STATUS_NONE:
      break;
  }
  return message;
}

void Instrumentation::explanation(Writer &stream,const QString &source_code,const QString &comment,const QStringList executedBy[2],int level,coverage_method_t method,int executed_by_limit) const
{
  if (getType()==NOP)
    return ;
  if (method==COVERAGE_BRANCH && getType()!=COMPUTATION)
  {
    stream << QObject::tr("Condition coverage are hidden");
    return ;
  }
  stream.begin(Writer::Explanation);

  stream.begin(Writer::FromLineOrg)   ; stream << startLineOrg()   ; stream.end(Writer::FromLineOrg)   ; 
  stream.begin(Writer::FromLinePre)   ; stream << startLinePre()   ; stream.end(Writer::FromLinePre)   ; 
  stream.begin(Writer::FromColumnPre) ; stream << startColumnPre() ; stream.end(Writer::FromColumnPre) ; 
  stream.begin(Writer::ToLineOrg)     ; stream << endLineOrg()     ; stream.end(Writer::ToLineOrg)     ; 
  stream.begin(Writer::ToLinePre)     ; stream << endLinePre()     ; stream.end(Writer::ToLinePre)     ; 
  stream.begin(Writer::ToColumnPre)   ; stream << endColumnPre()   ; stream.end(Writer::ToColumnPre)   ; 

  status_t st=status(level,method);
  switch(st)
  {
    case STATUS_UNVALIDATED:
      {
        WriterSection sec(stream,Writer::StatusUnexecuted);
        if (executionCountMax()>0)
          stream << QObject::tr("execution count too low");
        else
          stream << QObject::tr("never executed");
      }
      break;
    case STATUS_PARTIALLY_VALIDATED:
      {
        WriterSection sec(stream,Writer::StatusPartiallyExecuted);
        stream << QObject::tr("partially executed");
      }
      break;
    case STATUS_MANUALLY_VALIDATED:
      {
        WriterSection sec(stream,Writer::StatusManuallyMarktAsExecuted);
        stream << QObject::tr("manually set to be executed");
      }
      break;
    case STATUS_VALIDATED:
      {
        WriterSection sec(stream,Writer::StatusExecuted);
        stream << QObject::tr("executed");
      }
      break;
    case STATUS_HIDDEN :
      {
        WriterSection sec(stream,Writer::StatusHidden);
        stream << QObject::tr("status hidden");
      }
      break;
    case STATUS_UNKNOWN :
      {
        WriterSection sec(stream,Writer::StatusUnknown);
        stream << QObject::tr("unknown execution status");
      }
      break;
    case STATUS_NONE:
      {
        WriterSection sec(stream,Writer::StatusNone);
        stream << "???";
      }
      break;
  }
  {
    WriterSection source_code_sec1(stream,Writer::HtmlSourceCode);
    stream << ":";
    stream << " ";
  }
  {
    WriterSection source_code_sec2(stream,Writer::SourceCode);
    stream << source_code;
  }
  switch(st)
  {
    case STATUS_UNVALIDATED:
      if (getType()==CONDITION_FALSE_ONLY || getType()==CONDITION_TRUE_ONLY)
        ;
      else if (executionCountMax()<=0)
        break;
    case STATUS_MANUALLY_VALIDATED:
    case STATUS_PARTIALLY_VALIDATED:
    case STATUS_VALIDATED:
      switch (getType())
      { 
        case CONDITION_FALSE_ONLY:
          {
            stream.begin(Writer::FragmentExplanationTrueFalseTable);
            {
              WriterSection sec_true_false_header(stream,Writer::HtmlExplanationTrueFalseTableHeader);
              {
                WriterSection sec_true_false_header_item(stream,Writer::HtmlExplanationTrueFalseTableHeaderItem);
                stream << QObject::tr("TRUE");
              }
              {
                WriterSection sec_true_false_header_item(stream,Writer::HtmlExplanationTrueFalseTableHeaderItem);
                stream << QObject::tr("FALSE");
              }
            }
            {
              WriterSection sec_true_false_contain(stream,Writer::HtmlExplanationTrueFalseTableContain);
              {
                WriterSection sec_true_false_contain(stream,Writer::ExplanationTrueCase);
                {
                  WriterSection sec_not_instrumented(stream,Writer::ExecutionStateNotInstrumented);
                  stream << QObject::tr("Not Instrumented");
                }
              }
              {
                WriterSection sec_true_false_contain(stream,Writer::ExplanationFalseCase);
                if (st==STATUS_MANUALLY_VALIDATED)
                {
                  if (execution[0]!=EXECUTION_STATE_UNKNOWN)
                  {
                    ASSERT(executedBy[0].count()<=execution[0]);
                    executionCountHTML(stream,execution[0]);
                    executedByHTML(stream,executedBy[0],executed_by_limit);
                  }
                }
                else if (execution[0]>0 && execution[0]<level)
                {
                  ASSERT(executedBy[0].count()<=execution[0]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateExecutionCountTooLow);
                    stream << QObject::tr("execution count too low");
                  }
                  executionCountHTML(stream,execution[0]);
                  executedByHTML(stream,executedBy[0],executed_by_limit);
                }
                else if (execution[0]>0)
                {
                  ASSERT(executedBy[0].count()<=execution[0]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateExecuted);
                    stream << QObject::tr("yes");
                  }
                  executionCountHTML(stream,execution[0]);
                  executedByHTML(stream,executedBy[0],executed_by_limit);
                }
                else if (execution[0]==EXECUTION_STATE_NONE)
                {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateNone);
                  stream << "-";
                }
                else if (execution[0]==EXECUTION_STATE_HIDDEN)
                {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateHidden);
                  stream << "-";
                }
                else
                {
                  ASSERT(executedBy[0].count()<=execution[0]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateNotExecuted);
                    stream << QObject::tr("no");
                  }
                  executionCountHTML(stream,execution[0]);
                  executedByHTML(stream,executedBy[0],executed_by_limit);
                }
              }
            }
            stream.end(Writer::FragmentExplanationTrueFalseTable);
          }
          break;
        case CONDITION_TRUE_ONLY:
          {
            stream.begin(Writer::FragmentExplanationTrueFalseTable);
            {
              WriterSection sec_true_false_header(stream,Writer::HtmlExplanationTrueFalseTableHeader);
              {
                WriterSection sec_true_false_header_item(stream,Writer::HtmlExplanationTrueFalseTableHeaderItem);
                stream << QObject::tr("TRUE");
              }
              {
                WriterSection sec_true_false_header_item(stream,Writer::HtmlExplanationTrueFalseTableHeaderItem);
                stream << QObject::tr("FALSE");
              }
            }
            {
              WriterSection sec_true_false_contain(stream,Writer::HtmlExplanationTrueFalseTableContain);
              {
                WriterSection sec_true_false_contain(stream,Writer::ExplanationTrueCase);
                if (st==STATUS_MANUALLY_VALIDATED)
                {
                  if (execution[0]!=EXECUTION_STATE_UNKNOWN)
                  {
                    ASSERT(executedBy[0].count()<=execution[0]);
                    executionCountHTML(stream,execution[0]);
                    executedByHTML(stream,executedBy[0],executed_by_limit);
                  }
                }
                else if (execution[0]>0 && execution[0]<level)
                {
                  ASSERT(executedBy[0].count()<=execution[0]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateExecutionCountTooLow);
                    stream << QObject::tr("execution count too low");
                  }
                  executionCountHTML(stream,execution[0]);
                  executedByHTML(stream,executedBy[0],executed_by_limit);
                }
                else if (execution[0]>0)
                {
                  ASSERT(executedBy[0].count()<=execution[0]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateExecuted);
                    stream << QObject::tr("yes");
                  }
                  executionCountHTML(stream,execution[0]);
                  executedByHTML(stream,executedBy[0],executed_by_limit);
                }
                else if (execution[0]==EXECUTION_STATE_HIDDEN)
                {
                  WriterSection sec1(stream,Writer::HtmlExecutionState);
                  WriterSection sec2(stream,Writer::ExecutionStateHidden);
                  stream << "-";
                }
                else if (execution[0]==EXECUTION_STATE_NONE)
                {
                  WriterSection sec1(stream,Writer::HtmlExecutionState);
                  WriterSection sec2(stream,Writer::ExecutionStateNone);
                  stream << "-";
                }
                else
                {
                  ASSERT(executedBy[0].count()<=execution[0]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateNotExecuted);
                    stream << QObject::tr("no");
                  }
                  executionCountHTML(stream,execution[0]);
                  executedByHTML(stream,executedBy[0],executed_by_limit);
                }
              }
              {
                WriterSection sec_true_false_contain(stream,Writer::ExplanationFalseCase);
                {
                  WriterSection sec_not_instrumented(stream,Writer::ExecutionStateNotInstrumented);
                  stream << QObject::tr("Not Instrumented");
                }
              }
            }
            stream.end(Writer::FragmentExplanationTrueFalseTable);
          }
          break;
        case CONDITION_FULL:
          if (st==STATUS_PARTIALLY_VALIDATED 
              || (!executedBy[0].isEmpty()) 
              || (!executedBy[1].isEmpty()) )
          {
            stream.begin(Writer::FragmentExplanationTrueFalseTable);
            {
              WriterSection sec_true_false_header(stream,Writer::HtmlExplanationTrueFalseTableHeader);
              {
                WriterSection sec_true_false_header_item(stream,Writer::HtmlExplanationTrueFalseTableHeaderItem);
                stream << QObject::tr("TRUE");
              }
              {
                WriterSection sec_true_false_header_item(stream,Writer::HtmlExplanationTrueFalseTableHeaderItem);
                stream << QObject::tr("FALSE");
              }
            }
            {
              WriterSection sec_true_false_contain(stream,Writer::HtmlExplanationTrueFalseTableContain);
              {
                WriterSection sec_true_false_contain(stream,Writer::ExplanationTrueCase);
                if (st==STATUS_MANUALLY_VALIDATED)
                {
                  if (execution[0]!=EXECUTION_STATE_UNKNOWN)
                  {
                    ASSERT(executedBy[0].count()<=execution[0]);
                    executionCountHTML(stream,execution[0]);
                    executedByHTML(stream,executedBy[0],executed_by_limit);
                  }
                }
                else if (execution[0]>0 && execution[0]<level)
                {
                  ASSERT(executedBy[0].count()<=execution[0]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateExecutionCountTooLow);
                    stream << QObject::tr("execution count too low");
                  }
                  executionCountHTML(stream,execution[0]);
                  executedByHTML(stream,executedBy[0],executed_by_limit);
                }
                else if (execution[0]>0)
                {
                  ASSERT(executedBy[0].count()<=execution[0]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateExecuted);
                    stream << QObject::tr("yes");
                  }
                  executionCountHTML(stream,execution[0]);
                  executedByHTML(stream,executedBy[0],executed_by_limit);
                }
                else if (execution[0]==EXECUTION_STATE_NONE)
                {
                  WriterSection sec1(stream,Writer::HtmlExecutionState);
                  WriterSection sec2(stream,Writer::ExecutionStateNone);
                  stream << "-";
                }
                else if (execution[0]==EXECUTION_STATE_HIDDEN)
                {
                  WriterSection sec1(stream,Writer::HtmlExecutionState);
                  WriterSection sec2(stream,Writer::ExecutionStateHidden);
                  stream << "-";
                }
                else
                {
                  ASSERT(executedBy[0].count()<=execution[0]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateNotExecuted);
                    stream << QObject::tr("no");
                  }
                  executionCountHTML(stream,execution[0]);
                  executedByHTML(stream,executedBy[0],executed_by_limit);
                }
              }
              {
                WriterSection sec_true_false_contain(stream,Writer::ExplanationFalseCase);
                if (st==STATUS_MANUALLY_VALIDATED)
                {
                  if (execution[1]!=EXECUTION_STATE_UNKNOWN)
                  {
                    ASSERT(executedBy[1].count()<=execution[1]);
                    executionCountHTML(stream,execution[1]);
                    executedByHTML(stream,executedBy[1],executed_by_limit);
                  }
                }
                else if (execution[1]>0 && execution[1]<level)
                {
                  ASSERT(executedBy[1].count()<=execution[1]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateExecutionCountTooLow);
                    stream << QObject::tr("execution count too low");
                  }
                  executionCountHTML(stream,execution[1]);
                  executedByHTML(stream,executedBy[1],executed_by_limit);
                }
                else if (execution[1]>0)
                {
                  ASSERT(executedBy[1].count()<=execution[1]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateExecuted);
                    stream << QObject::tr("yes");
                  }
                  executionCountHTML(stream,execution[1]);
                  executedByHTML(stream,executedBy[1],executed_by_limit);
                }
                else if (execution[1]==EXECUTION_STATE_NONE)
                {
                  WriterSection sec1(stream,Writer::HtmlExecutionState);
                  WriterSection sec2(stream,Writer::ExecutionStateNone);
                  stream << "-";
                }
                else if (execution[1]==EXECUTION_STATE_HIDDEN)
                {
                  WriterSection sec1(stream,Writer::HtmlExecutionState);
                  WriterSection sec2(stream,Writer::ExecutionStateHidden);
                  stream << "-";
                }
                else
                {
                  ASSERT(executedBy[1].count()<=execution[1]);
                  {
                    WriterSection sec1(stream,Writer::HtmlExecutionState);
                    WriterSection sec2(stream,Writer::ExecutionStateNotExecuted);
                    stream << QObject::tr("no");
                  }
                  executionCountHTML(stream,execution[1]);
                  executedByHTML(stream,executedBy[1],executed_by_limit);
                }
              }
            }
            stream.end(Writer::FragmentExplanationTrueFalseTable);
          }
          break;
        default:
        //  ASSERT(execution[0]<0 || executedBy[0].count()<=execution[0]);
          executionCountHTML(stream,execution[0]);
          executedByHTML(stream,executedBy[0],executed_by_limit);
          break;
      }
      break;
    case STATUS_UNKNOWN :
    case STATUS_HIDDEN :
    case STATUS_NONE:
      break;
  }
  if (comment!=QString::null)
  {
    WriterSection sec(stream,Writer::Comment);
    stream.insertHtml(comment);
  }
  stream.end(Writer::Explanation);
}


void Instrumentation::addEquivalent(Instrumentation *v)
{
  if (v->hasEquivalent())
    return;

  Instrumentation *tmp=equiv;
  equiv=v;
  v->equiv=tmp;
}

int Instrumentation::executionCountMax() const
{
  int count=-1;
  for (int index=getMinIndex(); index<=getMaxIndex(); index++)
    if (count<executionCount(index))
      count=executionCount(index);
  return count;
}

int Instrumentation::executionCountMin() const
{
  int count=0x7FFFFFFF;
  for (int index=getMinIndex(); index<=getMaxIndex(); index++)
    if (count>executionCount(index))
      count=executionCount(index);
  return count;
}

Instrumentation::status_t Instrumentation::combine(status_t a,status_t b) 
{ 
  status_t status=combine_intern(a,b);
  ASSERT(status==combine_intern(b,a));
  return status; 
}

Instrumentation::status_t Instrumentation::combine_intern(status_t a,status_t b)
{
  status_t ret=STATUS_UNKNOWN;
  switch (a)
  {
    case STATUS_HIDDEN:
      switch (b)
      {
        case STATUS_MANUALLY_VALIDATED:
          ret = STATUS_MANUALLY_VALIDATED;
          break;
        case STATUS_PARTIALLY_VALIDATED:
        case STATUS_VALIDATED:
          ret = STATUS_PARTIALLY_VALIDATED;;
          break;
        case STATUS_UNKNOWN :
          ret = STATUS_UNKNOWN;
          break;
        case STATUS_UNVALIDATED:
        case STATUS_NONE:
        case STATUS_HIDDEN:
          ret = STATUS_HIDDEN;
          break;
      }
      break;
    case STATUS_PARTIALLY_VALIDATED:
      switch (b)
      {
        case STATUS_UNVALIDATED:
        case STATUS_PARTIALLY_VALIDATED:
        case STATUS_MANUALLY_VALIDATED:
        case STATUS_VALIDATED:
        case STATUS_HIDDEN:
          ret = STATUS_PARTIALLY_VALIDATED;;
          break;
        case STATUS_UNKNOWN :
          ret = STATUS_UNKNOWN;
          break;
        case STATUS_NONE:
          ret = a;
          break;
      }
      break;
    case STATUS_UNVALIDATED:
      switch (b)
      {
        case STATUS_UNVALIDATED:
          ret = STATUS_UNVALIDATED;;
          break;
        case STATUS_PARTIALLY_VALIDATED:
        case STATUS_MANUALLY_VALIDATED:
        case STATUS_VALIDATED:
          ret = STATUS_PARTIALLY_VALIDATED;;
          break;
        case STATUS_HIDDEN :
          ret = STATUS_HIDDEN;
          break;
        case STATUS_UNKNOWN :
          ret = STATUS_UNKNOWN;
          break;
        case STATUS_NONE:
          ret = a;
          break;
      }
      break;
    case STATUS_VALIDATED:
      switch (b)
      {
        case STATUS_PARTIALLY_VALIDATED:
        case STATUS_UNVALIDATED:
        case STATUS_HIDDEN :
          ret = STATUS_PARTIALLY_VALIDATED;;
          break;
        case STATUS_VALIDATED:
          ret = STATUS_VALIDATED;;
          break;
        case STATUS_MANUALLY_VALIDATED:
          ret = STATUS_MANUALLY_VALIDATED;;
          break;
        case STATUS_UNKNOWN :
          ret = STATUS_UNKNOWN;
          break;
        case STATUS_NONE:
          ret = a;
          break;
      }
      break;
    case STATUS_MANUALLY_VALIDATED:
      switch (b)
      {
        case STATUS_PARTIALLY_VALIDATED:
        case STATUS_UNVALIDATED:
          ret = STATUS_PARTIALLY_VALIDATED;;
          break;
        case STATUS_VALIDATED:
          ret = STATUS_MANUALLY_VALIDATED;;
          break;
        case STATUS_HIDDEN:
        case STATUS_MANUALLY_VALIDATED:
          ret = STATUS_MANUALLY_VALIDATED;;
          break;
        case STATUS_UNKNOWN :
          ret = STATUS_UNKNOWN;
          break;
        case STATUS_NONE:
          ret = a;
          break;
      }
      break;
    case STATUS_NONE:
      ret = b;
      break;
    case STATUS_UNKNOWN :
      ret = STATUS_UNKNOWN;
      break;
  }

  return ret;
}

bool Instrumentation::valid() const
{
  return ! (
      _end_column_org    == -1 &&
      _end_column_pre    == -1 &&
      _end_line_org      == -1 &&
      _end_line_pre      == -1 &&
      _start_column_org  == -1 &&
      _start_column_pre  == -1 &&
      _start_line_org    == -1 &&
      _start_line_pre    == -1 );
}
