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

#include "wildcard.h"
#include "debug.h"

bool wildmatch(const char * pat, const char * str) 
{
  const char * s=str;
  const char *  p=pat;

  for (; *p != '\0'; ++s, ++p) 
  {
    switch (*p)
    {
      case '?':
        if (*s == '\0')
          return false;
        break;
      case '*':
        while ( (*p) == '*' )
          ++p;
        switch (*p)
        {
          case '\0':
            break;
          default:
            for (;;)
            {
              if ( *s == '\0' )
                return false;
              if (*p != '?')
              {
                while ( *s != *p )
                {
                  ++s;
                  if ( *s == '\0' )
                    return false;
                }
              }
              if (wildmatch(p,s))
                break;
              ++s;
            }
        }
        return true;

      default:
        if (*s != *p)
          return false;
        break;
    } 
  } 
  return *s == '\0';
}

