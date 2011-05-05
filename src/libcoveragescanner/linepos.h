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

#ifndef LINEPOS_H
#define LINEPOS_H
#include "coveragescannerpdef.h"
#include <stdio.h>
#include "filename.h"

class LinePos
{
public:

    LinePos();
    virtual ~LinePos();

    void find(long abs_line,long &line_orig,const char *&,long &rel_line) const;
    const char * append(const char *,long line_orig,long abs_line);
    void at(long id,long &line_orig,long &line_abs, long &line_rel,const char * &filename) const;
    long count() const { return lineinfos.size() ; }
    void clear();
    const FileName &files() const { return filenames; }

#if LOG
    void PDEBUG() const;
#endif
private:
    FileName filenames;
    struct lineinfo_t 
    {
      long absolute_line_start;
      long absolute_line_end;
      long line_pre;
      long line_orig;
      const char *file; 
    }; 
    std::vector<lineinfo_t> lineinfos;

    const char * append_filename(const char *);
    const char*  find_internal(const char *name,long abs_line,long &rel_line,long &line_orig);
    void find_prot(long abs_line,long &,const char *&,long &rel_line,long &line_orig) const;
    const char * append_internal(const char *,long line_orig,long abs_line,long rel_line);
};
#endif
