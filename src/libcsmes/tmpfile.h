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

#ifndef _TMP_FILE_H_
#define _TMP_FILE_H_
#include "libcsmespdef.h"
#include <list>
#include <string>
class TestTmpFile;

class LIBCSMES_API TmpFile
{
  public:
    static TmpFile &object() ;

    std::string generate(const std::string &Template,bool inTempDir) const;
    void removeUnusedFile(const std::string &file);
    void deleteFile(const std::string &file);
    void deleteFiles();
    void setDeleteFiles(bool b) { delete_files=b; }
    bool isDeleteFiles()const  { return delete_files; }
  private:
    static std::string number_base36(unsigned int n);
    TmpFile();
    static void AtExit();
    bool delete_files;
    struct tmp_file_t
    {
      std::string tmpl;
      std::string file;
      bool operator==(const tmp_file_t &a) const { return (a.file==this->file) && (a.tmpl==this->tmpl) ; }
    };
    mutable std::list<tmp_file_t> *files;
    std::string tmp_dir;
    int id;
    unsigned int tm;
    std::string id_str;
    const std::string &tmp_folder() const ;
    friend class TestTmpFile;
};
#endif
