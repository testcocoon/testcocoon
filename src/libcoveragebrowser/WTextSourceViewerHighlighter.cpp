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

#include <QtGui>

#include "coveragebrowserpdef.h"
#include "WTextSourceViewerHighlighter.h"
#include "TextSourceInstrumentationData.h"

WTextSourceViewerHighlighter::WTextSourceViewerHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
  init();
}

void WTextSourceViewerHighlighter::init()
{
  HighlightingRule rule;
  QString pattern;
  highlightingRules.clear();

  operatorFormat.setForeground(QColor(0x70,0x00,0x70));
  QStringList keywordOperators;
  keywordOperators 
    << "abstract"
    << "as"
    << "_*asm_*"
    << "__attribute__"
    << "base"
    << "bool"
    << "break"
    << "byte"
    << "case"
    << "catch"
    << "char"
    << "checked"
    << "class"
    << "const"
    << "const_cast"
    << "continue"
    << "decimal"
    << "default"
    << "delegate"
    << "delete"
    << "do"
    << "double"
    << "dynamic_cast"
    << "else"
    << "enum"
    << "event"
    << "explicit"
    << "__extension__"
    << "extern"
    << "false"
    << "finally"
    << "fixed"
    << "float"
    << "for"
    << "foreach"
    << "goto"
    << "if"
    << "implicit"
    << "in"
    << "inline"
    << "int"
    << "interface"
    << "internal"
    << "is"
    << "lock"
    << "long"
    << "namespace"
    << "new"
    << "null"
    << "object"
    << "operator"
    << "out"
    << "override"
    << "params"
    << "partial"
    << "private"
    << "protected"
    << "public"
    << "readonly"
    << "ref"
    << "register"
    << "return"
    << "sbyte"
    << "sealed"
    << "short"
    << "signed"
    << "sizeof"
    << "stackalloc"
    << "static"
    << "static_cast"
    << "string"
    << "struct"
    << "switch"
    << "template"
    << "this"
    << "throw"
    << "true"
    << "try"
    << "typedef"
    << "typeid"
    << "typename"
    << "typeof"
    << "uint"
    << "ulong"
    << "unchecked"
    << "union"
    << "unsafe"
    << "unsigned"
    << "ushort"
    << "using"
    << "virtual"
    << "void"
    << "_*volatile_*"
    << "while"
    ;
  QString pattern_rule;
  foreach (pattern, keywordOperators) 
  {
     if (!pattern_rule.isEmpty())
        pattern_rule += "|" ;
     pattern_rule += "\\b"+pattern+"\\b" ;
  }
  keywordOperators.clear();
  keywordOperators << "[+-*/!<>=]=" << "[=,<>;!]" << "->" << "::" << "[()]" << "\\[" << "\\]" << "\\." << "\\*" ;
  foreach (pattern, keywordOperators) 
  {
     if (!pattern_rule.isEmpty())
        pattern_rule += "|" ;
     pattern_rule += pattern ;
  }
  rule.pattern = QRegExp(pattern_rule);
  rule.format = operatorFormat;
  highlightingRules.append(rule);

  cppFormat.setForeground(QColor(0x70,0x00,0x70));
  cppFormat.setFontWeight(QFont::Bold);
  QStringList keywordCppRegExp ;
  keywordCppRegExp << "throw" << "break" << "case" << "continue" << "default" << "do" << "else" << "for" << "goto" << "if" << "try" << "catch" << "return" << "switch" << "while" ;

  pattern_rule.clear();
  foreach (pattern, keywordCppRegExp) 
  {
     if (!pattern_rule.isEmpty())
        pattern_rule += "|" ;
     pattern_rule += "\\b"+pattern+"\\b" ;
  }
  keywordCppRegExp.clear(); 
  keywordCppRegExp << "\\|\\|" << "&&" ;
  foreach (pattern, keywordCppRegExp) 
  {
     if (!pattern_rule.isEmpty())
        pattern_rule += "|" ;
     pattern_rule += pattern ;
  }
  rule.pattern = QRegExp(pattern_rule);
  rule.format = cppFormat;
  highlightingRules.append(rule);

  keywordHighlightFormat.setFontWeight(QFont::Bold);
  QStringList keywordsHighlight ;
  keywordsHighlight << "{" << "}"   ;
  foreach (pattern, keywordsHighlight) 
  {
    rule.pattern = QRegExp(pattern,Qt::CaseSensitive,QRegExp::FixedString);
    rule.format = keywordHighlightFormat;
    highlightingRules.append(rule);
  }

  preprocessorFormat.setForeground(QColor(0x40,0x40,0x80));

  singleLineCommentFormat.setForeground(Qt::gray);
  singleLineCommentFormat.setFontItalic(true);
  rule.pattern = QRegExp("//[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);

  multiLineCommentFormat.setForeground(Qt::gray);
  multiLineCommentFormat.setFontItalic(true);

  quotationFormat.setForeground(QColor(0x40,0x40,0x00));
  rule.pattern = QRegExp("\"([^\"]|\\\\\")*\"");
  rule.format = quotationFormat;
  highlightingRules.append(rule);

  commentStartExpression = QRegExp("/\\*");
  commentEndExpression = QRegExp("\\*/");
  preprocessorStartExpression = QRegExp("^\\s*#");
  preprocessorEndExpression = QRegExp("[^\\\\]$");
}

void WTextSourceViewerHighlighter::highlightBlock(const QString &text)
{
  // Highlight instrumentation
  QTextBlockUserData *user_data=currentBlockUserData();
  TextSourceInstrumentationData *instrumentationData=dynamic_cast<TextSourceInstrumentationData*>(user_data);
  if (instrumentationData)
  {
    if (instrumentationData->line_mode)
    {
      bool selected=false;
      Instrumentation::status_t status=Instrumentation::STATUS_NONE;
      for (QVector<TextSourceInstrumentationData::InstrumentationData>::const_iterator it=instrumentationData->instrumentations.begin();
          it!=instrumentationData->instrumentations.end();++it)
      {
        status=Instrumentation::combine(status,
            (*it).status(instrumentationData->coverage_level,instrumentationData->coverage_method));
        selected= selected || (*it).selected();
      }

      if ( (status!=Instrumentation::STATUS_NONE) )
      {
        QBrush background_format=Instrumentation::statusBackgroundColor(status,selected);
        QTextCharFormat f;
        f.setBackground(background_format);
        setFormat(0,text.length(),f);
      }
    }
    else
    {
      for (QVector<TextSourceInstrumentationData::InstrumentationData>::const_iterator it=instrumentationData->instrumentations.begin();
          it!=instrumentationData->instrumentations.end();++it)
      {
        Instrumentation::status_t status= (*it).status(instrumentationData->coverage_level,instrumentationData->coverage_method);

        if ( (status!=Instrumentation::STATUS_NONE) )
        {
          QBrush background_format=Instrumentation::statusBackgroundColor(status,(*it).selected());
          QTextCharFormat f;
          f.setBackground(background_format);
          int start_column;
          int end_column;
          int current_line=instrumentationData->current_line;
          if ((*it).startLinePre()!=current_line)
            start_column=0;
          else
            start_column=(*it).startColumnPre()-1;

          if (current_line!=(*it).endLinePre())
            end_column=text.length()-start_column;
          else
            end_column=(*it).endColumnPre()-1;

          setFormat(start_column,end_column-start_column,f);
        }
      }
    }
  }

  // Syntax highlighting for keywords
  foreach (HighlightingRule rule, highlightingRules) 
  {
    QRegExp expression(rule.pattern);
    int index = text.indexOf(expression);
    while (index >= 0) 
    {
      int length = expression.matchedLength();
      ASSERT(length>0);
      mergeFormat(index, length, rule.format);
      index = text.indexOf(expression, index + length);
    }
  }
  setCurrentBlockState(ST_NONE);

  int startCommentIndex ;
  int startPreprocessorIndex ;
  state_t previous_state = static_cast<state_t>(previousBlockState());
  switch (previous_state)
  {
    default:
    case ST_NONE:
      startCommentIndex = text.indexOf(commentStartExpression);
      startPreprocessorIndex = text.indexOf(preprocessorStartExpression);
      break;
    case ST_COMMENT:
      startCommentIndex = 0;
      startPreprocessorIndex = text.indexOf(preprocessorStartExpression);
      break;
    case ST_PREPROCESSOR:
      startCommentIndex = -1;
      startPreprocessorIndex = 0;
      break;
  }

  // Syntax highlighting for comments
  while (startCommentIndex >= 0) 
  {
    int endCommentIndex = text.indexOf(commentEndExpression, startCommentIndex);
    int commentLength;
    if (endCommentIndex == -1) 
    {
      setCurrentBlockState(ST_COMMENT);
      commentLength = text.length() - startCommentIndex;
    }
    else 
    {
      commentLength = endCommentIndex - startCommentIndex
        + commentEndExpression.matchedLength();
    }
    mergeFormat(startCommentIndex, commentLength, multiLineCommentFormat);
    startCommentIndex = text.indexOf(commentStartExpression,
        startCommentIndex + commentLength);
  }
  
  // Syntax highlighting for preprocessors
  while (startPreprocessorIndex >= 0) 
  {
    int endPreprocessorIndex = text.indexOf(preprocessorEndExpression, startPreprocessorIndex);
    int preprocessorLength;
    if (endPreprocessorIndex == -1) 
    {
      setCurrentBlockState(ST_PREPROCESSOR);
      preprocessorLength = text.length() - startPreprocessorIndex;
    }
    else 
    {
      preprocessorLength = endPreprocessorIndex - startPreprocessorIndex
        + preprocessorEndExpression.matchedLength();
    }
    mergeFormat(startPreprocessorIndex, preprocessorLength, preprocessorFormat);
    startPreprocessorIndex = text.indexOf(preprocessorStartExpression,
        startPreprocessorIndex + preprocessorLength);
  }
}


void WTextSourceViewerHighlighter::mergeFormat(int start,int count,const FormatRule &rule)
{
  QTextCharFormat f=format(start);
  rule.mergeFormat(f);
  setFormat(start, count, f);
}
