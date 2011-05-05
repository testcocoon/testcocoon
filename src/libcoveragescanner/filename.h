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


#ifndef FILENAME_H
#define FILENAME_H
#include "coveragescannerpdef.h"
#include <stdio.h>
#include <vector>

class FileName
{
public:

    FileName();
    virtual ~FileName();

    const char * append(const char *);
    bool find(const char *) const;
    const char * at(long id) const { return files_tab.at(id); }
    long count() const { return files_tab.size(); }
    void clear();

    unsigned long signature(long) const;
    void merge(const FileName &v);
#if LOG
    void PDEBUG() const;
#endif
private:

    std::vector<char *> files_tab;

    unsigned long calc_signature(const char *file) const;
};
#endif
