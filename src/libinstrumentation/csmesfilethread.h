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

#ifndef CSMESFILETHREAD_H
#define CSMESFILETHREAD_H
#include "csmesfile.h"
#include <QMutex>

class  CSMESFileThread : public CSMESFile
{
public:
    CSMESFileThread();
    bool open( const char *filename, access_t );
    bool reopen( access_t a) ;
    bool openSection(const char *module,const char *module_rel,const char *name,const char *name_rel,const char *info,type_t type,unsigned long signature,access_t,unsigned long flag);
    void close();
    bool deleteSection(const char *module,const char *name,const char *info,type_t type);
    bool deleteSectionID(int id);
    bool append(CSMESFile &csmes);
    bool merge(CSMESFile &csmes,unsigned long fl_merge, CSMESFile::merge_policy_t policy,std::string &);
    bool save_blackbox(const char *filename);
    bool closeSection();
    bool openSectionID(int id);
    bool isCoverageBranchOnly();
private:
    mutable QMutex locker;
};

#endif
