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

#ifndef ARGV_H_
#define ARGV_H_
#include "coveragescannerpdef.h"
#include <string>
#include <vector>

class Argv
{
  public:


    Argv();
    ~Argv();
    Argv(const Argv &v) ;
    Argv(int c,const char * const * v);
    int argc() const { return _argc; }
    const char *const* argv() const { return _argv; }
    const char *operator[] (int i) const { return argv()[i]; }
    bool set(int index,const char*value); 
    void append(const char *val);
    void suppress_args(int min_index,int max_index);
    void suppress_arg(int index) { suppress_args(index,index); }
    void append(int argc_to_append,const char *const* argv_to_append);
    void append(const Argv &v) { append(v.argc(),v.argv()); }
    void clear();
    Argv& operator=(const Argv&);

  private:
    int _argc;
    char **_argv;
    static inline void suppress_args(int &argc,char **&argv,int min_index,int max_index);
    static inline void append(int &argc,char **&argv,const char *val);
    static inline void append(int &argc,char **&argv,int argc_to_append,const char *const* argv_to_append);
    static inline void duplicate(int &argc_dest,char **&argv_dest,int argc_src,const char * const * argv_src);
    static inline void free(char **&argv);
};

#endif
