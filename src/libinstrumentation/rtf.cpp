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

#include "rtf.h"
#include <math.h>
#include <qregexp.h>
#include <QTextDocument>
#include <QTextBlock>
#include <QBitArray>
#include <QTextCursor>
#include <QTextDocumentFragment> 
#include <QColor>
#include <QFont>
#include "TextSourceInstrumentationData.h"

int RTF::numberOfLines(const QString &input)
{
  return input.count("<p>",Qt::CaseInsensitive);
}

void RTF::CPPSyntax (const QString &input,QString &output)
{
  static QRegExp CPPKeywords ( "&amp;&amp;|\\|\\||(\\b(throw|break|case|continue|default|do|else|for|goto|if|try|catch|return|switch|while)\\b)" ) ;
  static QRegExp RTFTag ( "<[^>]*>" ) ;
  static QRegExp CPPComments ( "(//.*(?=</p>)|/\\*.*\\*/)" ) ;
  static QRegExp CPPString ( "(\"([^\"\\\\\\n\\r]|\\\\[^\\n\\r])*\")" ) ;
  static QRegExp CPPOperators("\\b(struct|union|delete|typedef|namespace|class|new|template|using|typename|typeid|static|static_cast|const_cast|dynamic_cast|__extension__|volatile|float|bool|short|signed|unsigned|__volatile|__volatile__|private|protected|public|inline|register|extern|virtual|this|const|extern|register|volatile|void|int|long|char|asm|__asm__|__asm|operator|__attribute__)\\b") ; 
  static QRegExp CPPPreprocessor("<p[^>]*>[ \t]*#.*</p>") ; 
  CPPKeywords.setMinimal(true);
  RTFTag.setMinimal(true);
  CPPComments.setMinimal(true);
  CPPString.setMinimal(true);
  CPPOperators.setMinimal(true);
  CPPPreprocessor.setMinimal(true);
  int pos1=0;
  int pos2=0;
  int pos_keyword=-2;
  int pos_preprocessor=-2;
  int pos_comment=-2;
  int pos_operator=-2;
  int pos_rtftag=-2;
  int pos_string=-2;
  output="";
  int reserve = 3*input.length()+1;
  output.reserve(reserve);

  do
  {
    if (pos_string!=-1 && pos_string<pos2)
      pos_string=CPPString.indexIn(input,pos2);
    if (pos_keyword!=-1 && pos_keyword<pos2)
      pos_keyword=CPPKeywords.indexIn(input,pos2);
    if (pos_comment!=-1 && pos_comment<pos2)
      pos_comment=CPPComments.indexIn(input,pos2);
    if (pos_operator!=-1 && pos_operator<pos2)
      pos_operator=CPPOperators.indexIn(input,pos2);
    if (pos_rtftag!=-1 && pos_rtftag<pos2)
      pos_rtftag=RTFTag.indexIn(input,pos2);
    if (pos_preprocessor!=-1 && pos_preprocessor<pos2)
    {
      pos_preprocessor=CPPPreprocessor.indexIn(input,pos2);
      if (pos_preprocessor>=0)
        pos_preprocessor+=3;
    }
    pos2=-1;
    if (pos_preprocessor!=-1) pos2=pos_preprocessor;
    if (pos_rtftag!=-1) pos2=pos_rtftag;
    if (pos_string!=-1) pos2=pos_string;
    if (pos_operator!=-1) pos2=pos_operator;
    if (pos_comment!=-1) pos2=pos_comment;
    if (pos_keyword!=-1) pos2=pos_keyword;

    if (pos_preprocessor!=-1 && pos_preprocessor<pos2) pos2=pos_preprocessor;
    if (pos_rtftag!=-1 && pos_rtftag<pos2) pos2=pos_rtftag;
    if (pos_string!=-1 && pos_string<pos2) pos2=pos_string;
    if (pos_operator!=-1 && pos_operator<pos2) pos2=pos_operator;
    if (pos_comment!=-1 && pos_comment<pos2) pos2=pos_comment;
    if (pos_keyword!=-1 && pos_keyword<pos2) pos2=pos_keyword;

    if ( (pos_keyword!=-1) 
        && (pos_keyword==pos2) )
    { // keyword detected
      QString orig=CPPKeywords.cap();
#if 0
      QString replace="<B><FONT color=\"#600060\">"+orig+"</FONT></B>";
#else
      QString replace="<font color=\"#800080\">"+orig+"</font>";
#endif
      output+=input.mid(pos1,pos2-pos1);
      output+=replace;
      pos2+=orig.length();
      pos1=pos2;
    }
    else if ( (pos_preprocessor!=-1) 
        && (pos_preprocessor==pos2) )
    { // preprocessor detected
      QString orig=CPPPreprocessor.cap();
      int lg=orig.length()-7;
      orig=orig.mid(3,lg);
      QString replace="<FONT color=\"#404080\">"+orig+"</FONT>";
      output+=input.mid(pos1,pos2-pos1);
      output+=replace;
      pos2+=lg;
      pos1=pos2;
    }
    else if ( (pos_string!=-1) 
        && (pos_string==pos2) )
    { // string detected
      QString orig=CPPString.cap();
      QString replace=orig;
      replace.replace("</p><p>","</FONT></p><p><FONT color=\"#404000\">");
      replace="<FONT color=\"#404000\">"+replace+"</FONT>";
      output+=input.mid(pos1,pos2-pos1);
      output+=replace;
      pos2+=orig.length();
      pos1=pos2;
    }
    else if ( (pos_operator!=-1) 
        && (pos_operator==pos2) )
    { // operator detected
      QString orig=CPPOperators.cap();
#if 0
      QString replace="<B>"+orig+"</B>";
#else
      QString replace="<font color=\"#600060\">"+orig+"</font>";
#endif
      output+=input.mid(pos1,pos2-pos1);
      output+=replace;
      pos2+=orig.length();
      pos1=pos2;
    }
    else if ( (pos_comment!=-1) 
        && (pos_comment==pos2) )
    { // comment detected
      QString orig=CPPComments.cap();
      QString replace=orig;
      replace.replace("</p><p>","</FONT></p><p><FONT color=\"#808080\">");
      replace="<FONT color=\"#808080\">"+replace+"</FONT>";
      output+=input.mid(pos1,pos2-pos1);
      output+=replace;
      pos2+=orig.length();
      pos1=pos2;
    }
    else if ( (pos_rtftag!=-1) 
        && (pos_rtftag==pos2) )
    { // HTML Tag detected
      pos2+=RTFTag.cap().length();
      output+=input.mid(pos1,pos2-pos1);
      pos1=pos2;
    }
    else
      pos2=-1;

  } while (pos2!=-1) ;
  output+=input.mid(pos1,input.length()-pos1);
}


void RTF::return2paragraph(QString &input)
{
  input.replace("\n","</p><p>",Qt::CaseInsensitive);
  input.replace("<pre>","<pre><p>",Qt::CaseInsensitive);
  input.replace("</pre>","</p></pre>",Qt::CaseInsensitive);
}

void RTF::hideLines(const QBitArray &hidden_lines,const QTextDocument *in,QTextDocument *&output)
{
  if (in==NULL)
  {
    output=NULL;
    return;
  }

  int sz=hidden_lines.size();
  QTextDocument *input=const_cast<QTextDocument*>(in);
  output=new QTextDocument();
  QTextCursor cur(input);
  cur.movePosition(QTextCursor::Start);
  QTextBlock textBlock;
  int index=0;
  for (textBlock = in->begin(); textBlock!=in->end() ;textBlock=textBlock.next())
  {
    QTextBlockUserData *user_data=textBlock.userData();
    TextSourceInstrumentationData *instrumentation_data=dynamic_cast<TextSourceInstrumentationData*>(user_data);
    if (instrumentation_data)
    {
      index=instrumentation_data->current_line-1;
      ASSERT(index>=0);
    }
      
    if ( (index >= sz) || !hidden_lines[index])
    {
      QTextCursor in_cur(textBlock);
      in_cur.movePosition(QTextCursor::StartOfBlock);
      in_cur.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
      QTextCursor out_cur(output);
      out_cur.movePosition(QTextCursor::End);
      out_cur.insertFragment(in_cur.selection());
      if (instrumentation_data)
      {
        TextSourceInstrumentationData  *new_instrumentation_data=new TextSourceInstrumentationData;
        *new_instrumentation_data=*instrumentation_data;
        out_cur.block().setUserData(new_instrumentation_data);
      }
      out_cur.insertBlock();
    }
    index++;
  }
}

void RTF::toTableFormat(QString &text)
{
  text.replace("<p>","<table width=\"100%\" border=0 cellpadding=0 cellspacing=\"-1\"><tr><td bgcolor=\"yellow\"><pre><p>",Qt::CaseInsensitive);
  text.replace("</p>","</p></pre></td></tr></table>",Qt::CaseInsensitive);
}

QString RTF::extractLine(const QString &text,int line)
{
  /* extract the line */
  QString output=QString::null;
  int pos1=0;
  int cur_line=1;
  while (pos1>=0)
  {
    pos1=text.indexOf("<p>",pos1,Qt::CaseInsensitive);
    if (pos1>=0) 
    {
      if (cur_line==line)
      {
        int pos2=0;
        pos2=text.indexOf("</p>",pos2,Qt::CaseInsensitive);
        output+=text.mid(pos1,pos2+4-pos1);
        break;
      }
      else
        pos1++;
      cur_line++;
    }
  }

  return output;
}

QString RTF::toPlain(const QString &text)
{
  static QRegExp HTMLTag ( "<[^>]*>" ) ;
  static QRegExp BRTag ( "</br>|</p>" ) ;
  BRTag.setCaseSensitivity(Qt::CaseInsensitive);
  BRTag.setMinimal(true);
  HTMLTag.setMinimal(true);

  QString output=text;
  output.replace(BRTag,"\n");
  output.replace(HTMLTag,"");

  return output;
}

void RTF::setDefaultFormating(const QFont &font,QTextDocument *document)
{
  QTextCursor cur(document);
  cur.movePosition(QTextCursor::Start);
  cur.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
  QTextBlockFormat block_format;
  QTextCharFormat char_format;
#if 0
  char_format.setFont(font);
#else
  char_format.setFontFamily(font.family());
  char_format.setFontPointSize(font.pointSizeF());
#endif
  block_format.setNonBreakableLines(true);
  block_format.setLeftMargin(0);
  block_format.setTopMargin(0);
  block_format.setBottomMargin(0);
  block_format.setTextIndent(0);
  block_format.setIndent(0);
  cur.mergeBlockFormat(block_format);
  cur.mergeCharFormat(char_format);
}
