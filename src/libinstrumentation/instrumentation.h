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

#ifndef _INSTRUMENTATION_H
#define _INSTRUMENTATION_H
#include "libinstrumentationpdef.h"
#include "executionnames.h"
#include <QString>
#include <QColor>
#include <QBrush>
class Writer;

class LIBINSTRUMENTATION_API Instrumentation
{
  public:
    enum coverage_method_t { COVERAGE_BRANCH=1, COVERAGE_CONDITION=0 , METHOD_UNKNOWN=2} ;
    enum status_t { STATUS_PARTIALLY_VALIDATED, STATUS_UNVALIDATED, STATUS_VALIDATED, STATUS_MANUALLY_VALIDATED,STATUS_UNKNOWN,STATUS_NONE,STATUS_HIDDEN };
    enum filter_t { FILTER_UNVALIDATED, FILTER_VALIDATED, FILTER_NONE,FILTER_MANUALLY_VALIDATED,FILTER_COMMENT };
    enum type_t { COMPUTATION, LABEL, CONDITION_FULL, CONDITION_TRUE_ONLY, CONDITION_FALSE_ONLY,NOP,TYPE_NULL };
    enum execution_state_t { EXECUTION_STATE_NOT_EXECUTED=0, EXECUTION_STATE_UNKNOWN=-1, EXECUTION_STATE_HIDDEN=-3, EXECUTION_STATE_NONE=-4 ,EXECUTION_MAX=0x7FFFFFFF };

    Instrumentation()
    {
      index              = -1;
      _end_line_pre       = -1;
      _end_column_pre     = -1;
      _start_line_pre     = -1;
      _start_column_pre   = -1;
      _end_line_org       = -1;
      _end_column_org     = -1;
      _start_line_org     = -1;
      _start_column_org   = -1;
      type               = TYPE_NULL;
      execution[0]       = EXECUTION_STATE_UNKNOWN;
      execution[1]       = EXECUTION_STATE_UNKNOWN;
      manually_validated = false;
      equiv              = this;
    }

    Instrumentation(int ind, type_t t,long s_line_org,long s_column_org,long e_line_org,long e_column_org,long s_line_pre,long s_column_pre,long e_line_pre,long e_column_pre)
    {
      index              = ind;
      type               = t;
      _end_line_pre       = e_line_pre;
      _end_column_pre     = e_column_pre;
      _start_line_pre     = s_line_pre;
      _start_column_pre   = s_column_pre;
      _end_line_org       = e_line_org;
      _end_column_org     = e_column_org;
      _start_line_org     = s_line_org;
      _start_column_org   = s_column_org;
      execution[0]       = EXECUTION_STATE_UNKNOWN;
      execution[1]       = EXECUTION_STATE_UNKNOWN;
      manually_validated = false;
      equiv              = this;
      ASSERT(!valid() || _end_line_org>=0);
      ASSERT(!valid() || _end_line_pre>=0);
      ASSERT(!valid() || _start_line_pre>=0);
      ASSERT(!valid() || _end_column_pre>=0);
      ASSERT(!valid() || _start_column_pre>=0);
      ASSERT(!valid() || _start_line_org>=0);
    }

    bool valid() const ;
    long endLineOrg() const              { ASSERT(!valid() || _end_line_org>=0); return _end_line_org; }
    long endColumnOrg() const            { return _end_column_org; }
    long endLinePre() const              { ASSERT(!valid() || _end_line_pre>=0);return _end_line_pre; }
    long startLinePre() const            { ASSERT(!valid() || _start_line_pre>=0);return _start_line_pre; }
    long endColumnPre() const            { ASSERT(!valid() || _end_column_pre>=0);return _end_column_pre; }
    long startColumnPre() const          { ASSERT(!valid() || _start_column_pre>=0);return _start_column_pre; }
    long startLineOrg() const            { ASSERT(!valid() || _start_line_org>=0);return _start_line_org; }
    long startColumnOrg() const          { return _start_column_org; }

    bool isInRangeOrg(int _start_line,int _start_column, int _end_line,int _end_column) const
    {
      if (_start_line>startLineOrg())
        return false;
      if (_end_line<endLineOrg())
        return false;
      if (_end_line==endLineOrg() && _end_column<endColumnOrg())
        return false;
      if (_start_line==startLineOrg() && _start_column>startColumnOrg())
        return false;
      return true;
    }

    bool isInRangePre(int _start_line,int _start_column, int _end_line,int _end_column) const
    {
      if (_start_line>startLinePre())
        return false;
      if (_end_line<endLinePre())
        return false;
      if (_end_line==endLinePre() && _end_column<endColumnPre())
        return false;
      if (_start_line==startLinePre() && _start_column>startColumnPre())
        return false;
      return true;
    }

    status_t status(int level,coverage_method_t) const ;
    static QColor statusColor(status_t) ;
    static QBrush statusBackgroundColor(status_t status,bool selected);
    type_t getType() const { return type; }
    bool validIndex(int index) const
    {
      return (index>=getMinIndex() && index<=getMaxIndex());
    }
    int getMinIndex() const 
    { 
      ASSERT(index>=0);
      return index; 
    }
    int getMaxIndex() const 
    {
      ASSERT(type!=TYPE_NULL);
      ASSERT(index>=0);
      switch(type)
      {
        case CONDITION_FALSE_ONLY:
        case CONDITION_TRUE_ONLY:
        case LABEL:
        case COMPUTATION:
        case NOP:
          return index;
        case CONDITION_FULL:
          return index+1;
        case TYPE_NULL:
          break;
      }
      return index;
    }
    void setExecution(execution_state_t executed);
    void clearExecution()
    {
      execution[0]=EXECUTION_STATE_UNKNOWN;
      execution[1]=EXECUTION_STATE_UNKNOWN;
    }

    QString explanationPlainText(const QString &source_code,int level,coverage_method_t method) const;
    bool isConditionInstrumentation() const 
    {
      switch (getType())
      {
        case TYPE_NULL:
          ASSERT(false);
        case NOP:
        case COMPUTATION:
          return false;
        case LABEL:
        case CONDITION_FULL:
        case CONDITION_TRUE_ONLY:
        case CONDITION_FALSE_ONLY:
          return true;
      }
      ASSERT(false);
      return false;
    }

    void explanation(Writer &,const QString &,const QString &comment,const ExecutionNames executedBy[2],int level,coverage_method_t,int) const;
    void setManuallyValidated(bool b) {  manually_validated=b; }
    bool getManuallyValidated() const {  return manually_validated; }
    void setNull() { type=TYPE_NULL; }
    bool isNull() const { return type==TYPE_NULL; }
    bool hasEquivalent() const { return this!=equiv; }
    Instrumentation *getEquivalent() const { return equiv; }
    void addEquivalent(Instrumentation *);
    void resetEquivalent() { equiv=this; }

    void copyExecutionState(const Instrumentation &v)
    {
      execution[0]=v.execution[0];
      execution[1]=v.execution[1];
    }

    void setExecution(int index,execution_state_t executed)
    {
      ASSERT(index>=getMinIndex());
      ASSERT(index<=getMaxIndex());
      execution[index-getMinIndex()]=executed;
    }

    execution_state_t getExecution(int index) const
    {
      ASSERT(index>=getMinIndex());
      ASSERT(index<=getMaxIndex());
      return static_cast<execution_state_t>(execution[index-getMinIndex()]);
    }

    static execution_state_t combineExecution(execution_state_t a,execution_state_t b)
    {
      if (a==EXECUTION_STATE_HIDDEN || b==EXECUTION_STATE_HIDDEN) return EXECUTION_STATE_HIDDEN;
      if (a==EXECUTION_STATE_NONE || b==EXECUTION_STATE_NONE) return EXECUTION_STATE_NONE;
      if (static_cast<int>(a)>=0 && static_cast<int>(b)>=0) 
      {
        unsigned int v=static_cast<unsigned int>(a)+static_cast<unsigned int>(b);
        if (v>EXECUTION_MAX)
          return EXECUTION_MAX;
        else
          return static_cast<execution_state_t>(v);
      }
      if (a==EXECUTION_STATE_UNKNOWN && static_cast<int>(b)>=0) return static_cast<execution_state_t>(b);
      if (static_cast<int>(a)>=0 && b==EXECUTION_STATE_UNKNOWN) return static_cast<execution_state_t>(a);
      return EXECUTION_STATE_UNKNOWN;
    }

    static execution_state_t adjustExecutionCount(execution_state_t e,bool test_coverage_mode)
    {
      if (test_coverage_mode)
      {
        if (static_cast<int>(e)>0)
          e = static_cast<execution_state_t> (1);
      }
      return e;
    }

    int nb_tested(int coverage_level,coverage_method_t method) const 
    {
      ASSERT(coverage_level>0);
      ASSERT(method!=METHOD_UNKNOWN);
      if (getType()==NOP)
        return 0;
      if (method==COVERAGE_BRANCH)
      {
        if (isConditionInstrumentation())
          return 0;
      }

      int nb=getMaxIndex()-getMinIndex()+1;
      if (manually_validated)
        return nb;

      int tested=0;
      for (int i=0;i<nb;i++)
      {
        if (execution[i]>=coverage_level)
          tested++;
      }

      return tested;
    }

    int nb_untested(int coverage_level,coverage_method_t method) const 
    {
      ASSERT(coverage_level>0);
      ASSERT(method!=METHOD_UNKNOWN);
      if (getType()==NOP)
        return 0;
      if (method==COVERAGE_BRANCH)
      {
        if (isConditionInstrumentation())
          return 0;
      }
      if (manually_validated)
        return 0;

      int nb=getMaxIndex()-getMinIndex()+1;
      int un_untested=0;

      for (int i=0;i<nb;i++)
      {
        if ( (execution[i]<coverage_level && execution[i]>=0)
            || 
            execution[i]==EXECUTION_STATE_HIDDEN )
          un_untested++;
      }
      return un_untested;
    }

    static status_t combine(status_t a,status_t b) ;

    int executionCount(int index)  const
    {
      ASSERT(index>=getMinIndex());
      ASSERT(index<=getMaxIndex());
      return execution[index-getMinIndex()];
    }

    int executionCountMin() const;
    int executionCountMax() const;

    Instrumentation& operator= ( const Instrumentation & src ) 
    {
      index              = src.index;
      type               = src.type;
      _end_line_pre       = src.endLinePre();
      _end_column_pre     = src.endColumnPre();
      _start_line_pre     = src.startLinePre();
      _start_column_pre   = src.startColumnPre();
      _end_line_org       = src.endLineOrg();
      _end_column_org     = src.endColumnOrg();
      _start_line_org     = src.startLineOrg();
      _start_column_org   = src.startColumnOrg();
      execution[0]       = src.execution[0];
      execution[1]       = src.execution[1];
      manually_validated = src.manually_validated;
      equiv              = this;
      return *this;
    }

    bool operator== ( const Instrumentation & x ) const
    {
      if (isNull() && x.isNull())               return true;
      if (isNull())                             return false;
      if (x.isNull())                           return false;
      if (type!=x.type)                         return false;
      if (startColumnOrg()==-1 && x.startColumnOrg()==-1)
      { // Compare original line with preprocessed column
         if (startColumnPre()!=x.startColumnPre()) return false;
         if (endColumnPre()!=x.endColumnPre())     return false;
      }
      else
      {
         if (endColumnOrg()!=x.endColumnOrg())     return false;
         if (startColumnOrg()!=x.startColumnOrg()) return false;
      }
      if (startLineOrg()!=x.startLineOrg())     return false;
      if (endLineOrg()!=x.endLineOrg())         return false;
      //if (startLinePre()!=x.startLinePre())     return false;
      //if (endLinePre()!=x.endLinePre())         return false;
      return true;
    }

    bool operator< ( const Instrumentation & x ) const
    {
      ASSERT(!isNull());
      ASSERT(!x.isNull());
      if (_start_line_pre>=x._start_line_pre)     return false;
      if (_start_column_pre>=x._start_column_pre) return false;
      if (_end_line_pre>=x._end_line_pre)         return false;
      if (_end_column_pre>=x._end_column_pre)     return false;
      if (type>=x.type)                         return false;
      return true;
    }


  private:
    int index;
    type_t type;
    bool manually_validated;
    long _start_line_pre,_end_line_pre,_start_column_pre,_end_column_pre;
    long _start_line_org,_end_line_org,_start_column_org,_end_column_org;
    execution_state_t execution[2];
    Instrumentation *equiv;

    static inline status_t combine_intern(status_t a,status_t b);

};

#endif
