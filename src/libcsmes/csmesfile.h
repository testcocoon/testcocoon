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

#ifndef CSMESFILE_H
#define CSMESFILE_H
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "libcsmespdef.h"
#if _MSC_VER == 1200
// VC6.0
#define _I64 __int64
#define _U64 unsigned __int64
#else
#ifndef _I64
#define _I64 long long
#endif
#ifndef _U64
#define _U64 unsigned long long
#endif
#endif 
#define CSMES_FILE_FLAG_FORCE_DLL_EXPORT (1<<0)

class LIBCSMES_API CSMESFile
{
  public:
    enum type_childrens_t { CHILDRENS_NONE=0, } ;
    enum type_children_t { CHILDREN_REL=0, } ;
    enum type_part_t { PART_IS_EXECUTED=0, PART_IS_TRUE=1, PART_IS_FALSE=2 } ;
    enum type_item_t { PART_ITEM_IS_EXECUTED=0, PART_ITEM_IS_TRUE=1, PART_ITEM_IS_FALSE=2 } ;
    enum merge_policy_t
    {
       FULL,
       ONLY_INSTRUMENTED_SOURCES
    } ;

    struct instrumentation_children_t
    {
      type_children_t  type;
      _I64 identifier;
    } ;

    struct instrumentation_part_item_t
    {
      type_item_t  type;
      std::string description;
      long s_line_org;
      long s_column_org;
      long s_line_pre;
      long s_column_pre; 
      long e_line_org;
      long e_column_org;
      long e_line_pre;
      long e_column_pre;
    } ;

    struct instrumentation_part_t
    {
      type_part_t type;
      std::string description;
      std::vector<instrumentation_part_item_t> items;
    };

    struct instrumentation_childrens_t
    {
      type_childrens_t type;
      std::vector<instrumentation_children_t> childrens;
    };

    enum type_t { NONE=0x12345678 , ORIGINAL=1,PREPROCESSED=2,_INSTRUMENTATION_V1=3, _INSTRUMENTATION_V2=23, EXECUTION=4,MANUAL_VALIDATION=5,COMMENT=6,_FUNCTIONS_INFO_V2=27, _FUNCTIONS_INFO_V1=7, EXECUTION_COMMENT=8, EXECUTION_COMPACT=9 , INSTRUMENTATION=_INSTRUMENTATION_V2, FUNCTIONS_INFO=_FUNCTIONS_INFO_V2} ;
    enum instrumentation_t { 
      _INSTRUMENTATION_COMPUTATION=1,
      _INSTRUMENTATION_CONDITION_FULL=2,
      _INSTRUMENTATION_CONDITION_TRUE_ONLY=3,
      _INSTRUMENTATION_CONDITION_FALSE_ONLY=4,
      _INSTRUMENTATION_NOP=5,
      _INSTRUMENTATION_LABEL=6,
    };
    enum access_t { RO,NEW_RW,APPEND_RW};
    CSMESFile();
    bool open( const char *filename, access_t );
    bool reopen( access_t a) { return open(fileName(),a); }
    const char *fileName() const { return _filename; }
    void close();
    virtual ~CSMESFile();
    bool openSection(const char *module,const char *module_rel,const char *name,const char *name_rel,const char *info,type_t type,unsigned long signature,access_t,unsigned long flag);
    bool deleteSection(const char *module,const char *name,const char *info,type_t type);
    bool deleteSectionID(int id);
    bool append(CSMESFile &csmes);
    bool merge(CSMESFile &csmes,unsigned long fl_merge,merge_policy_t policy, std::string &err);
    bool closeSection();
    int nbSections() const { return nb_section_tab ; }
    void sectionInfo(int id,char *module, int max_module_lg, char *module_rel, int max_module_rel_lg, char *name, int max_name_lg,char *name_rel, int max_name_rel_lg,char *info, int max_info_lg,type_t &type,unsigned long &signature) const;

    bool writeLong( const long c );
    bool writeLongLong( const _I64 c );
    bool writeChar( const char c );
    bool writeText( const char* text );
    bool writeCharTab( const char* text ,_I64 lg);
    bool writeExecutionComment( const char*comment /*! Comment (HTML text file) */);
    bool readExecutionComment( char**comment /*! Comment (HTML text file) deallocation from the called */);
    bool writeComment( long index,         /*!< Instrumentation index */
        const char*comment /*! Comment (HTML text file) */);
    bool readComment(long &index, /*!< Instrumentation index */
        char**comment /*! Comment (HTML text file) deallocation from the called */);
    bool readLong( long &c );
    bool readLongLong( _I64 &c );
    bool readChar( char &c );
    bool readCString(char **string_p);
    bool readInstrumentation(_I64 &identifier,long &index, long &nb_index,  instrumentation_t &t, long &s_line_org, long &s_column_orig,long &s_line_pre, long &s_column_pre, long &e_line_org, long &e_column_orig,long &e_line_pre, long &e_column_pre,std::vector<instrumentation_part_t> &, instrumentation_childrens_t &);
    bool writeInstrumentation(_I64 identifier,long index,  instrumentation_t t, long s_line_org,  long s_column_org,long s_line_pre, long s_column_pre, long e_line_org,long e_column_org,long e_line_pre, long e_column_pre, const std::vector<instrumentation_part_t> &, const instrumentation_childrens_t &);
    bool readFunctionInfo(char **scoped_name_p,char **prototype_p,long &s_line_orig, long &s_column_orig, long &s_line_pre, long &s_column_pre,long &e_line_orig, long &e_column_orig,long &e_line_pre, long &e_column_pre);
    bool writeFunctionInfo(const char *scoped_name,const char *prototype,long s_line_orig,long s_column_orig,long s_line_pre, long s_column_pre,long e_line_orig, long e_column_orig,long e_line_pre, long e_column_pre);

    _I64 nbExecution() const;
    bool readExecution(long *execution,int nb);
    void writeExecution(const long *execution,int nb);
    bool readExecutionStatus (int sec_id,long &execution_status) const;
    bool readExecutionStatus(long &execution_status) const;
    bool writeExecutionStatus(const char* name,long execution_status) ;
    bool writeExecutionStatus(int sec_id,long execution_status) ;
    _I64 readCharTab( char* text , _I64 max_lg);
    bool compress();
    static bool isCSMESFile(const char *file);
    unsigned long  signature() const { return section_tab[current_section].signature; }
    unsigned long  checksum() const { return section_tab[current_section].checksum; }
    unsigned long  flags() const { return section_tab[current_section].flags; }
    _I64 size() const { return section_tab[current_section].size; }
    _I64 position() const { return section_tab[current_section].position; }
    const char * sectionNameRelative() const 
    {
      const char*relative;
      const char*absolute;

      assoc_get(name_assoc,section_tab[current_section].name_id,absolute,relative);
      return relative;
    }
    const char * sectionName() const 
    {
      const char*relative;
      const char*absolute;

      assoc_get(name_assoc,section_tab[current_section].name_id,absolute,relative);
      return absolute;
    }
    const char * sectionModule() const 
    {
      const char*relative;
      const char*absolute;

      assoc_get(module_assoc,section_tab[current_section].module_id,absolute,relative);
      return absolute;
    }
    type_t  sectionType() const { return section_tab[current_section].type; }
    const char * sectionInfo() const { return section_tab[current_section].info; }
    const char *sectionNameRelative(int) const;
    const char *sectionModuleRelative(int) const;
    const char *sectionName(int) const;
    const char *sectionModule(int) const;
    const char *sectionInfo(int) const;
    type_t sectionType(int) const;
    unsigned long  sectionSignature() const { return sectionSignature(current_section); }
    bool isOpen() const { return f!=NULL; }
    bool openSectionID(int id);
    bool sectionExists(const char *module,const char *name,const char *info,type_t type) const;
    bool writeCString(const char *string_p);
    bool readExecutionCompact(char **module,long **execution,int *nb);
    void writeExecutionCompact(const char *module,const long *execution,int nb);
    unsigned long  flags(int i) const { return section_tab[i].flags; }
    unsigned long  sectionSignature(int) const;
    unsigned long  signature(int i) const { return section_tab[i].signature; }
    bool save_blackbox(const char *filename) ;
    bool isBlackBox() const;
    bool isCoverageBranchOnly() ;
    bool readExecutionStatus(const char* name,long &execution_status) const;
    void add_missing_sources();
    int findSectionID(const char *module,const char *name,const char *info,type_t type) const;
    unsigned long  checksum(int i) const { return section_tab[i].checksum; }
    const std::string &getLastErrorMsg() const { return error_msg; }
    bool empty() const { return nbSections()<=0; }

  private:
    _I64 size(int i)  const{ return section_tab[i].size; }
    _I64 position(int i) const { return section_tab[i].position; }
    _I64 section_pos;
    bool createSection(const char *module,const char *module_rel,const char *name,const char *name_rel,const char *info,type_t type,unsigned long signature,unsigned long flags);
    bool createSectionNoDelete(const char *module,const char *module_rel,const char *name,const char *name_rel,const char *info,type_t type,unsigned long signature,unsigned long flags);
    access_t access;
    char * buffer;
    size_t nb_allocated;
    long buffer_size;
    _I64 buffer_pos;
    inline void allocateBuffer(size_t size);
    bool readBuffer();

    int current_section;
    struct section_t 
    {
      type_t type;
      _I64 position;
      _I64 size;
      unsigned long flags;
      unsigned long signature;
      unsigned long checksum;
      char *info;
      int module_id;
      int name_id;
    };
    section_t *section_tab;
    int nb_section_tab;

    bool readSectionIndex();
    bool writeSectionIndex();
    bool moveSectionToEnd(int id);
    bool section_modifyed;
    FILE *f;
    int findLastSection();
    int last_section_id;
    void clearLastSection() { last_section_id=-1; }
    bool isLastSection() { return findLastSection()==current_section; }
    static inline void char2longlong(const char *,_I64 &);
    static inline void longlong2char(_I64,char *);
    static inline void char2long(const char *,long &);
    static inline void long2char(long,char *);
    char *_filename;
    static bool section_pos_eq(const section_t &,const section_t &);
    static bool section_pos_gt(const section_t &,const section_t &);
  private:
    inline bool writeCharTabInternal( const char* text ,_I64 lg);
    inline _I64 readCharTabInternal( char* text , _I64 max_lg);
    inline _I64 getFilePos() const;
    inline bool setFilePos(_I64,int);
    inline size_t readFile(void *data,size_t size,size_t elem) const;
    inline size_t writeFile(const void *data,size_t size,size_t elem);

  private:
     bool is_coverage_branch_only;
    struct assoc_data_t
    {
      char *absolute;
      char *relative;
      int *associated_id;
      int nb_associated_id;
    };
    struct assoc_t
    {
      int nb;
      mutable int last_id;
      assoc_data_t *data;
    };
    assoc_t name_assoc;
    assoc_t module_assoc;
    static inline int assoc_insert(assoc_t &assoc,const char *absolute,const char *relative) ;
    static void assoc_get(const assoc_t &assoc,int id,const char *&absolute,const char *&relative) ;
    static inline void assoc_clear(assoc_t &assoc);
    static inline void assoc_init(assoc_t &assoc);
    static inline int assoc_find_absolute(const assoc_t &assoc,const char *absolute) ;
    static inline void assoc_get_associated_id(const assoc_t &assoc,int id,int &nb_associated_id,const int *&associated_id) ;
    static inline void assoc_clear_associated_id(assoc_t &assoc,int id);
    static inline void assoc_clear_associated_id(assoc_t &assoc);
    static inline void assoc_insert_associated_id(assoc_t &assoc,int id,int associated_id) ;
    inline void generateAssociatedID();
    inline void create_section_link (int section_id,const char *module,const char *module_rel,const char *name,const char *name_rel,const char *info,type_t type,unsigned long signature,unsigned long flag);
    bool merge_precheck(CSMESFile &csmes,unsigned long fl_merge,merge_policy_t policy, std::string &err) ;
    bool merge_internal(CSMESFile &csmes,unsigned long fl_merge,merge_policy_t policy, std::string &err);
    void merge_instrumentation_list(int section_id,std::vector<instrumentation_t> &instrumentation,long &instrumentation_startindex);
    mutable std::string error_msg;
};

#endif
