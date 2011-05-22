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

#include "csmesfile.h"
#include "version.h"
#include <string.h>
#include <string>
#include <list>
#include <sstream>
#define MAGIC (0x31330000|(((TESTCOCOON_VERSION+0x070000)>>8)&0xFFFF))
#define MAGICCODE(X) (((X)&0xFFFF0000)>>16)
#define MAGICVERSION(X) ((X)&0xFFFF)
#define MAGICOLD 0x31323335

#if defined(OS_WIN32)
#define OFFSET _I64
#define FTELL _ftelli64
#define FSEEK _fseeki64
#elif defined(OS_LINUX)
#define OFFSET off64_t
#define FTELL ftello64
#define FSEEK fseeko64
#elif defined(OS_MACX)
#define OFFSET off_t
#define FTELL ftello
#define FSEEK fseeko
#else
#error "unsupported platform"
#endif
#ifndef NO_DEBUG
#define PREALLOCATE_BUFFER_SIZE 0
#else
#define PREALLOCATE_BUFFER_SIZE 4096
#endif
#define BINARY_FORMAT "b"


CSMESFile::CSMESFile()
{
  FUNCTION_TRACE;
  ASSERT (sizeof(OFFSET) == 8);
  assoc_init(name_assoc);
  assoc_init(module_assoc);
  is_coverage_branch_only=false;
  nb_allocated=0;
  buffer_size=0;
  buffer=NULL;
  _filename=NULL;
  f=NULL;
  nb_section_tab=0;
  section_tab=NULL;
  section_modifyed=false;
  clearLastSection();
  current_section=-1;
}

bool CSMESFile::open(const char *file_name,access_t a)
{
  FUNCTION_TRACE;
  f=NULL;
  if (_filename)
    FREE(_filename);
  _filename=STRDUP(file_name);
  section_modifyed=false;
  clearLastSection();
  current_section=-1;
  section_tab=NULL;
  nb_section_tab=0;
  is_coverage_branch_only=false;
  assoc_clear(name_assoc);
  assoc_clear(module_assoc);
  access=a;
  bool ret;
  switch (access)
  {
    case RO:
      f=fopen(fileName(),"r"BINARY_FORMAT );
      if (f==NULL)
      {
        error_msg="File nor existing";
        return false;
      }
      ret = readSectionIndex();
      if (ret)
         is_coverage_branch_only=isCoverageBranchOnly();
      return ret;
      break;
    case APPEND_RW:
       f=fopen(fileName(),"r+"BINARY_FORMAT );
       if (f==NULL)
       {
          error_msg="File nor existing";
          return false;
       }
       ret = readSectionIndex();
       if (ret)
          is_coverage_branch_only=isCoverageBranchOnly();
       return ret;
      break;
    case NEW_RW:
      f=fopen(fileName(),"w+"BINARY_FORMAT );
      if (f==NULL)
      {
        error_msg="File nor existing";
        return false;
      }
      break;
  }
  return true;
}

CSMESFile::~CSMESFile()
{
  FUNCTION_TRACE;
  close();
  FREE(_filename);
  assoc_clear(name_assoc);
  assoc_clear(module_assoc);
}

void CSMESFile::close()
{
  FUNCTION_TRACE;
  if (f!=NULL)
  {
    if (access!=RO)
    {
      closeSection();
      writeSectionIndex();
    }
    fclose(f);
    f=NULL;
  }
  if (section_tab!=NULL)
  {
    int i;
    for (i=0;i<nb_section_tab;i++)
    {
      FREE(section_tab[i].info);
    }
    FREE (section_tab);
    section_tab=NULL;
  }
  section_modifyed=false;
  clearLastSection();
  current_section=-1;
  nb_section_tab=0;
  assoc_clear(name_assoc);
  assoc_clear(module_assoc);
}

bool CSMESFile::readSectionIndex()
{
  FUNCTION_TRACE;
  long section_sz=0;
  char tmp_ch[4];
  int i;
  if (section_tab!=NULL)
  {
    error_msg="Section already read";
    return false;
  }
  setFilePos(-8,SEEK_END);
  if (readFile(tmp_ch,4,1)<=0)
  {
    error_msg="IO Error";
    return false;
  }
  char2long(tmp_ch,section_sz);
  int magic;
  if (readFile(&magic,1,4)<=0)
  {
    error_msg="Wrong file format";
    return false;
  }
  if (MAGICCODE(magic)==MAGICCODE(MAGIC))
  {
    long magic_version=MAGICVERSION(magic);
    long testcocoon_version=MAGICVERSION(MAGIC);
    if (magic_version>testcocoon_version)
    {
      char tmp[100];
      sprintf(tmp,"TestCocoon version %i.%i.00 or greater is needed",(MAGICVERSION(magic)>>8) - 7,MAGICVERSION(magic)&0xFF);
      error_msg=tmp;
      return false;
    }
  }
  else if (magic!=MAGICOLD)
  {
    error_msg="Wrong file version";
    return false;
  }
  setFilePos(-8-section_sz,SEEK_CUR);
  section_pos=getFilePos();
  if (readFile(tmp_ch,4,1)<=0)
  {
    error_msg="IO Error";
    return false;
  }
  long l;
  char2long(tmp_ch,l);nb_section_tab=l;

  int sz=nb_section_tab*sizeof(section_t);
  if (sz!=0)
  {
    /* read the complete index from the file */
    int nb_read=0;
    int rd=0;
    char * index_buffer;
    index_buffer=(char*)MALLOC(section_sz);
    do
    {
      rd=readFile(&(index_buffer[nb_read]),1,section_sz-nb_read);
      if (rd>0)
        nb_read+=rd;
      else if (!feof(f))
      {
        error_msg="IO Error";
        return false;
      }
      else
        break; // EOF
    }
    while (section_sz-nb_read>0) ;

    /* build the index */
    section_tab=(section_t*)MALLOC(sz);
    memset(section_tab,0xFF,sz);
    int index_buffer_pos=0;
    for (i=0;i<nb_section_tab;i++)
    {
      char c;
      int strlg;
      int pos;

       /* module name */
      strlg=0;
      pos=index_buffer_pos;
      do
      {
        c=index_buffer[index_buffer_pos++];
        strlg++;
      } while (c!='\0');
      int pos_module_absolute=pos;

       /* module name relative */
      strlg=0;
      pos=index_buffer_pos;
      do
      {
        c=index_buffer[index_buffer_pos++];
        strlg++;
      } while (c!='\0');
      int pos_module_relative=pos;
      section_tab[i].module_id=assoc_insert(module_assoc,&index_buffer[pos_module_absolute],&index_buffer[pos_module_relative]);

      /* file name */
      strlg=0;
      pos=index_buffer_pos;
      do
      {
        c=index_buffer[index_buffer_pos++];
        strlg++;
      } while (c!='\0');
      int pos_name_absolute=pos;

      /* file name relative */
      strlg=0;
      pos=index_buffer_pos;
      do
      {
        c=index_buffer[index_buffer_pos++];
        strlg++;
      } while (c!='\0');
      int pos_name_relative=pos;
      section_tab[i].name_id=assoc_insert(name_assoc,&index_buffer[pos_name_absolute],&index_buffer[pos_name_relative]);

      /* info */
      strlg=0;
      pos=index_buffer_pos;
      do
      {
        c=index_buffer[index_buffer_pos++];
        strlg++;
      } while (c!='\0');
      section_tab[i].info=(char*)MALLOC(strlg+1);
      strcpy(section_tab[i].info,&index_buffer[pos]);

      long l=-1;
      _I64 ll=-1;

      char2long(&index_buffer[index_buffer_pos],l);index_buffer_pos+=4; section_tab[i].type=(type_t)l;
      char2longlong(&index_buffer[index_buffer_pos],ll);index_buffer_pos+=8; section_tab[i].position=ll;
      char2longlong(&index_buffer[index_buffer_pos],ll);index_buffer_pos+=8; section_tab[i].size=ll;
      char2long(&index_buffer[index_buffer_pos],l);index_buffer_pos+=4; section_tab[i].signature=l;
      char2long(&index_buffer[index_buffer_pos],l);index_buffer_pos+=4; section_tab[i].checksum=l;
      char2long(&index_buffer[index_buffer_pos],l);index_buffer_pos+=4; section_tab[i].flags=l;

      ASSERT(section_tab[i].type>0);
      ASSERT(section_tab[i].size>=0);
      ASSERT(section_tab[i].position>=0);
      ASSERT(section_tab[i].position>=0);
    }
  }
  generateAssociatedID();
  return true;
}

bool CSMESFile::writeSectionIndex()
{
  FUNCTION_TRACE;
  char * index_buffer=NULL;
  long nb_index_buffer=0;
  long index_buffer_pos=0;
  OFFSET index_file_pos=0;

  for (int i=0;i<nb_section_tab;i++)
  {
    _I64 tmp_ll;
    const char*name;
    const char*name_rel;
    const char*module;
    const char*module_rel;

    assoc_get(name_assoc,section_tab[i].name_id,name,name_rel);
    assoc_get(module_assoc,section_tab[i].module_id,module,module_rel);

    int lg_module=strlen(module)+1;
    int lg_module_rel=strlen(module_rel)+1;
    int lg_name=strlen(name)+1;
    int lg_name_rel=strlen(name_rel)+1;
    int lg_info=strlen(section_tab[i].info)+1;
    nb_index_buffer+=lg_module_rel +lg_name_rel +lg_module +lg_name +lg_info+4+8+8+4+4+4;
    index_buffer=(char*)REALLOC(index_buffer,nb_index_buffer);
    strcpy( &index_buffer[index_buffer_pos],module);index_buffer_pos+=lg_module;
    strcpy( &index_buffer[index_buffer_pos],module_rel);index_buffer_pos+=lg_module_rel;
    strcpy( &index_buffer[index_buffer_pos],name);index_buffer_pos+=lg_name;
    strcpy( &index_buffer[index_buffer_pos],name_rel);index_buffer_pos+=lg_name_rel;
    strcpy( &index_buffer[index_buffer_pos],section_tab[i].info);index_buffer_pos+=lg_info;
    long2char(section_tab[i].type,&index_buffer[index_buffer_pos]); index_buffer_pos+=4;
    tmp_ll=position(i); longlong2char(tmp_ll,&index_buffer[index_buffer_pos]);  index_buffer_pos+=8;
    tmp_ll=size(i);longlong2char(tmp_ll,&index_buffer[index_buffer_pos]);  index_buffer_pos+=8;
    long2char(section_tab[i].signature,&index_buffer[index_buffer_pos]);  index_buffer_pos+=4;
    long2char(section_tab[i].checksum,&index_buffer[index_buffer_pos]);  index_buffer_pos+=4;
    long2char(section_tab[i].flags,&index_buffer[index_buffer_pos]);  index_buffer_pos+=4;
  }

  if (nb_section_tab!=0)
  {
    long last_id=findLastSection();
    index_file_pos=size(last_id)+position(last_id);
    setFilePos(0,SEEK_END);
    OFFSET end_pos=getFilePos();
    if (end_pos-nb_index_buffer-12>index_file_pos)
      index_file_pos=end_pos-nb_index_buffer-12;
  }
  setFilePos(index_file_pos,SEEK_SET);
  char tmp_ch[4];
  long2char(nb_section_tab,tmp_ch);
  writeFile(tmp_ch,4,1);
  int nb_write=0;
  while (nb_write<nb_index_buffer)
  {
    int wd=writeFile(&(index_buffer[nb_write]),1,nb_index_buffer-nb_write);
    if (wd<=0)
    {
      error_msg="IO Error";
      return false;
    }
    nb_write+=wd;
  }
  nb_index_buffer+=4;
  long2char(nb_index_buffer,tmp_ch);
  writeFile(tmp_ch,4,1);
  long magic=MAGIC;
  writeFile(&magic,4,1);
  return true;
}

bool CSMESFile::isCSMESFile(const char *file)
{
  FUNCTION_TRACE;
  long magic;
  FILE *ftest;
  ftest=fopen(file,"r+"BINARY_FORMAT );
  if (ftest==NULL)
    return false;
  FSEEK(ftest,-4,SEEK_END);
  if (fread(&magic,1,sizeof(magic),ftest)<=0)
  {
    fclose(ftest);
    return false;
  }
  fclose(ftest);
  if (MAGICCODE(magic)==MAGICCODE(MAGIC))
  {
    if (MAGICVERSION(magic)>MAGICVERSION(MAGIC))
      return false;
  }
  else if (magic!=MAGICOLD)
    return false;
  return true;
}

bool CSMESFile::writeLongLong( const _I64 c )
{
  FUNCTION_TRACE;
  char l[8];
  longlong2char(c,l);
  return writeCharTabInternal(l,8);
}

bool CSMESFile::writeLong( const long c )
{
  FUNCTION_TRACE;
  char l[4];
  long2char(c,l);
  return writeCharTabInternal(l,4);
}

bool CSMESFile::writeChar( const char c )
{
  FUNCTION_TRACE;
  return writeCharTabInternal(&c,1);
}

bool CSMESFile::writeText( const char *text )
{
  FUNCTION_TRACE;
  return writeCharTabInternal(text,strlen(text));
}

bool CSMESFile::writeCharTab( const char *text , _I64 sz)
{
  FUNCTION_TRACE;
  ASSERT(sz>=0);
  return writeCharTabInternal( text , sz);
}


#define MOD_ADLER 65521

inline void adler32(unsigned long &chksum,const unsigned char *data, unsigned long len)
{
  FUNCTION_TRACE;
  unsigned long a=chksum&0xFFFF;
  unsigned long b=(chksum>>16)&0xFFFF;

  while (len > 0)
  {
    unsigned long tlen = len > 4096 ? 4096 : len;
    len -= tlen;
    do
    {
      a += *data++;
      b += a;
    } while (--tlen);

    a %= MOD_ADLER;
    b %= MOD_ADLER;
  }

  chksum = (b << 16) | a;
}

bool CSMESFile::writeCharTabInternal( const char *text , _I64 sz)
{
  FUNCTION_TRACE;
  if (sz==0)
    return true;
  ASSERT(sz>0);

  if (!isLastSection()) moveSectionToEnd(current_section);
  section_modifyed=true;

  int nb_write=0;
  char *tmp;
  tmp=(char*)text;
  adler32(
      section_tab[current_section].checksum,
      (const unsigned char*)text,
      static_cast<unsigned long>(sz));

  while (nb_write<sz)
  {
     int wd=writeFile(&(tmp[nb_write]),1,static_cast<int>(sz-nb_write));
    if (wd<=0)
    {
      error_msg="IO Error";
      return false;
    }
    nb_write+=wd;
  }
  return true;
}


bool CSMESFile::readLongLong( _I64 &c )
{
  FUNCTION_TRACE;
  char l[8];

  if ( readCharTabInternal(l,8) )
  {
    char2longlong(l,c);
    return true;
  }
  return false;
}

bool CSMESFile::readLong( long &c )
{
  FUNCTION_TRACE;
  char l[4];

  if ( readCharTabInternal(l,4) )
  {
    char2long(l,c);
    return true;
  }
  return false;
}

bool CSMESFile::readChar( char &c )
{
  FUNCTION_TRACE;
  return   readCharTabInternal(&c,1)!=0 ;
}

_I64 CSMESFile::readCharTabInternal( char *text , _I64 max_lg)
{
  FUNCTION_TRACE;
  if (max_lg<=0)
    return 0;
  if (buffer==NULL)
  {
    buffer_pos=0;
    if (!readBuffer())
      return 0;
  }
  OFFSET nb_read=size()-buffer_pos;
  if (nb_read>max_lg)
    nb_read=max_lg;
  memcpy(text,&buffer[buffer_pos],static_cast<size_t>(nb_read));
  buffer_pos+=nb_read;
  return nb_read;
}

_I64 CSMESFile::readCharTab( char *text , _I64 max_lg)
{
  FUNCTION_TRACE;
  return readCharTabInternal(text,max_lg);
}

bool CSMESFile::createSection(const char *module,const char *module_rel,const char *name,const char *name_rel,const char *info,type_t type,unsigned long signature,unsigned long flag)
{
  FUNCTION_TRACE;
  deleteSection(module,name,info,type);
  return createSectionNoDelete(module,module_rel,name,name_rel,info,type,signature,flag);
}

bool CSMESFile::createSectionNoDelete(const char *module,const char *module_rel,const char *name,const char *name_rel,const char *info,type_t type,unsigned long signature,unsigned long flag)
{
  FUNCTION_TRACE;
  int last_sec=findLastSection();
  nb_section_tab++;
  int sz=nb_section_tab*sizeof(section_t);
  section_tab=(section_t*)REALLOC((void*)section_tab,sz);

  section_tab[nb_section_tab-1].name_id=assoc_insert(name_assoc,name,name_rel);
  section_tab[nb_section_tab-1].module_id=assoc_insert(module_assoc,module,module_rel);
  assoc_insert_associated_id(name_assoc,section_tab[nb_section_tab-1].name_id,nb_section_tab-1);
  assoc_insert_associated_id(module_assoc,section_tab[nb_section_tab-1].module_id,nb_section_tab-1);
  if (info)
    section_tab[nb_section_tab-1].info=STRDUP(info);
  else
    section_tab[nb_section_tab-1].info=STRDUP("");
  section_tab[nb_section_tab-1].type=type;
  section_tab[nb_section_tab-1].signature=signature;
  section_tab[nb_section_tab-1].size=0;
  section_tab[nb_section_tab-1].checksum=1;
  section_tab[nb_section_tab-1].flags=flag;
  if (last_sec>=0)
    section_tab[nb_section_tab-1].position=position(last_sec)+size(last_sec);
  else
    section_tab[nb_section_tab-1].position=0;
  clearLastSection();
  last_section_id=nb_section_tab-1;

  return true;
}

bool CSMESFile::deleteSection(const char *module,const char *name,const char *info,type_t type)
{
  FUNCTION_TRACE;
  int sec_id= findSectionID(module,name,info,type) ;
  if (sec_id!=-1)
    return deleteSectionID(sec_id) ;
  return false;
}

bool CSMESFile::deleteSectionID(int id)
{
  FUNCTION_TRACE;
  if (id>=nb_section_tab)
    return false;
  FREE(section_tab[id].info);
  nb_section_tab--;
  for (int i=id;i<nb_section_tab;i++)
  {
    section_tab[i]=section_tab[i+1];
  }
  current_section=-1;
  section_modifyed=false;
  clearLastSection();
  generateAssociatedID();
  return true;
}

bool CSMESFile::openSectionID(int id)
{
  FUNCTION_TRACE;
  if (id>=nb_section_tab)
    return false;
  current_section=id;
  section_modifyed=false;
  if (!setFilePos(section_tab[current_section].position,SEEK_SET))
    return false;
  allocateBuffer(0);
  return true;
}

int CSMESFile::findSectionID(const char *module,const char *name,const char *info,type_t type) const
{
  FUNCTION_TRACE;
  int name_id=assoc_find_absolute(name_assoc,name);
  if (name_id<0)
    return -1;
  int module_id=assoc_find_absolute(module_assoc,module);
  if (module_id<0)
    return -1;

  const int *module_associated_id;
  int nb_module_associated_id;
  assoc_get_associated_id(module_assoc,module_id,nb_module_associated_id,module_associated_id);
  int found=-1;
  for (int id=0;id<nb_module_associated_id;id++)
  {
    int i=module_associated_id[id];
    ASSERT (module_id==section_tab[i].module_id) ;
    if ( (type==section_tab[i].type)
        && (name_id==section_tab[i].name_id) )
    {
      if (info)
      {
        if (strcmp(info,section_tab[i].info)==0)
        {
          found= i;
          break;
        }
      }
      else
      {
        found= i;
        break;
      }
    }
  }
#ifndef NO_DEBUG
  int verify_found=-1;
  for (int i=0;i<nb_section_tab;i++)
  {
    if ( (type==section_tab[i].type)
        && (module_id==section_tab[i].module_id)
        && (name_id==section_tab[i].name_id) )
    {
      if (info)
      {
        if (strcmp(info,section_tab[i].info)==0)
        {
          verify_found= i;
          break;
        }
      }
      else
      {
        verify_found= i;
        break;
      }
    }
  }
  ASSERT(verify_found==found);
#endif
  return found;
}

bool CSMESFile::sectionExists(const char *module,const char *name,const char *info,type_t type) const
{
  FUNCTION_TRACE;
  return findSectionID(module,name,info,type) != -1 ;
}

bool CSMESFile::openSection(const char *module,const char *module_rel,const char *name,const char *name_rel,const char *info,type_t type,unsigned long signature,access_t acc,unsigned long flag)
{
  FUNCTION_TRACE;
  switch(acc)
  {
    case NEW_RW:
      if (!createSection(module,module_rel,name,name_rel,info,type,signature,flag))
        return false;
      break;
    default:
      break;
  }
  int sec_id= findSectionID(module,name,info,type) ;
  if (sec_id!=-1)
      return openSectionID(sec_id);
  return false;
}

bool CSMESFile::closeSection()
{
  FUNCTION_TRACE;
  if (section_modifyed)
    section_tab[current_section].size=getFilePos()-position();
  section_modifyed=false;
  current_section=-1;
  return true;
}

bool CSMESFile::compress()
{
  FUNCTION_TRACE;
  return true;
}

bool CSMESFile::moveSectionToEnd(int id)
{
  FUNCTION_TRACE;
  OFFSET pos=-1;
  int count=0;
  int last=-1;
  for (int i=0;i<nb_section_tab;i++)
  {
    if  (pos<section_tab[i].position)
    {
      pos=section_tab[i].position;
      last=i;
      count=1;
    }
    else if (pos==section_tab[i].position)
      count++;
  }
  if ( (count>1) || id!=last )
  {
    section_tab[id].position=position(last)+size(last);
    section_tab[id].size=0;
    setFilePos(section_tab[id].position,SEEK_SET);
  }
  last_section_id=id;
  return true;
}

void CSMESFile::sectionInfo(int id,char *module, int max_module_lg, char *module_rel, int max_module_rel_lg, char *name, int max_name_lg,char *name_rel, int max_name_rel_lg,char *info, int max_user_lg,type_t &type,unsigned long &signature) const
{
  FUNCTION_TRACE;
  const char*_name;
  const char*_name_rel;
  const char*_module;
  const char*_module_rel;

  assoc_get(name_assoc,section_tab[id].name_id,_name,_name_rel);
  assoc_get(module_assoc,section_tab[id].module_id,_module,_module_rel);
  strncpy(name,_name,max_name_lg);
  strncpy(name_rel,_name_rel,max_name_rel_lg);
  strncpy(module,_module,max_module_lg);
  strncpy(module_rel,_module_rel,max_module_rel_lg);
  strncpy(info,section_tab[id].info,max_user_lg);
  type=section_tab[id].type;
  signature=section_tab[id].signature;
}

int CSMESFile::findLastSection()
{
  FUNCTION_TRACE;
  int i;
  OFFSET pos=0;
  if (last_section_id!=-1)
    return last_section_id;
  for (i=0;i<nb_section_tab;i++)
  {
    if  (pos<=section_tab[i].position)
    {
      last_section_id=i;
      pos=section_tab[i].position;
    }
  }
  return last_section_id;
}

void CSMESFile::create_section_link (int section_id,const char *module,const char *module_rel,const char *name,const char *name_rel,const char *info,type_t type,unsigned long signature,unsigned long flag)
{
  FUNCTION_TRACE;
  section_tab[nb_section_tab-1].name_id=assoc_insert(name_assoc,name,name_rel);
  section_tab[nb_section_tab-1].module_id=assoc_insert(module_assoc,module,module_rel);
  assoc_insert_associated_id(name_assoc,section_tab[nb_section_tab-1].name_id,nb_section_tab-1);
  assoc_insert_associated_id(module_assoc,section_tab[nb_section_tab-1].module_id,nb_section_tab-1);
  section_tab[nb_section_tab-1].info=STRDUP(info);
  section_tab[nb_section_tab-1].type=type;
  section_tab[nb_section_tab-1].signature=signature;
  section_tab[nb_section_tab-1].position=position(section_id);
  section_tab[nb_section_tab-1].size=size(section_id);
  section_tab[nb_section_tab-1].checksum=checksum(section_id);
  section_tab[nb_section_tab-1].flags=flag;
}

bool CSMESFile::merge(CSMESFile &csmes,unsigned long fl_merge,merge_policy_t policy,std::string &errmsg)
{

  if ( ! merge_precheck(csmes,fl_merge,policy,errmsg))
    return false;
  return  merge_internal(csmes,fl_merge,policy,errmsg);
}

void CSMESFile::merge_instrumentation_list(int section_id,std::vector<merge_precheck_info_t> &instrumentation,long &instrumentation_startindex)
{
  instrumentation.clear();
  instrumentation_startindex=0x7FFFFFFF;
  long instrumentation_size=0;
  if (openSectionID(section_id))
  {
    _I64 idd;
    CSMESFile::instrumentation_t instrumentationType                    ;
    std::vector<CSMESFile::instrumentation_part_t> instrumentation_part ;
    CSMESFile::instrumentation_childrens_t childrens                    ;
    long index                                                          ;
    long nb_index                                                       ;
    long startLineOrg                                                   ;
    long startLinePre                                                   ;
    long startColumnPre                                                 ;
    long startColumnOrg                                                 ;
    long endLineOrg                                                     ;
    long endLinePre                                                     ;
    long endColumnOrg                                                   ;
    long endColumnPre                                                   ;

    while (readInstrumentation(idd,index,nb_index, instrumentationType, startLineOrg, startColumnOrg, startLinePre, startColumnPre, endLineOrg, endColumnOrg, endLinePre, endColumnPre, instrumentation_part,childrens))
    {
      if (index<instrumentation_startindex)
        instrumentation_startindex=index;

      int new_size=index+nb_index;
      if (new_size>instrumentation_size)
        instrumentation_size=new_size;
    }
    closeSection();
  }
  else
    return;

  instrumentation.resize(instrumentation_size-instrumentation_startindex);
  if (openSectionID(section_id))
  {
    _I64 idd;
    CSMESFile::instrumentation_t instrumentationType                    ;
    std::vector<CSMESFile::instrumentation_part_t> instrumentation_part ;
    CSMESFile::instrumentation_childrens_t childrens                    ;
    long index                                                          ;
    long nb_index                                                       ;
    long startLineOrg                                                   ;
    long startLinePre                                                   ;
    long startColumnPre                                                 ;
    long startColumnOrg                                                 ;
    long endLineOrg                                                     ;
    long endLinePre                                                     ;
    long endColumnOrg                                                   ;
    long endColumnPre                                                   ;

    while (readInstrumentation(idd,index,nb_index, instrumentationType, startLineOrg, startColumnOrg, startLinePre, startColumnPre, endLineOrg, endColumnOrg, endLinePre, endColumnPre, instrumentation_part,childrens))
    {
      for (int i_inst=0;i_inst<nb_index;i_inst++)
      {
        instrumentation[i_inst+index-instrumentation_startindex].instrumentation=instrumentationType;
        instrumentation[i_inst+index-instrumentation_startindex].line=startLineOrg;
      }
    }
    closeSection();
  }
}

std::list<std::string>  CSMESFile::source_list(const char *module) const
{
  std::list<std::string> lst;
  int nb_sec=nbSections();
  for (int i=0;i<nb_sec;i++)
  {
    if (strcmp(sectionModule(i),module)==0)
      lst.push_back(std::string(sectionName(i)));
  }
  lst.sort();
  lst.unique();
  return lst;
}

std::string  CSMESFile::merge_precheck_source_list(const char *module,const char *increment) const
{
  std::list<std::string> lst=source_list(module);
  std::string msg=std::string(increment);
  msg+="Source file instrumented during the compilation of '"+std::string(module)+"' in the instrumentation database '"+fileName()+"'\n";
  for (std::list<std::string>::const_iterator it=lst.begin();it!=lst.end();++it)
  {
    msg+=increment;
    msg+="  ";
    msg+=*it;
    msg+='\n';
  }
  return msg;
}

bool CSMESFile::merge_precheck(CSMESFile &csmes,unsigned long fl_merge,merge_policy_t policy,std::string &errmsg) 
{
  FUNCTION_TRACE;
  int nb_sec=csmes.nbSections();
  std::list<std::string> msgs;
  for (int i=0;i<nb_sec;i++)
  {
    int section_id=findSectionID(csmes.sectionModule(i),
        csmes.sectionName(i),
        csmes.section_tab[i].info,
        csmes.section_tab[i].type);

    if (section_id!=-1)
    {
      if ( (csmes.size(i)!=size(section_id) ) && (csmes.section_tab[i].type==ORIGINAL)  )
      {
        msgs.push_back("Source file '"+std::string(csmes.sectionName(i))+"' is different (different size)");
      }
#if 0
      else if ( (csmes.checksum(i)!=checksum(section_id)) && (csmes.section_tab[i].type==ORIGINAL)  )
      {
        msgs.push_back("Source file '"+std::string(csmes.sectionName(i))+"' is different");
      }
#endif
      else if ( ( (csmes.size(i)!=size(section_id)) || (csmes.checksum(i)!=checksum(section_id)) ) && ((csmes.section_tab[i].type==_INSTRUMENTATION_V1) || (csmes.section_tab[i].type==_INSTRUMENTATION_V2))  )
      {
        std::vector<merge_precheck_info_t> csmes_instrumentation;
        long csmes_instrumentation_startindex;
        csmes.merge_instrumentation_list(i,csmes_instrumentation,csmes_instrumentation_startindex);

        std::vector<merge_precheck_info_t> instrumentation;
        long instrumentation_startindex;
        merge_instrumentation_list(section_id,instrumentation,instrumentation_startindex);

        if (instrumentation.size()!=csmes_instrumentation.size() || instrumentation_startindex!=csmes_instrumentation_startindex )
        {
          std::string msg=("Compilation of '"+std::string(csmes.sectionModule(i))+"': Instrumentation of source file '"+std::string(csmes.sectionName(i))+"' is different");
          if (source_list(csmes.sectionModule(i))!=csmes.source_list(csmes.sectionModule(i)))
          {
            msg+='\n';
            msg+="  Hint: a --cs-include-path command line option is certainly missing for the compilation of one of the objects\n";
            msg+=merge_precheck_source_list(csmes.sectionModule(i),"    ");
            msg+=csmes.merge_precheck_source_list(csmes.sectionModule(i),"    ");
          }
          msgs.push_back(msg);
        }
        else
        {
          int nb_inst=instrumentation.size();
          for (int i_inst=0;i_inst<nb_inst;i_inst++)
          {
            merge_precheck_info_t csmes_inst=csmes_instrumentation[i_inst];
            if (csmes_inst.instrumentation==_INSTRUMENTATION_NOP)
              continue;
            merge_precheck_info_t inst=instrumentation[i_inst];
            if (inst.instrumentation==_INSTRUMENTATION_NOP)
              continue;
            if (inst.instrumentation!=csmes_inst.instrumentation)
            {
              std::ostringstream msg;
              msg << "Compilation of '";
              msg << std::string(csmes.sectionModule(i));
              msg << "': Instrumentation of source file '";
              msg << std::string(csmes.sectionName(i));
              msg << "' (line ";
              msg << csmes_inst.line;
              msg << ") is different";
              msgs.push_back(msg.str());
              break;
            }
          }
        }
      }
      else if ( csmes.signature(i)!=signature(section_id) )
      {
        switch (csmes.section_tab[i].type)
        {
          case NONE:
          case EXECUTION:
          case EXECUTION_COMPACT:
          case EXECUTION_COMMENT:
          case COMMENT:
          case MANUAL_VALIDATION:
            break;
          case PREPROCESSED:
            msgs.push_back("Source file '"+std::string(csmes.sectionModule(i))+"' is compiled with different preprocessor options");
            break;
          case _FUNCTIONS_INFO_V2:
          case _FUNCTIONS_INFO_V1:
          case ORIGINAL:
            msgs.push_back("Source file '"+std::string(csmes.sectionName(i))+"' is different");
            break;
          case _INSTRUMENTATION_V1:
          case _INSTRUMENTATION_V2:
            msgs.push_back("Source file '"+std::string(csmes.sectionName(i))+"' is generated with different instrumentation settings");
            break;
        }
      }
    }
  }
  msgs.unique();
  errmsg.clear();
  for (std::list<std::string>::const_iterator it=msgs.begin();it!=msgs.end();++it)
  {
    if (it!=msgs.begin())
      errmsg+="\n";
    errmsg+=*it;
  }
  return errmsg.empty();
}

bool CSMESFile::merge_internal(CSMESFile &csmes,unsigned long fl_merge,merge_policy_t policy,std::string &errmsg)
{
  FUNCTION_TRACE;
  errmsg.clear();
  int nb_sec=csmes.nbSections();
  for (int i=0;i<nb_sec;i++)
  {
    int section_id=findSectionID(csmes.sectionModule(i),
        csmes.sectionName(i),
        csmes.section_tab[i].info,
        csmes.section_tab[i].type);

    const char*csmes_name;
    const char*csmes_name_rel;
    const char*csmes_module;
    const char*csmes_module_rel;

    assoc_get(csmes.name_assoc,csmes.section_tab[i].name_id,csmes_name,csmes_name_rel);
    assoc_get(csmes.module_assoc,csmes.section_tab[i].module_id,csmes_module,csmes_module_rel);

    int equivalent_index=-1;
    if (section_id==-1)
    {
      if (csmes.section_tab[i].type==ORIGINAL
          || csmes.section_tab[i].type==_FUNCTIONS_INFO_V2
          || csmes.section_tab[i].type==_FUNCTIONS_INFO_V1
          || csmes.section_tab[i].type==PREPROCESSED)
      {
        int name_id=assoc_find_absolute(name_assoc,csmes_name) ;
        if (name_id>=0)
        {
          const int *name_associated_id;
          int nb_name_associated_id;
          assoc_get_associated_id(name_assoc,name_id,nb_name_associated_id,name_associated_id);
          for (int jj=0;jj<nb_name_associated_id;jj++)
          {
            int j=name_associated_id[jj];
            if (  csmes.section_tab[i].type==section_tab[j].type
                && csmes.section_tab[i].size==section_tab[j].size
                && csmes.section_tab[i].checksum==section_tab[j].checksum )
            {
              ASSERT( name_id==section_tab[j].name_id);
              equivalent_index=j;
              break;
            }
          }
        }
#ifndef NO_DEBUG
        int verify_equivalent_index=-1;
        for (int j=0;j<nbSections();j++)
        {
          if ((  csmes.section_tab[i].type==section_tab[j].type
                && csmes.section_tab[i].size==section_tab[j].size
                && csmes.section_tab[i].checksum==section_tab[j].checksum
                && strcmp(csmes.sectionName(i),sectionName(j))==0))
          {
            verify_equivalent_index=j;
            break;
          }
        }
        ASSERT(verify_equivalent_index==equivalent_index);
#endif
      }
    }
    if (equivalent_index>=0)
    {
      nb_section_tab++;
      int sz=nb_section_tab*sizeof(section_t);
      section_tab=(section_t*)REALLOC((void*)section_tab,sz);
      const char*name;
      const char*name_rel;
      const char*module;
      const char*module_rel;

      assoc_get(csmes.name_assoc,csmes.section_tab[i].name_id,name,name_rel);
      assoc_get(csmes.module_assoc,csmes.section_tab[i].module_id,module,module_rel);
      section_tab[nb_section_tab-1].name_id=assoc_insert(name_assoc,name,name_rel);
      section_tab[nb_section_tab-1].module_id=assoc_insert(module_assoc,module,module_rel);
      assoc_insert_associated_id(name_assoc,section_tab[nb_section_tab-1].name_id,nb_section_tab-1);
      assoc_insert_associated_id(module_assoc,section_tab[nb_section_tab-1].module_id,nb_section_tab-1);
      section_tab[nb_section_tab-1].info=STRDUP(csmes.section_tab[i].info);
      section_tab[nb_section_tab-1].type=csmes.section_tab[i].type;
      section_tab[nb_section_tab-1].signature=csmes.signature(i);
      section_tab[nb_section_tab-1].position=position(equivalent_index);
      section_tab[nb_section_tab-1].size=size(equivalent_index);
      section_tab[nb_section_tab-1].checksum=checksum(equivalent_index);
      section_tab[nb_section_tab-1].flags=flags(equivalent_index)|fl_merge;
    }
    else
    {
      if (section_id!=-1)
      { // merge contains
        switch (csmes.section_tab[i].type)
        {
          case COMMENT:
          case EXECUTION_COMMENT:
          case MANUAL_VALIDATION:
            {
              long nb_comments=0;
              struct comments_t { long index; char *comment ; } ;
              comments_t *comments=NULL;
              long nb_validated=0;
              char *execution_comment=NULL;
              long *validated=NULL;

              if (openSectionID(section_id))
              {
                switch (csmes.section_tab[i].type)
                {
                  case EXECUTION_COMMENT:
                    { // read execution comment
                      if (!readExecutionComment(&execution_comment))
                      {
                        error_msg="Reading execution comments";
                        return false;
                      }
                    }
                    break;
                  case COMMENT:
                    { // read all comments
                      if (!readLong(nb_comments))
                      {
                        error_msg="Reading number of comments";
                        return false;
                      }
                      if (nb_comments==0)
                        break;
                      comments=(comments_t*)MALLOC(sizeof(comments_t)*nb_comments);
                      for (long icomment=0;icomment<nb_comments;icomment++)
                      {
                        if (!readComment(comments[icomment].index,&comments[icomment].comment))
                        {
                          error_msg="Reading comments";
                          return false;
                        }
                      }
                    }
                    break;
                  case MANUAL_VALIDATION:
                    { // read all manually validated index
                      if (!readLong(nb_validated))
                      {
                        error_msg="Reading number of manually validaded index";
                        return false;
                      }
                      if (nb_validated==0)
                        break;
                      validated=(long*)MALLOC(sizeof(long)*nb_validated);
                      for (long ivalidated=0;ivalidated<nb_validated;ivalidated++)
                      {
                        if (!readLong(validated[ivalidated]))
                        {
                          error_msg="Reading manually validated index";
                          return false;
                        }
                      }
                    }
                    break;
                  default:
                    break; // Not merging this type
                }
                closeSection();
                if (csmes.openSectionID(i))
                {
                  switch (csmes.section_tab[i].type)
                  {
                    case EXECUTION_COMMENT:
                      { // write execution comment
                        char *comment;
                        if (!csmes.readExecutionComment(&comment))
                        {
                          error_msg="Reading execution comments";
                          return false;
                        }
                        execution_comment=(char*)REALLOC(
                            execution_comment,
                            strlen(execution_comment)+1+strlen(comment)+10);
                        strcat(execution_comment,"<HR>");
                        strcat(execution_comment,comment);
                        FREE(comment);
                      }
                      break;
                    case COMMENT:
                      { // read all comments
                        long csmes_nb_comments;
                        if (!csmes.readLong(csmes_nb_comments))
                        {
                          error_msg="Reading number of comments";
                          return false;
                        }
                        if (csmes_nb_comments==0)
                          break;
                        for (long icsmescomment=0;icsmescomment<csmes_nb_comments;icsmescomment++)
                        {
                          long comment_id;
                          char *comment;
                          if (!csmes.readComment(comment_id,&comment))
                          {
                            error_msg="Reading comments";
                            return false;
                          }
                          bool found_comment=false;
                          for (long icomment=0;icomment<nb_comments;icomment++)
                          {
                            if (comments[icomment].index==comment_id)
                            {
                              found_comment=true;
                              comments[icomment].comment=(char*)REALLOC(
                                  comments[icomment].comment,
                                  strlen(comments[icomment].comment)+1+strlen(comment)+10);
                              strcat(comments[icomment].comment,"<HR>");
                              strcat(comments[icomment].comment,comment);
                              FREE(comment);
                            }
                          }
                          if (!found_comment)
                          {
                            nb_comments++;
                            comments=(comments_t*)REALLOC(comments,sizeof(comments_t)*nb_comments);
                            comments[nb_comments-1].index=comment_id;
                            comments[nb_comments-1].comment=comment;
                          }
                        }
                      }
                      break;
                    case MANUAL_VALIDATION:
                      { // read all manually validated index
                        long csmes_nb_validated;
                        if (!csmes.readLong(csmes_nb_validated))
                        {
                          error_msg="Reading number of manually validaded index";
                          return false;
                        }
                        if (csmes_nb_validated==0)
                          break;
                        for (long icsmesvalidated=0;icsmesvalidated<csmes_nb_validated;icsmesvalidated++)
                        {
                          long validated_index;
                          if (!csmes.readLong(validated_index))
                          {
                            error_msg="Reading manually validated index";
                            return false;
                          }
                          bool found_validated=false;
                          for (long ivalidated=0;ivalidated<nb_validated;ivalidated++)
                          {
                            if (validated[ivalidated]==validated_index)
                              found_validated=true;
                          }
                          if (!found_validated)
                          {
                            nb_validated++;
                            validated=(long*)MALLOC(sizeof(long)*nb_validated);
                            validated[nb_validated-1]=validated_index;
                          }
                        }
                      }

                      break;
                    default:
                      break; // Not merging this type
                  }
                  csmes.closeSection();
                }
              }
              else
              {
                error_msg="Reading section  failed";
                return false;
              }
              if (openSection(csmes_module,
                    csmes_module_rel,
                    csmes_name,
                    csmes_name_rel,
                    csmes.section_tab[i].info,
                    csmes.section_tab[i].type,
                    csmes.section_tab[i].signature,
                    NEW_RW,
                    csmes.section_tab[i].flags|fl_merge))
              {
                switch (csmes.section_tab[i].type)
                {
                  case EXECUTION_COMMENT:
                    { // write all comments
                      if (!writeExecutionComment(execution_comment))
                      {
                        error_msg="Writing comments";
                        return false;
                      }
                      FREE(execution_comment);
                    }
                    break;
                  case COMMENT:
                    { // write all comments
                      if (!writeLong(nb_comments))
                      {
                        error_msg="Writing number of comments";
                        return false;
                      }
                      for (long icomment=0;icomment<nb_comments;icomment++)
                      {
                        if (!writeComment(comments[icomment].index,comments[icomment].comment))
                        {
                          error_msg="Writing comments";
                          return false;
                        }
                        FREE(comments[icomment].comment);
                      }
                      FREE(comments);
                    }
                    break;
                  case MANUAL_VALIDATION:
                    { // read all comments
                      if (!writeLong(nb_validated))
                      {
                        error_msg="Writing number of manually validated index";
                        return false;
                      }
                      for (long ivalidated=0;ivalidated<nb_validated;ivalidated++)
                      {
                        if (!writeLong(validated[ivalidated]))
                        {
                          error_msg="Writing manually validated index";
                          return false;
                        }
                      }
                      FREE(validated);
                    }
                    break;
                  default:
                    break; // Not merging this type
                }
                closeSection();
              }
              else
              {
                error_msg="Creation of the section failed";
                return false;
              }
            }
            break;
          default:
            break; // Not merging this type
        }
      }
      else
      { // new section
        bool import_section= false;
        type_t type=csmes.section_tab[i].type;
        if (policy==FULL)
          import_section=true;
        else if (type==EXECUTION || type==EXECUTION_COMPACT || type==EXECUTION_COMMENT || type==COMMENT || type==MANUAL_VALIDATION)
          import_section=true;
        else if (type==PREPROCESSED || type==ORIGINAL || type==_INSTRUMENTATION_V1 || type==_INSTRUMENTATION_V2)
        {
          for (int id=0;id<nbSections();id++)
          {
            if  (sectionType(id)==_INSTRUMENTATION_V1 || sectionType(id)==_INSTRUMENTATION_V2)
            {
              const char *name=sectionName(id);
              if ( strcmp(name,csmes_name)==0)
              {
                if (openSectionID(id))
                {
                  _I64 idd;
                  CSMESFile::instrumentation_t instrumentationType                    ;
                  std::vector<CSMESFile::instrumentation_part_t> instrumentation_part ;
                  CSMESFile::instrumentation_childrens_t childrens                    ;
                  long index                                                          ;
                  long nb_index                                                       ;
                  long startLineOrg                                                   ;
                  long startLinePre                                                   ;
                  long startColumnPre                                                 ;
                  long startColumnOrg                                                 ;
                  long endLineOrg                                                     ;
                  long endLinePre                                                     ;
                  long endColumnOrg                                                   ;
                  long endColumnPre                                                   ;

                  while (readInstrumentation(idd,index,nb_index, instrumentationType, startLineOrg, startColumnOrg, startLinePre, startColumnPre, endLineOrg, endColumnOrg, endLinePre, endColumnPre, instrumentation_part,childrens))
                  {
                    if (instrumentationType!=_INSTRUMENTATION_NOP)
                    {
                      import_section=true;
                      break;
                    }
                  }
                  closeSection();
                  if ( import_section)
                    break;
                }
              }
            }
          }
        }

        if (import_section)
        {
          if (openSection(csmes_module,
                csmes_module_rel,
                csmes_name,
                csmes_name_rel,
                csmes.section_tab[i].info,
                csmes.section_tab[i].type,
                csmes.section_tab[i].signature,
                NEW_RW,
                csmes.section_tab[i].flags|fl_merge))
          {
            char *buf=(char*)MALLOC(static_cast<size_t>(csmes.size(i)));
            if (csmes.openSectionID(i))
            {
              csmes.readCharTab(buf,csmes.size(i));
              writeCharTab(buf,csmes.size(i));
              FREE(buf);
              csmes.closeSection();
              closeSection();
            }
            else
            {
              error_msg="Reading section  failed";
              return false;
            }
          }
          else
          {
            error_msg="Creation of the section failed";
            return false;
          }
        }
        else if (type==_INSTRUMENTATION_V1 || type==_INSTRUMENTATION_V2)
        { // Create a section with NOP instrumentation
          if (openSection(csmes_module,
                csmes_module_rel,
                csmes_name,
                csmes_name_rel,
                csmes.section_tab[i].info,
                INSTRUMENTATION,
                csmes.section_tab[i].signature,
                NEW_RW,
                csmes.section_tab[i].flags|fl_merge))
          {
            if (csmes.openSectionID(i))
            {
              _I64 id;
              CSMESFile::instrumentation_t instrumentationType                    ;
              std::vector<CSMESFile::instrumentation_part_t> instrumentation_part ;
              CSMESFile::instrumentation_childrens_t childrens                    ;
              long index                                                          ;
              long nb_index                                                       ;
              long startLineOrg                                                   ;
              long startLinePre                                                   ;
              long startColumnPre                                                 ;
              long startColumnOrg                                                 ;
              long endLineOrg                                                     ;
              long endLinePre                                                     ;
              long endColumnOrg                                                   ;
              long endColumnPre                                                   ;

              while (csmes.readInstrumentation(id,index, nb_index, instrumentationType, startLineOrg, startColumnOrg, startLinePre, startColumnPre, endLineOrg, endColumnOrg, endLinePre, endColumnPre, instrumentation_part,childrens))
              {
                writeInstrumentation(id,index, _INSTRUMENTATION_NOP, startLineOrg, startColumnOrg, startLinePre, startColumnPre, endLineOrg, endColumnOrg, endLinePre, endColumnPre, instrumentation_part,childrens);
                if (instrumentationType==_INSTRUMENTATION_CONDITION_FULL)
                  writeInstrumentation(id,index+1, _INSTRUMENTATION_NOP, startLineOrg, startColumnOrg, startLinePre, startColumnPre, endLineOrg, endColumnOrg, endLinePre, endColumnPre, instrumentation_part,childrens);
              }

              csmes.closeSection();
              closeSection();
            }
            else
            {
              error_msg="Reading section  failed";
              return false;
            }
          }
          else
          {
            error_msg="Creation of the section failed";
            return false;
          }
        }
      }
    }
  }
  return true;
}

bool CSMESFile::append(CSMESFile &csmes)
{
  FUNCTION_TRACE;
  int nb_sec=csmes.nbSections();
  for (int i=0;i<nb_sec;i++)
  {
    const char*name;
    const char*name_rel;
    const char*module;
    const char*module_rel;

    assoc_get(csmes.name_assoc,csmes.section_tab[i].name_id,name,name_rel);
    assoc_get(csmes.module_assoc,csmes.section_tab[i].module_id,module,module_rel);

    if (openSection(module,
          module_rel,
          name,
          name_rel,
          csmes.section_tab[i].info,
          csmes.section_tab[i].type,
          csmes.section_tab[i].signature,
          NEW_RW,
          csmes.section_tab[i].type))
    {
      char *buf=(char*)MALLOC(static_cast<size_t>(csmes.section_tab[i].size));
      if (csmes.openSectionID(i))
      {
        csmes.readCharTab(buf,csmes.section_tab[i].size);
        writeCharTab(buf,csmes.section_tab[i].size);
        FREE(buf);
        csmes.closeSection();
        closeSection();
      }
      else
      {
        error_msg="Reading section failed";
        return false;
      }
    }
    else
    {
      error_msg="Creation of the section failed";
      return false;
    }
  }
  return true;
}


bool CSMESFile::writeInstrumentation(_I64 identifier,long index, instrumentation_t t, long s_line_org, long s_column_org,long s_line_pre, long s_column_pre, long e_line_org,long e_column_org, long e_line_pre, long e_column_pre, const std::vector<CSMESFile::instrumentation_part_t> &instrumentation_part, const instrumentation_childrens_t &childrens)
{
  FUNCTION_TRACE;
  long ti=(long)t;
  writeLong(index) ;
  writeLong(ti) ;
  writeLong(s_line_org) ;
  writeLong(s_column_pre) ;
  writeLong(e_line_org) ;
  bool result = writeLong(e_column_pre) ;
  switch (sectionType())
  {
    case _INSTRUMENTATION_V1:
      break;
    case _INSTRUMENTATION_V2:
      writeLong(s_line_pre);
      writeLong(s_column_org);
      writeLong(e_line_pre);
      writeLong(e_column_org);
      writeLongLong(identifier);
      writeLong(instrumentation_part.size()) ;
      for (int i=0;i<(int)instrumentation_part.size();i++)
      {
        writeLong(instrumentation_part[i].type) ;
        writeCString(instrumentation_part[i].description.c_str()) ;
        writeLong(instrumentation_part[i].items.size()) ;
        int j;
        for (j=0;j<(int)instrumentation_part[i].items.size();j++)
        {
          writeLong(instrumentation_part[i].items[j].type) ;
          writeCString(instrumentation_part[i].items[j].description.c_str()) ;
          writeLong(instrumentation_part[i].items[j].s_line_org) ;
          writeLong(instrumentation_part[i].items[j].s_column_org) ;
          writeLong(instrumentation_part[i].items[j].s_line_pre) ;
          writeLong(instrumentation_part[i].items[j].s_column_pre) ;
          writeLong(instrumentation_part[i].items[j].e_line_org) ;
          writeLong(instrumentation_part[i].items[j].e_column_org) ;
          writeLong(instrumentation_part[i].items[j].e_line_pre) ;
          writeLong(instrumentation_part[i].items[j].e_column_pre) ;
        }
      }
      writeLong(childrens.type) ;
      result = writeLong(childrens.childrens.size()) ;
      for (int j=0;j<(int)childrens.childrens.size();j++)
      {
        writeLong(childrens.childrens[j].type) ;
        result = writeLongLong(childrens.childrens[j].identifier) ;
      }
      break;
    default:
      ASSERT(false);
  }
  return result;
}

bool CSMESFile::readInstrumentation(_I64 &identifier, long &index, long &nb_index, instrumentation_t &t, long &s_line_org,long &s_column_org,long &s_line_pre, long &s_column_pre, long &e_line_org, long &e_column_org, long &e_line_pre, long &e_column_pre, std::vector<CSMESFile::instrumentation_part_t> &instrumentation_part, instrumentation_childrens_t &childrens)
{
  FUNCTION_TRACE;
  long ti;
  nb_index=0;

  long _nb_index;
  if (!readLong(index)) return false;
  if (!readLong(ti)) return false;
  t=(instrumentation_t)ti;
  if (!readLong(s_line_org)) return false;
  if (!readLong(s_column_pre)) return false;
  if (!readLong(e_line_org)) return false;
  if (!readLong(e_column_pre)) return false;
  switch(sectionType())
  {
    case _INSTRUMENTATION_V1:
      identifier=0;
      s_line_pre=s_line_org;
      e_line_pre=e_line_org;
      s_column_org=-1;
      e_column_org=-1;
      instrumentation_part.resize(0);
      childrens.type=CHILDRENS_NONE;
      childrens.childrens.clear();
      break;
    case _INSTRUMENTATION_V2:
      if (!readLong(s_line_pre)) return false;
      if (!readLong(s_column_org)) return false;
      if (!readLong(e_line_pre)) return false;
      if (!readLong(e_column_org)) return false;
      if (!readLongLong(identifier)) return false;
      if (!readLong(_nb_index)) return false;
      instrumentation_part.resize(_nb_index);
      for (int i=0;i<(int)instrumentation_part.size();i++)
      {
        if (!readLong(ti)) return false;
        instrumentation_part[i].type=(type_part_t)ti;
        char *desc=NULL;
        if (!readCString(&desc)) return false;
        instrumentation_part[i].description=std::string(desc);
        FREE(desc);
        if (!readLong(_nb_index)) return false;
        instrumentation_part[i].items.resize(_nb_index);
        int j;
        for (j=0;j<(int)instrumentation_part[i].items.size();j++)
        {
          if (!readLong(ti)) return false;
          instrumentation_part[i].items[j].type=(type_item_t)ti;
          char *desc=NULL;
          if (!readCString(&desc)) return false;
          instrumentation_part[i].items[j].description=std::string(desc);
          FREE(desc);
          if (!readLong(instrumentation_part[i].items[j].s_line_org)) return false;
          if (!readLong(instrumentation_part[i].items[j].s_column_org)) return false;
          if (!readLong(instrumentation_part[i].items[j].s_line_pre)) return false;
          if (!readLong(instrumentation_part[i].items[j].s_column_pre)) return false;
          if (!readLong(instrumentation_part[i].items[j].e_line_org)) return false;
          if (!readLong(instrumentation_part[i].items[j].e_column_org)) return false;
          if (!readLong(instrumentation_part[i].items[j].e_line_pre)) return false;
          if (!readLong(instrumentation_part[i].items[j].e_column_pre)) return false;
        }
      }
      if (!readLong(ti)) return false;
      childrens.type=(type_childrens_t)ti;
      if (!readLong(_nb_index)) return false;
      childrens.childrens.resize(_nb_index);
      for (int j=0;j<(int)childrens.childrens.size();j++)
      {
        if (!readLong(ti)) return false;
        childrens.childrens[j].type=(type_children_t)ti;
        if (!readLongLong(childrens.childrens[j].identifier)) return false;
      }
      break;
    default:
      ASSERT(false);
      return false;
  }

  switch (t)
  {
    case _INSTRUMENTATION_CONDITION_FULL:
      nb_index=2;
      break;
    case _INSTRUMENTATION_CONDITION_FALSE_ONLY:
    case _INSTRUMENTATION_LABEL:
    case _INSTRUMENTATION_CONDITION_TRUE_ONLY:
    case _INSTRUMENTATION_NOP:
    case _INSTRUMENTATION_COMPUTATION:
      nb_index=1;
      break;
  }

  return true;
}

bool CSMESFile::readBuffer()
{
  FUNCTION_TRACE;
  int nb_read=0;
  if (!setFilePos(section_tab[current_section].position,SEEK_SET))
    return false;
  allocateBuffer(static_cast<size_t>(size()));
  while (nb_read<buffer_size)
  {
    int rd=readFile(&(buffer[nb_read]),1,buffer_size-nb_read);
    if (rd<=0)
    {
      if (!feof(f))
      {
        error_msg="IO Error";
        return false;
      }
      else
      {
        error_msg="Premature EOF of section";
        return false;
      }
    }
    nb_read+=rd;
  }
  return true;
}


void CSMESFile::char2long(const char *t,long &l)
{
  FUNCTION_TRACE;
 l= ((unsigned char)t[3] )
   | (((unsigned char)t[2])<<8)
   | (((unsigned char)t[1])<<16)
   | (((unsigned char)t[0])<<24);
}

void CSMESFile::long2char(long l,char *t)
{
  FUNCTION_TRACE;
  t[3]=(char)(l&0xFF);
  t[2]=(char)( (l&0xFF00) >> 8 );
  t[1]=(char)( (l&0xFF0000) >> 16 );
  t[0]=(char)( (l&0xFF000000) >> 24 );
}

void CSMESFile::char2longlong(const char *t,_I64 &l)
{
  FUNCTION_TRACE;
  long l1,l2;
  _I64 ll1,ll2;
  char2long(t,l1);
  char2long(&t[4],l2);
  ll1=l1;
  ll2=l2;
  l=(ll1<<32)|(ll2&0xFFFFFFFF);
}

void CSMESFile::longlong2char(_I64 l,char *t)
{
  FUNCTION_TRACE;
  long l1,l2;
  _I64 ll1,ll2;
  ll1=l>>32;
  ll2=l&0xFFFFFFFF;
  l1=static_cast<long>(ll1);
  l2=static_cast<long>(ll2);
  long2char(l1,t);
  long2char(l2,&t[4]);
}

_I64 CSMESFile::nbExecution() const
{
  FUNCTION_TRACE;
  const long record_size=4;
  OFFSET sz=size()-4;
#ifndef NO_DEBUG
  if (sz<0)
  {
    error_msg="Wrong length for the instrumentation";
    return -1;
  }
  if ((sz%record_size)!=0)
  {
    error_msg="Wrong length for the instrumentation";
    return -1;
  }
#endif
  return (sz/record_size);
}

bool CSMESFile::readExecutionStatus (long &execution_status) const
{
  FUNCTION_TRACE;
  if (sectionType()==EXECUTION || sectionType()==EXECUTION_COMPACT)
  {
    execution_status= sectionSignature();
    return true;
  }

  return false;
}


bool CSMESFile::readExecutionStatus (int sec_id,long &execution_status) const
{
  FUNCTION_TRACE;
  if (sectionType(sec_id)==EXECUTION || sectionType(sec_id)==EXECUTION_COMPACT)
  {
    execution_status= sectionSignature(sec_id);
    return true;
  }

  return false;
}

bool CSMESFile::readExecutionStatus (const char *name,long &execution_status) const
{
  FUNCTION_TRACE;
  int nbsec=nbSections();
  for (int i=0;i<nbsec;i++)
  {
    if (strcmp(sectionName(i),name)==0 && (sectionType(i)==EXECUTION || sectionType(i)==EXECUTION_COMPACT) )
    {
      execution_status= sectionSignature(i);
      return true;
    }
  }
  return false;
}

bool CSMESFile::writeExecutionStatus(int sec_id, long execution_status)
{
  FUNCTION_TRACE;
  if ((sectionType(sec_id)==EXECUTION || sectionType(sec_id)==EXECUTION_COMPACT) )
    section_tab[sec_id].signature=execution_status;
  return true;
}


bool CSMESFile::writeExecutionStatus(const char *name, long execution_status)
{
  FUNCTION_TRACE;
  int nbsec=nbSections();
  for (int i=0;i<nbsec;i++)
  {
    if (strcmp(sectionName(i),name)==0 && (sectionType(i)==EXECUTION || sectionType(i)==EXECUTION_COMPACT) )
      section_tab[i].signature=execution_status;
  }
  return true;
}

bool CSMESFile::readExecutionCompact(char **module,long **execution,int *nb)
{
  FUNCTION_TRACE;
  long save_nb;
  if (!readLong(save_nb))
    return false;
  *nb=save_nb;
  *execution=(long*)(MALLOC(sizeof(long)*save_nb));
  for(int i=0;i<save_nb;i++)
  {
    if (!readLong( (*execution) [i]))
      return false;
  }
  if (!readCString(module))
    return false;

  return true;
}

void CSMESFile::writeExecutionCompact(const char *module,const long *execution,int nb)
{
  FUNCTION_TRACE;
  writeLong(nb);
  for(int i=0;i<nb;i++)
    writeLong(execution[i]);
  writeCString(module);
}

bool CSMESFile::readExecution(long *execution,int nb)
{
  FUNCTION_TRACE;
  long save_nb;
  if (!readLong(save_nb))
    return false;
  if (nb!=save_nb)
      return false;
  for(int i=0;i<nb;i++)
    if (!readLong(execution[i]))
      return false;

  return true;
}

void CSMESFile::writeExecution(const long *execution,int nb)
{
  FUNCTION_TRACE;
  writeLong(nb);
  for(int i=0;i<nb;i++)
    writeLong(execution[i]);
}

const char *CSMESFile::sectionNameRelative(int id) const
{
  FUNCTION_TRACE;
  const char*relative;
  const char*absolute;

  assoc_get(name_assoc,section_tab[id].name_id,absolute,relative);
  return relative;
}

const char *CSMESFile::sectionName(int id) const
{
  FUNCTION_TRACE;
  const char*relative;
  const char*absolute;

  assoc_get(name_assoc,section_tab[id].name_id,absolute,relative);
  return absolute;
}

const char *CSMESFile::sectionInfo(int id)const
{
  FUNCTION_TRACE;
  return section_tab[id].info;
}

const char *CSMESFile::sectionModuleRelative(int id) const
{
  FUNCTION_TRACE;
  const char*relative;
  const char*absolute;

  assoc_get(module_assoc,section_tab[id].module_id,absolute,relative);
  return relative;
}

const char *CSMESFile::sectionModule(int id) const
{
  FUNCTION_TRACE;
  const char*relative;
  const char*absolute;

  assoc_get(module_assoc,section_tab[id].module_id,absolute,relative);
  return absolute;
}

CSMESFile::type_t CSMESFile::sectionType(int id) const
{
  FUNCTION_TRACE;
  return section_tab[id].type;
}

unsigned long  CSMESFile::sectionSignature(int id) const
{
  FUNCTION_TRACE;
  return section_tab[id].signature;
}

void CSMESFile::allocateBuffer(size_t size)
{
  FUNCTION_TRACE;
  if (size==0)
  {
    if (buffer!=NULL)
    {
      FREE(buffer);
      buffer=NULL;
    }
    nb_allocated=0;
  }
  else
  {
    if (size>nb_allocated)
    {
      nb_allocated=size;
      buffer=(char*)REALLOC(buffer,nb_allocated);
    }
  }
  buffer_size=size;
}




bool CSMESFile::writeCString(const char *string_p)
{
  FUNCTION_TRACE;
  return writeCharTab(string_p,strlen(string_p)+1);
}

bool CSMESFile::readCString(char **string_p)
{
  FUNCTION_TRACE;
  if (buffer==NULL)
  {
    buffer_pos=0;
    if (!readBuffer())
      return false;
  }
  *string_p=STRDUP(&buffer[buffer_pos]);
  buffer_pos+=strlen(*string_p)+1;
  return true;
}

bool CSMESFile::readFunctionInfo(char **scoped_name_p,char **prototype_p, long &s_line_org, long &s_column_org,long &s_line_pre, long &s_column_pre, long &e_line_org, long &e_column_org,long &e_line_pre, long &e_column_pre)
{
  FUNCTION_TRACE;
  readCString(scoped_name_p);
  readCString(prototype_p);
  readLong(s_line_org) ;
  readLong(s_column_pre) ;
  readLong(e_line_org) ;
  bool result = readLong(e_column_pre) ;
  switch (sectionType())
  {
    case _FUNCTIONS_INFO_V1:
      s_line_pre=s_line_org;
      e_line_pre=e_line_org;
      s_column_org=-1;
      e_column_org=-1;
      break;
    case _FUNCTIONS_INFO_V2:
      readLong(s_line_pre) ;
      readLong(s_column_org) ;
      readLong(e_line_pre) ;
      result = readLong(e_column_org) ;
      break;
    default:
      ASSERT(false);
      break;
  }
  return result;
}

bool CSMESFile::writeFunctionInfo(const char *scoped_name,const char *prototype,long s_line_org,long s_column_org,long s_line_pre, long s_column_pre, long e_line_org,long e_column_org,  long e_line_pre, long e_column_pre)
{
  FUNCTION_TRACE;
  writeCharTab(scoped_name,strlen(scoped_name)+1) ;
  writeCharTab(prototype,strlen(prototype)+1) ;
  writeLong(s_line_org) ;
  writeLong(s_column_pre) ;
  writeLong(e_line_org) ;
  bool result = writeLong(e_column_pre) ;
  switch (sectionType())
  {
    default:
      ASSERT(false);
      break;
    case _FUNCTIONS_INFO_V1:
      break;
    case _FUNCTIONS_INFO_V2:
      writeLong(s_line_pre) ;
      writeLong(s_column_org) ;
      writeLong(e_line_pre) ;
      result = writeLong(e_column_org) ;
      break;
  }
  return result;
}

bool CSMESFile::writeExecutionComment(const char*comment)
{
  FUNCTION_TRACE;
  ASSERT(sectionType()==EXECUTION_COMMENT);
  long lg=strlen(comment)+1;
  writeLong(lg);
  return  writeCharTab(comment,lg);
}

bool CSMESFile::readExecutionComment(char**comment)
{
  FUNCTION_TRACE;
  ASSERT(sectionType()==EXECUTION_COMMENT);
  long lg;
  if (!readLong(lg))
    return false;
  *comment=(char*)MALLOC(lg+1);
  bool res = (readCharTab(*comment,lg)==lg);
  (*comment)[lg]='\0';
  return res;
}

bool CSMESFile::writeComment(long index,const char*comment)
{
  FUNCTION_TRACE;
  ASSERT(sectionType()==COMMENT);
  long lg=strlen(comment)+1;
  writeLong(index);
  writeLong(lg);
  return  writeCharTab(comment,lg);
}

bool CSMESFile::readComment(long &index,char**comment)
{
  FUNCTION_TRACE;
  ASSERT(sectionType()==COMMENT);
  long lg;
  readLong(index);
  if (!readLong(lg))
    return false;
  *comment=(char*)MALLOC(lg+1);
  bool res = (readCharTab(*comment,lg)==lg);
  (*comment)[lg]='\0';
  return res;
}

_I64 CSMESFile::getFilePos() const
{
  FUNCTION_TRACE;
  return FTELL(f);
}

bool CSMESFile::setFilePos(_I64 pos,int move)
{
  FUNCTION_TRACE;
  bool res = FSEEK(f,pos,move)==0;

  return res;
}


size_t CSMESFile::readFile(void *data,size_t size,size_t elem) const
{
  FUNCTION_TRACE;
  size_t nb_read = fread(data,size,elem,f);

  return nb_read;
}

size_t CSMESFile::writeFile(const void *data,size_t size,size_t elem)
{
  FUNCTION_TRACE;
  size_t nb_written = fwrite(data,size,elem,f);

  return nb_written;
}

bool CSMESFile::save_blackbox(const char *filename)
{
  FUNCTION_TRACE;
  CSMESFile csmes_output;
  if (!csmes_output.open(filename,NEW_RW))
    return false;
  int nb_sec=nbSections();
  for (int i=0;i<nb_sec;i++)
  {
    switch (section_tab[i].type)
    {
      default:
        break;
      case _INSTRUMENTATION_V1:
      case _INSTRUMENTATION_V2:
        {
          const char*name;
          const char*name_rel;
          const char*module;
          const char*module_rel;

          assoc_get(name_assoc,section_tab[i].name_id,name,name_rel);
          assoc_get(module_assoc,section_tab[i].module_id,module,module_rel);
          if (csmes_output.openSection(module,
                module_rel,
                name,
                name_rel,
                section_tab[i].info,
                section_tab[i].type,
                section_tab[i].signature,
                NEW_RW,
                section_tab[i].flags))
          {
            char *buf=(char*)MALLOC(static_cast<size_t>(size(i)));
            if (openSection(module,
                  module_rel,
                  name,
                  name_rel,
                  section_tab[i].info,
                  section_tab[i].type,
                  section_tab[i].signature,
                  RO,0 /* flags ignored */))
            {
              readCharTab(buf,size(i));
              csmes_output.writeCharTab(buf,size(i));
              FREE(buf);
              csmes_output.closeSection();
              closeSection();
            }
            else
            {
              error_msg="Reading section  failed";
              return false;
            }
          }
          else
          {
            error_msg="Creation of the section failed";
            return false;
          }
          break;
        }
    }
  }
  csmes_output.close();
  return true;
}

bool CSMESFile::isCoverageBranchOnly()
{
  FUNCTION_TRACE;
  int nb_sec=nbSections();


  if (nb_sec==0)
    return false;
  bool branch_only=true;
  for (int i=0;i<nb_sec;i++)
  {
    switch (section_tab[i].type)
    {
      case _INSTRUMENTATION_V1:
      case _INSTRUMENTATION_V2:
        {
          long index;
          if (openSectionID(i))
          {
            for (;branch_only;)
            {
              instrumentation_t type;
              instrumentation_childrens_t childrens;
              long start_column_org,end_column_org,start_line_org,end_line_org,start_line,start_column,end_line,end_column;
              std::vector<instrumentation_part_t> instrumentation_part;
              _I64 identifier;
              long nb_index;
              if (readInstrumentation(identifier,index,nb_index,type,start_line_org,start_column_org,start_line,start_column,end_line_org,end_column_org,end_line,end_column,instrumentation_part,childrens))
              {
                switch(type)
                {
                  default:
                  case _INSTRUMENTATION_CONDITION_FULL:
                  case _INSTRUMENTATION_LABEL:
                  case _INSTRUMENTATION_CONDITION_FALSE_ONLY:
                  case _INSTRUMENTATION_CONDITION_TRUE_ONLY:
                    branch_only=false;
                    break;
                  case _INSTRUMENTATION_COMPUTATION:
                  case _INSTRUMENTATION_NOP:
                    break;
                }
              }
              else
                break;
            }
            closeSection();
          }
        }
        break;
      default:
        break;
    }
    if (!branch_only)
      return branch_only;
  }
  return branch_only;
}

bool CSMESFile::isBlackBox() const
{
  FUNCTION_TRACE;
  int nb_sec=nbSections();

  if (nb_sec==0)
    return false;
  for (int i=0;i<nb_sec;i++)
  {
    switch (section_tab[i].type)
    {
      case ORIGINAL:
      case PREPROCESSED:
        return false;
      default:
        break;
    }
  }
  return true;
}

void CSMESFile::assoc_init(assoc_t &assoc)
{
  FUNCTION_TRACE;
  assoc.nb=0;
  assoc.last_id=0;
  assoc.data=NULL;
}

void CSMESFile::assoc_clear(assoc_t &assoc)
{
  FUNCTION_TRACE;
  for (int i=0;i<assoc.nb;i++)
  {
    FREE(assoc.data[i].absolute);
    FREE(assoc.data[i].relative);
    FREE(assoc.data[i].associated_id);
  }
  assoc.nb=0;
  FREE(assoc.data);
  assoc.data=NULL;
}

int CSMESFile::assoc_insert(assoc_t &assoc,const char *absolute,const char *relative)
{
  FUNCTION_TRACE;
  for (int i=assoc.nb-1;i>=0;i--)
  {
    if (strcmp(absolute,assoc.data[i].absolute)==0)
    {
      //ASSERT((strcmp(relative,"")==0) || (strcmp(relative,assoc.data[i].relative)==0));
      assoc.last_id=i;
      return i;
    }
  }
  if ( (assoc.nb&0xFF) == 0 )
    assoc.data=(assoc_data_t*)REALLOC(assoc.data,(assoc.nb+257)*sizeof(assoc_data_t));
  assoc.data[assoc.nb].relative=STRDUP(relative);
  assoc.data[assoc.nb].absolute=STRDUP(absolute);
  assoc.data[assoc.nb].associated_id=NULL;
  assoc.data[assoc.nb].nb_associated_id=0;
  assoc.last_id=assoc.nb;
  assoc.nb++;
  return assoc.nb-1;
}

int CSMESFile::assoc_find_absolute(const assoc_t &assoc,const char *absolute)
{
  FUNCTION_TRACE;
  for (int i=assoc.last_id;i<assoc.nb;i++)
  {
    if (strcmp(absolute,assoc.data[i].absolute)==0)
    {
      assoc.last_id=i;
      return i;
    }
  }
  int assoc_max=assoc.nb;
  if (assoc_max>assoc.last_id)
    assoc_max=assoc.last_id;

  for (int i=0;i<assoc_max;i++)
  {
    if (strcmp(absolute,assoc.data[i].absolute)==0)
    {
      assoc.last_id=i;
      return i;
    }
  }
  return -1;
}

void CSMESFile::assoc_get(const assoc_t &assoc,int id,const char *&absolute,const char *&relative)
{
  FUNCTION_TRACE;
  if (id<0 || id>=assoc.nb)
  {
    absolute=NULL;
    relative=NULL;
    return;
  }

  absolute=assoc.data[id].absolute;
  relative=assoc.data[id].relative;
}

void CSMESFile::assoc_get_associated_id(const assoc_t &assoc,int id,int &nb_associated_id,const int *&associated_id)
{
  FUNCTION_TRACE;
  nb_associated_id=assoc.data[id].nb_associated_id;
  associated_id=assoc.data[id].associated_id;
}

void CSMESFile::assoc_clear_associated_id(assoc_t &assoc,int id)
{
  FUNCTION_TRACE;
  assoc.data[id].nb_associated_id=0;
  FREE(assoc.data[id].associated_id);
  assoc.data[id].associated_id=NULL;
}

void CSMESFile::assoc_insert_associated_id(assoc_t &assoc,int id,int associated_id)
{
  FUNCTION_TRACE;
  if ( (assoc.data[id].nb_associated_id&0xFF) == 0 )
    assoc.data[id].associated_id=(int*)REALLOC(assoc.data[id].associated_id,(assoc.data[id].nb_associated_id+257)*sizeof(int));

  assoc.data[id].associated_id[assoc.data[id].nb_associated_id]=associated_id;
  assoc.data[id].nb_associated_id++;
}

void CSMESFile::assoc_clear_associated_id(assoc_t &assoc)
{
  FUNCTION_TRACE;
  for (int i=0;i<assoc.nb;i++)
  {
    FREE(assoc.data[i].associated_id);
    assoc.data[i].nb_associated_id=0;
    assoc.data[i].associated_id=NULL;
  }
}

void CSMESFile::generateAssociatedID()
{
  FUNCTION_TRACE;
  assoc_clear_associated_id(module_assoc);
  assoc_clear_associated_id(name_assoc);
  int nb_sec=nbSections();
  for (int i=0;i<nb_sec;i++)
  {
    const char *name=sectionName(i);
    const char *module=sectionModule(i);
    int name_id=assoc_find_absolute(name_assoc,name);
    if (name_id>=0)
      assoc_insert_associated_id(name_assoc,name_id,i);
    int module_id=assoc_find_absolute(module_assoc,module);
    if (module_id>=0)
      assoc_insert_associated_id(module_assoc,module_id,i);
  }
}

void CSMESFile::add_missing_sources()
{
  FUNCTION_TRACE;
  const char *absolute=NULL;
  const char *relative=NULL;
  int name_id=0;
  for (;;)
  {
    assoc_get(name_assoc,name_id,absolute,relative);
    if (absolute)
    {
      const int *name_associated_id;
      int nb_name_associated_id;
      assoc_get_associated_id(name_assoc,name_id,nb_name_associated_id,name_associated_id);
      for (int ii=0;ii<nb_name_associated_id;ii++)
      {
        int original_source_id=-1;
        int ipre=name_associated_id[ii];
        if (sectionType(ipre)==PREPROCESSED)
        {
          bool original_found=false;
          for (int jj=0;jj<nb_name_associated_id;jj++)
          {
            int iorg=name_associated_id[jj];
            if ( (sectionType(iorg)==ORIGINAL)
                &&
                (section_tab[ipre].module_id==section_tab[iorg].module_id) )
            {
              original_found=true;
              original_source_id=iorg;
              break;
            }
          }
          if (!original_found)
          {
            if (original_source_id<0)
            { // find link
              for (int jj=0;jj<nb_name_associated_id;jj++)
              {
                int iorg=name_associated_id[jj];
                if (sectionType(iorg)==ORIGINAL)
                {
                  original_source_id=iorg;
                  break;
                }
              }
            }

            if (original_source_id>=0)
            { // insert link
              section_tab[nb_section_tab-1].name_id=section_tab[ipre].name_id;
              section_tab[nb_section_tab-1].module_id=section_tab[ipre].module_id;
              assoc_insert_associated_id(name_assoc,section_tab[nb_section_tab-1].name_id,nb_section_tab-1);
              assoc_insert_associated_id(module_assoc,section_tab[nb_section_tab-1].module_id,nb_section_tab-1);
              section_tab[nb_section_tab-1].info=STRDUP(section_tab[ipre].info);
              section_tab[nb_section_tab-1].type=ORIGINAL;
              section_tab[nb_section_tab-1].signature=section_tab[ipre].signature;
              section_tab[nb_section_tab-1].position=section_tab[original_source_id].position;
              section_tab[nb_section_tab-1].size=section_tab[original_source_id].size;
              section_tab[nb_section_tab-1].checksum=section_tab[original_source_id].checksum;
              section_tab[nb_section_tab-1].flags=section_tab[ipre].flags;
            }
            else
            { // read source file
              FILE *original_file;
              const char*name;
              const char*name_rel;
              assoc_get(name_assoc,section_tab[ipre].name_id,name,name_rel);

              if ( (original_file=fopen(name,"r"))!=NULL)
              {
                char str[CHAINE_LEN];
                const char*module;
                const char*module_rel;

                assoc_get(module_assoc,section_tab[ipre].module_id,module,module_rel);
                openSection(module,module_rel,name,name_rel,"",CSMESFile::ORIGINAL,section_tab[ipre].signature,CSMESFile::NEW_RW,0);
                int nb_read=0;
                while ( (nb_read=fread(str,1,CHAINE_LEN,original_file)) > 0 )
                {
                  writeCharTab(str,nb_read);
                }
                closeSection();
                fclose(original_file);
              }
              else
                continue;
            }
          }
        }
      }
    }
    else
      break;
    name_id++;
  }
}

