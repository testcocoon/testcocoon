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

#include "diffalgo.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <QList>
#include <QHash>
#include <QString>
#include <QTextFragment>
#include <QVector>
#include <stdlib.h>


enum LCSMarker 
{
  ARROW_UP      = 0,
  ARROW_LEFT    = 1,
  ARROW_UP_LEFT = 2,
  FINAL         = 3
} ;

class LCSMatrix
{
  public :
    LCSMatrix(unsigned int mX,unsigned int mY)
    {
      _nX=mX;
      _nY=mY;
      _allocated=(mX+1);
      _allocated=_allocated*(mY+1);
      _allocated=_allocated/4;
      _allocated=_allocated+1;
      if (_allocated>=0x100000000ULL)
        _values=NULL;
      else
        _values=(unsigned char*)malloc(_allocated);
    }
    ~LCSMatrix()
    {
      free(_values);
    }

    bool allocated() const { return _values!=NULL; }

    LCSMarker get(unsigned int x,unsigned int y) const 
    {
      ASSERT(x<_nX);
      ASSERT(y<_nY);
      unsigned long long pos=_nY*x+y; 
      unsigned int shift=(pos&0x3)<<1 ;
      unsigned int table_pos=(pos>>2);
      ASSERT(_allocated>table_pos);
      return (LCSMarker)( ( _values[table_pos] >> shift ) & 3 );
    }
    void set(unsigned int x,unsigned int y, LCSMarker v)
    {
      ASSERT(x<_nX);
      ASSERT(y<_nY);
      unsigned long long pos=_nY*x+y; 
      unsigned int shift=(pos&0x3)<<1 ;
      unsigned int table_pos=(pos>>2);
      ASSERT(_allocated>table_pos);
      _values[table_pos] &= ~(3 << shift);
      _values[table_pos] |= v << shift; 
    }
  private:
    
    unsigned char *_values;
    unsigned int _nX,_nY;
    unsigned long long _allocated;
};

class LCSItem
{
  private:
    int _id;
    bool _is_space;
  public:
    void setIndex(int i) { _id=i; _is_space=false;}
    void setHandleAsSpace() { _is_space=true; }
    bool handleAsSpace() const { return _is_space; }
    int index() const { return _id; }
    bool operator== ( const LCSItem & x ) const
    {
      return _id==x._id;
    }
} ;

bool wordSeparator(QChar c1,QChar c2)
{
  static QChar special[] = { '*' , '/' , '.' , '+' , '-' , '<' , '>', '!' , '=' , '~' ,'[' ,']','(',')','{','}','?',',',':',';' , '\0' } ;
  bool specialc1=false;
  bool specialc2=false;
  const QChar *sp;
  for (sp=special;*sp!='\0';sp++)
  {
    if (c1==*sp)
    {
      specialc1=true;
      break;
    }
  }
  for (sp=special;*sp!='\0';sp++)
  {
    if (c2==*sp)
    {
      specialc2=true;
      break;
    }
  }
  return specialc1!=specialc2;
}

bool onlySpaces(const QString &_text)
{
  for (QString::const_iterator it=_text.begin();it!=_text.end();++it)
  {
    if (! (*it).isSpace() )
      return false;
  }
  return true;
}

static void calcLCS(
    const QVector<LCSItem>  &X,  
    const QVector<LCSItem> &Y,  
    LCSMatrix &b   
    )
{
  int     i, j, mX, nY;
  mX = X.size()+1;
  nY = Y.size()+1;

  QVector<int> minic[2];
  minic[0].resize(nY);
  minic[1].resize(nY);

  b.set(0,0, FINAL);
  for (i=1; i<mX; i++)
  {
    b.set(i,0, ARROW_UP);
  }

  for (j=1; j<nY; j++)
  {
    minic[0][j]=0;
    b.set(0,j, ARROW_LEFT);
  }
  minic[1][0]=0;
  minic[0][0]=0;
  
  for (i=1; i<mX; i++)
  {
    int icur=i%2;
    int ilast=(i-1)%2;
    for (j=1; j<nY; j++)
    {
      int c;
      if (X.at(i-1) == Y.at(j-1)) 
      {
        c=minic[ilast][j-1] + 1;
        b.set(i,j,  ARROW_UP_LEFT);
      }
      else if (minic[ilast][j] > minic[icur][j-1])
      {
        c=minic[ilast][j];
        b.set(i,j,ARROW_UP);
      }
      else
      {
        c=minic[icur][j-1];
        b.set(i,j, ARROW_LEFT);
      }
      minic[icur][j]=c;
    }
  }
}



static QList<DiffItem> generateDiffList(const QVector<QString> &string_table,const QVector<LCSItem> &Xstripped, const QVector<LCSItem> &Ystripped, const QVector<LCSItem> &X, const QVector<LCSItem> &Y, const LCSMatrix &b)
{
  QList<DiffItem> diff_list;
  int i=X.size()-1;
  int j=Y.size()-1;
  int istripped=Xstripped.size()-1;
  int jstripped=Ystripped.size()-1;
  DiffItem diff_item;
  DiffItem::operation_t last_operation=DiffItem::SAME;
  bool finish=false;
  while (!finish)
  {
    LCSMarker depl=b.get(istripped+1,jstripped+1);
    switch (depl)
    {
      case ARROW_UP_LEFT:
        if (jstripped>=0)
        {
          while (Ystripped[jstripped].index() != Y[j].index())
          {
            if (!string_table.at(Y[j].index()).isEmpty())
            {
              diff_item.setLength(string_table.at(Y[j].index()).length());
              //diff_item.setText(string_table.at(Y[j].index()));
              diff_item.setOperation(DiffItem::SAME);
              diff_list.prepend(diff_item);
            }
            j--;
          }
          diff_item.setLength(string_table.at(Ystripped[jstripped].index()).length());
          //diff_item.setText(string_table.at(Ystripped[jstripped].index()));
          diff_item.setOperation(DiffItem::SAME);
          diff_list.prepend(diff_item);
        }
        if (istripped>=0)
        {
          while (Xstripped[istripped].index() != X[i].index())
          {
            i--;
          }
        }
        istripped--;
        jstripped--;
        i--;
        j--;
        last_operation=DiffItem::SAME;
        break;
      case ARROW_UP:
        if (istripped>=0)
        {
          while (Xstripped[istripped].index() != X[i].index())
          {
            if (!string_table.at(X[i].index()).isEmpty())
            {
              if (last_operation==DiffItem::DELETED)
              {
                diff_item.setLength(string_table.at(X[i].index()).length());
                diff_item.setText(string_table.at(X[i].index()));
                diff_item.setOperation(DiffItem::DELETED);
                diff_list.prepend(diff_item);
              }
            }
            i--;
          }
          diff_item.setLength(string_table.at(Xstripped[istripped].index()).length());
          diff_item.setText(string_table.at(Xstripped[istripped].index()));
          diff_item.setOperation(DiffItem::DELETED);
          diff_list.prepend(diff_item);
        }
        istripped--;
        i--;
        last_operation=DiffItem::DELETED;
        break;
      case ARROW_LEFT:
        if (jstripped>=0)
        {
          while (Ystripped[jstripped].index() != Y[j].index())
          {
            if (!string_table.at(Y[j].index()).isEmpty())
            {
              diff_item.setLength(string_table.at(Y[j].index()).length());
              //diff_item.setText(string_table.at(Y[j].index()));
              if (last_operation==DiffItem::INSERTED)
                diff_item.setOperation(DiffItem::INSERTED);
              else
                diff_item.setOperation(DiffItem::SAME);
              diff_list.prepend(diff_item);
            }
            j--;
          }
          diff_item.setLength(string_table.at(Ystripped[jstripped].index()).length());
          //diff_item.setText(string_table.at(Ystripped[jstripped].index()));
          diff_item.setOperation(DiffItem::INSERTED);
          diff_list.prepend(diff_item);
        }
        jstripped--;
        j--;
        last_operation=DiffItem::INSERTED;
        break;
      default:
      case FINAL:
        finish=true;
        break;
    }
  }

  while (j>=0)
  {
    if (!string_table.at(Y[j].index()).isEmpty())
    {
      diff_item.setLength(string_table.at(Y[j].index()).length());
      //diff_item.setText(string_table.at(Y[j].index()));
      diff_item.setOperation(DiffItem::SAME);
      diff_list.prepend(diff_item);
    }
    j--;
  }
  return diff_list;
}


static void splitPolicyChar(
  QVector<LCSItem> &X,
  QVector<LCSItem> &Y,
  QVector<QString> & string_vector,
  const QString &str1,
  const QString &str2
  )
{
  int lg1=str1.length();
  int lg2=str2.length();

  X.resize(lg1);
  Y.resize(lg2);
  string_vector.resize(256);

  int i;
  for (i=0;i<256;i++)
  {
    QChar c(i);
    QString str;
    str+=c;
    string_vector[i]=str;
  }

  for (i=0;i<lg1;i++)
    X[i].setIndex(str1[i].toAscii());
  for (i=0;i<lg2;i++)
    Y[i].setIndex(str2[i].toAscii());
}


static void splitPolicyWord(
  QVector<LCSItem> &X,
  QVector<LCSItem> &Y,
  QVector<QString> & string_vector,
  const QString &str1,
  const QString &str2,
  bool ignore_comments)
{
  int lg1=str1.length();
  int lg2=str2.length();

  X.resize(lg1+1+2);
  Y.resize(lg2+1+2);
  string_vector.resize(lg1+lg2+2);

  QHash<QString,int> string_table;
  int index=0;

  for (int istr=0;istr<2;istr++)
  {
    QVector<LCSItem> *T;
    const QString *str;
    int lg;
    if (istr==1)
    {
      T=&Y;
      str=&str2;
      lg=lg2;
    }
    else
    {
      T=&X;
      str=&str1;
      lg=lg1;
    }
    int istring=0;
    QString extracted;
    QChar last_char='\0';
    bool in_string=false;
    bool c_comment=false;
    bool cpp_comment=false;
    bool last_comment=c_comment || cpp_comment;
    bool comment=last_comment;
    for (int i=0;i<lg;i++)
    {
      QChar c=str->at(i);
      bool record=false;
      ASSERT((in_string && !(c_comment || cpp_comment)) || !in_string);
      ASSERT((!in_string && (c_comment || cpp_comment)) || !(c_comment || cpp_comment));
      if (!in_string)
      {
        if (last_char=='/' && c=='/')
          cpp_comment=ignore_comments;
        else if (last_char=='/' && c=='*')
          c_comment=ignore_comments;
      }
      last_comment=comment;
      comment=c_comment || cpp_comment;
      if (!comment)
      {
        if (last_char!='\\' && c=='"')
          in_string=!in_string;
      }
     
      if (last_comment!=comment)
        record=true;
      else if (comment)
        record=false;
      else if (last_char.isSpace() == c.isSpace())
      {
        if (c.isSpace())
          record=false;
        else if (wordSeparator(last_char,c))
          record=true;
      }
      else
        record=true;

      if (record)
      {
        int id=string_table.value(extracted,index);
        if (id==index)
        {
          string_vector[index]=extracted;
          string_table[extracted]=index++;
        }
        (*T)[istring].setIndex(id);
        if (comment || onlySpaces(extracted))
        {
          if (!in_string)
            (*T)[istring].setHandleAsSpace();
        }
        istring++;
        extracted.clear();
      }

      if (!in_string)
      {
        if (last_char=='*' && c=='/')
          c_comment=false;
        if (c=='\n')
          cpp_comment=false;
      }
      extracted+=c;
      last_char=c;
    }
    if (!extracted.isEmpty())
    {
      int id=string_table.value(extracted,index);
      if (id==index)
      {
        string_vector[index]=extracted;
        string_table[extracted]=index++;
      }
      (*T)[istring].setIndex(id);
      if (c_comment || cpp_comment || onlySpaces(extracted))
      {
        if (!in_string)
          (*T)[istring].setHandleAsSpace();
      }
      istring++;
    }
    T->resize(istring);
  }
}
#define DIFF_END_HANDLING 1
#define DIFF_START_HANDLING 1

void calcDiff(QList<DiffItem> &diff,const QString &str1,const QString &str2,DiffPolicy policy)
{
  QVector<LCSItem> X;
  QVector<LCSItem> Y;
  QVector<LCSItem> Xstripped;
  QVector<LCSItem> Ystripped;
  QVector<QString> string_vector;
  diff.clear();
  switch (policy)
  {
    case DiffChar:
      splitPolicyChar(X,Y,string_vector,str1,str2);
      break;
    case DiffWord:
    case DiffWordIgnoreSpaces:
      splitPolicyWord(X,Y,string_vector,str1,str2,false);
      break;
    case DiffWordIgnoreSpacesAndComments:
      splitPolicyWord(X,Y,string_vector,str1,str2,true);
      break;
  }

  int i;
#if DIFF_START_HANDLING
  // Start handling
  QVector<LCSItem> Start;
  int lg=qMin(X.size(),Y.size());
  Start.resize(lg);
  for (i=0;i<lg;i++)
  {
    if (X[i]==Y[i])
      Start[i]=X[i];
    else
      break;
  }
  Start.resize(i);
  if (i>0)
  {
    X.remove(0,i);
    Y.remove(0,i);
  }
#endif

#if DIFF_END_HANDLING
  // End handling
  int Xlg=X.size();
  QVector<LCSItem> End;
  int Ylg=Y.size();
  lg=qMin(Xlg,Ylg);
  Ylg--; Xlg--;
  End.resize(lg);
  for (i=0;i<lg;i++)
  {
    if (X[Xlg-i]==Y[Ylg-i])
      End[i]=X[Xlg-i];
    else
      break;
  }
  int EndLg=i;
  End.resize(EndLg);
  if (EndLg>0)
  {
    X.remove(X.size()-EndLg,EndLg);
    Y.remove(Y.size()-EndLg,EndLg);
  }
#endif

  switch (policy)
  {
    case DiffChar:
    case DiffWord:
      Xstripped=X;
      Ystripped=Y;
      break;
    case DiffWordIgnoreSpacesAndComments:
    case DiffWordIgnoreSpaces:
      {
        QVector<LCSItem>::const_iterator it;
        for (it=X.begin();it!=X.end();++it)
        {
          if ( ! (*it).handleAsSpace() )
            Xstripped.append(*it);
        }
        for (it=Y.begin();it!=Y.end();++it)
        {
          if ( ! (*it).handleAsSpace() )
            Ystripped.append(*it);
        }
      }
      break;
  }

  int lgX=Xstripped.size();
  int lgY=Ystripped.size();

  diff.clear();
  LCSMatrix *b = new LCSMatrix (lgX+1,lgY+1);
  if (b->allocated())
  {
    calcLCS( Xstripped, Ystripped, *b ); 
    QList<DiffItem> diff_list=generateDiffList(string_vector,Xstripped,Ystripped,X,Y,*b);
    delete b;
    b=NULL;
#if DIFF_START_HANDLING
    for (i=Start.size()-1;i>=0;i--)
    {
      if (!string_vector.at(Start[i].index()).isEmpty())
      {
        DiffItem diff_item;
        diff_item.setLength(string_vector.at(Start[i].index()).length());
        diff_item.setOperation(DiffItem::SAME);
        diff_list.prepend(diff_item);
      }
    }
#endif
#if DIFF_END_HANDLING
    for (i=End.size()-1;i>=0;i--)
    {
      if (!string_vector.at(End[i].index()).isEmpty())
      {
        DiffItem diff_item;
        diff_item.setLength(string_vector.at(End[i].index()).length());
        diff_item.setOperation(DiffItem::SAME);
        diff_list.append(diff_item);
      }
    }
#endif

    // Compact list
    for (QList<DiffItem>::const_iterator it=diff_list.begin();it!=diff_list.end();++it)
    {
      if (diff.isEmpty())
        diff.append(*it);
      else
      {
        if (diff.last().operation()==(*it).operation())
        {
          diff.last().setLength(diff.last().length()+(*it).length());
          diff.last().setText(diff.last().text()+(*it).text());
        }
        else
          diff.append(*it);
      }
    }
  }

  if (b)
  {
    delete b;
    b=NULL;
  }
}

QString printDiff(const QString &str,const QList<DiffItem> &diff)
{
  QString ret;
  int pos=0;
  for (QList<DiffItem>::const_iterator it=diff.begin();it!=diff.end();++it)
  {
    switch( (*it).operation())
    {
      case DiffItem::DELETED:
        ret+="-"; 
        ret+= (*it).text();
        break;
      case DiffItem::INSERTED:
        ret+="+"; 
        ret += str.mid(pos,(*it).length());
        pos+=(*it).length();
        break;
      case DiffItem::SAME: 
        ret+="="; 
        ret += str.mid(pos,(*it).length());
        pos+=(*it).length();
        break;
    }
  }
  return ret;
}


differences_t differences(const QString &str1,const QString &str2)
{
  differences_t diff=DIFF_SAME;
  QString::const_iterator it1=str1.begin();
  QString::const_iterator it2=str2.begin();
  bool in_string1=false;
  bool in_string2=false;
  bool c_comment1=false;
  bool cpp_comment1=false;
  bool c_comment2=false;
  bool cpp_comment2=false;

  QChar last_char1='\0';
  QChar last_char2='\0';
  while (it1!=str1.end() && it2!=str2.end())
  {
    QChar c1=*it1;
    QChar c2=*it2;
    if (!in_string1)
    {
      if ( c1=='*' && last_char1=='/' ) c_comment1=true;
      if ( c1=='/' && last_char1=='/' ) cpp_comment1=true;
    }
    if (!in_string2)
    {
      if ( c2=='*' && last_char2=='/' ) c_comment2=true;
      if ( c2=='/' && last_char2=='/' ) cpp_comment2=true;
    }
    bool comment1=cpp_comment1 || c_comment1;
    bool comment2=cpp_comment2 || c_comment2;
    if (!comment1)
    {
      if ( c1=='"' && last_char1!='\\' ) in_string1=!in_string1;
    }
    if (!comment2)
    {
      if ( c2=='"' && last_char2!='\\' ) in_string2=!in_string2;
    }
    if (diff==DIFF_DIFFERENCE_IN_COMMENTS && comment1)
    {
      ++it1;
    }
    else if (diff==DIFF_DIFFERENCE_IN_COMMENTS && comment2)
    {
      ++it2;
    }
    else if (c1 != c2)
    {
      if ( c1.isSpace() ) 
      {
        if (in_string1)
          return DIFF_DIFFERENT;
        if (diff==DIFF_SAME) diff=DIFF_DIFFERENCE_IN_BLANCS;
        ++it1;
      }
      else if ( c2.isSpace() ) 
      {
        if (in_string2)
          return DIFF_DIFFERENT;
        if (diff==DIFF_SAME) diff=DIFF_DIFFERENCE_IN_BLANCS;
        ++it2;
      }
      else if (comment1)
        diff=DIFF_DIFFERENCE_IN_COMMENTS;
      else if (comment2)
        diff=DIFF_DIFFERENCE_IN_COMMENTS;
      else
        return DIFF_DIFFERENT;
    }
    else
    {
      ++it1;
      ++it2;
    }
    if ( c1=='/' && last_char1=='*' ) c_comment1=false;
    if ( c2=='/' && last_char2=='*' ) c_comment2=false;
    if ( c1=='\n' ) cpp_comment1=false;
    if ( c2=='\n' ) cpp_comment2=false;
    last_char1=c1;
    last_char2=c2;
  }
  while (it1!=str1.end())
  {
    QChar c1=*it1;
    if ( c1=='*' && last_char1=='/' ) c_comment1=true;
    if ( c1=='/' && last_char1=='/' ) cpp_comment1=true;
    bool comment1=cpp_comment1 || c_comment1;
    if ( c1.isSpace() ) 
    {
      if (diff==DIFF_SAME) diff=DIFF_DIFFERENCE_IN_BLANCS;
      ++it1;
    }
    else if (comment1)
    {
      diff=DIFF_DIFFERENCE_IN_COMMENTS;
      ++it1;
    }
    else
      return DIFF_DIFFERENT;
    if ( c1=='\n' ) cpp_comment1=false;
    if ( c1=='/' && last_char1=='*' ) c_comment1=false;
    last_char1=c1;
  }
  while (it2!=str2.end())
  {
    QChar c2=*it2;
    if ( c2=='*' && last_char2=='/' ) c_comment2=true;
    if ( c2=='/' && last_char2=='/' ) cpp_comment2=true;
    bool comment2=cpp_comment2 || c_comment2;

    if ( c2.isSpace() ) 
    {
      if (diff==DIFF_SAME) diff=DIFF_DIFFERENCE_IN_BLANCS;
      ++it2;
    }
    else if (comment2)
    {
      diff=DIFF_DIFFERENCE_IN_COMMENTS;
      ++it2;
    }
    else
      return DIFF_DIFFERENT;
    if ( c2=='/' && last_char2=='*' ) c_comment2=false;
    if ( c2=='\n' ) cpp_comment2=false;
    last_char2=c2;
  }

  return diff;
}
