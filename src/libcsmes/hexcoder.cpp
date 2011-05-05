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

#include "libcsmespdef.h"
#include "hexcoder.h"
#include <stdlib.h>
#include <string.h>

static int hxtoint(char c)
{
    if ( c>='A' && c<='F' )
        return c-'A'+10;
    if ( c>='0' && c<='9' )
        return c-'0';
    return -1;
}

static char inttohx(int h)
{
  if (h<=9)
    return '0'+h;
  else
    return 'A'+h-10;
}

int hexencode(const char *source,int source_lg,char **destination)
{
    int i;
    *destination=(char*)(MALLOC(source_lg*2+1));
    for (i=0;i<source_lg;i++)
    {
        char lsb,msb;
        lsb=source[i] & 0xF;
        msb=( source[i] & 0xF0 ) >> 4;
        (*destination)[i*2]=inttohx(msb);
        (*destination)[i*2+1]=inttohx(lsb);
    }
    (*destination)[2*source_lg]='\0';
    return TRUE;

}

int hexdecode(const char *source,char **destination,int *destination_lg)
{
    int i;
    int lg;

    lg=strlen(source);
    if (lg%2==1)
        return FALSE;
    *destination_lg=lg/2;
    *destination=(char*) MALLOC(*destination_lg);
    for(i=0;i<*destination_lg;i++)
    {
        int msb,lsb;
        msb=hxtoint(source[2*i]);
        lsb=hxtoint(source[2*i+1]);
        if ( (msb==-1) || (lsb==-1) )
        {
            FREE(*destination);
            return FALSE;
        }
        (*destination)[i]=(msb<<4)|(lsb);
    }
    return TRUE;
}
