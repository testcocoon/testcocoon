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

#ifndef WTextSourceViewerHighlighter_H
#define WTextSourceViewerHighlighter_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

#include <QTextDocument>

class WTextSourceViewerHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    WTextSourceViewerHighlighter(QTextDocument *parent = 0);

    void init();

protected:
    void highlightBlock(const QString &text);

private:
    enum state_t
    {
      ST_NONE         = -1,
      ST_COMMENT      =  1,
      ST_PREPROCESSOR =  2
    } ;

    class FormatRule
    {
      public:
        FormatRule()
        {
          modify_font_size=false;
          modify_foreground_color=false;
          modify_background_color=false;
          modify_italic=false;
          modify_font_weight=false;
        }

        void setForeground(const QBrush &brush)
        {
          modify_foreground_color=true;
          format.setForeground(brush);
        }
        void setFontWeight(int weight)
        {
          modify_font_weight=true;
          format.setFontWeight(weight);
        }
        void setFontItalic(bool italic)
        {
          modify_italic=true;;
          format.setFontItalic(italic);
        }
        void mergeFormat(QTextCharFormat &f) const
        {
          if (modify_font_size)
            f.setFontPointSize(format.fontPointSize());
          if (modify_foreground_color)
            f.setForeground(format.foreground());
          if (modify_background_color)
            f.setBackground(format.background());
          if (modify_italic)
            f.setFontItalic(format.fontItalic());
          if (modify_font_weight)
            f.setFontWeight(format.fontWeight());
        }
      private:
        QTextCharFormat format;
        bool modify_font_size;
        bool modify_foreground_color;
        bool modify_background_color;
        bool modify_italic;
        bool modify_font_weight;
    };
    struct HighlightingRule
    {
        QRegExp pattern;
        FormatRule format;
    };

    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;
    QRegExp preprocessorStartExpression;
    QRegExp preprocessorEndExpression;

    FormatRule preprocessorFormat;
    FormatRule operatorFormat;
    FormatRule cppFormat;
    FormatRule keywordHighlightFormat;
    FormatRule singleLineCommentFormat;
    FormatRule multiLineCommentFormat;
    FormatRule quotationFormat;

    inline void mergeFormat(int start,int count,const FormatRule &f);
};

#endif
