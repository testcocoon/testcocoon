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

#include "tools.h"
#ifdef OS_WIN32
#include <windows.h>
#include <winreg.h>
#include <winerror.h>
#define HKCU_STRING "HKEY_CURRENT_USER"
#define HKCR_STRING "HKEY_CLASSES_ROOT"
#define HKLM_STRING "HKEY_LOCAL_MACHINE"
#define HKU_STRING "HKEY_USERS"

QString Tools::readRegistry( QString keyname, QString valuename)
{
  DWORD maxValueNameLen;
  DWORD maxSubkeyLen;
  HKEY rootkey;
  HKEY openkey;
  HKEY hSubkey;
  LPBYTE value=NULL;
  DWORD valuetype;
  DWORD datacount;

  int id = keyname.indexOf("\\" );
  if ( id == -1 ) 
  {
    return QString::null;
  }
  QString subkey=keyname.mid(id+1);
  printf("%s",subkey.toAscii());

  QString rootkeystr=keyname.left(id);
  if ( rootkeystr == HKLM_STRING  )
    rootkey = HKEY_LOCAL_MACHINE;
  else if ( rootkeystr ==  HKCR_STRING  )
    rootkey = HKEY_CLASSES_ROOT;
  else if ( rootkeystr ==  HKCU_STRING )
    rootkey = HKEY_CURRENT_USER;
  else if ( rootkeystr == HKU_STRING )
    rootkey = HKEY_USERS;
  else
  {
    return QString::null;
  }
  long RegError = RegOpenKeyExA( rootkey, NULL, 0, KEY_READ, ( PHKEY ) &openkey );
  if ( RegError != 0 )
  {
    LPWSTR lpBuffer= (LPWSTR)malloc(65000);
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,RegError,LANG_SYSTEM_DEFAULT,lpBuffer,65000,NULL);
    free(lpBuffer);
    return QString::null;
  }

  RegError = RegOpenKeyExA( openkey,  subkey.toAscii() , 0, KEY_READ, ( PHKEY ) &hSubkey );
  if ( RegError != 0 ) 
  {
    LPWSTR lpBuffer= (LPWSTR)malloc(65000);
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,RegError,LANG_SYSTEM_DEFAULT,lpBuffer,65000,NULL);
    free(lpBuffer);
    RegCloseKey( openkey );
    return QString::null;
  }

  RegQueryInfoKeyA( hSubkey, NULL, NULL, NULL, NULL, &maxSubkeyLen, NULL, NULL, &maxValueNameLen, NULL, NULL, NULL);
  maxSubkeyLen++;
  maxValueNameLen++;

  RegError = RegQueryValueExA( hSubkey, valuename.toAscii(), NULL, &valuetype, NULL, ( LPDWORD ) &datacount );
  if ( RegError != 0 ) 
  {
    LPWSTR lpBuffer= (LPWSTR)malloc(65000);
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,RegError,LANG_SYSTEM_DEFAULT,lpBuffer,65000,NULL);
    free(lpBuffer);
    RegCloseKey( hSubkey );
    RegCloseKey( openkey );
    return QString::null;
  }

  value = ( LPBYTE ) malloc( datacount + 1 );
  RegError = RegQueryValueExA( hSubkey,  valuename.toAscii(), NULL,  &valuetype, value, ( LPDWORD ) &datacount );

  if ( RegError != 0 ) 
  {
    LPWSTR lpBuffer= (LPWSTR)malloc(65000);
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,RegError,LANG_SYSTEM_DEFAULT,lpBuffer,65000,NULL);
    free(lpBuffer);
    RegCloseKey( hSubkey );
    RegCloseKey( openkey );
    return QString::null;
  }

  QString retval;
  switch( valuetype ) 
  { 
    case REG_SZ:
    case REG_EXPAND_SZ: 
      retval=(char*)value;
      break;
    case REG_DWORD: 
      retval=QString::number(*( (DWORD*)value));
      break;
    case REG_BINARY: 
      for ( DWORD i = 0; i < datacount; i++ )
      {
        retval+=QString::number(( ( unsigned char * ) value )[i] , 16 ) + " ";
      }
      break;
  }
  free(value);

  RegCloseKey( hSubkey );
  RegCloseKey( openkey );

  return retval;
}

void Tools::writeRegistry( QString keyname, QString valuename, QString valueexpr)
{
  HKEY rootkey;
  HKEY openkey;
  HKEY hSubkey;

  int id = keyname.indexOf("\\" );
  if ( id == -1 ) 
  {
    return ;
  }
  QString subkey=keyname.mid(id+1);
  printf("%s",subkey.toAscii());

  QString rootkeystr=keyname.left(id);
  if ( rootkeystr == HKLM_STRING  )
    rootkey = HKEY_LOCAL_MACHINE;
  else if ( rootkeystr ==  HKCR_STRING  )
    rootkey = HKEY_CLASSES_ROOT;
  else if ( rootkeystr ==  HKCU_STRING )
    rootkey = HKEY_CURRENT_USER;
  else if ( rootkeystr == HKU_STRING )
    rootkey = HKEY_USERS;
  else
  {
    return ;
  }
  long RegError = RegOpenKeyExA( rootkey, NULL, 0, KEY_READ, ( PHKEY ) &openkey );
  if ( RegError != 0 )
  {
    LPWSTR lpBuffer= (LPWSTR)malloc(65000);
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,RegError,LANG_SYSTEM_DEFAULT,lpBuffer,65000,NULL);
    free(lpBuffer);
    return ;
  }

  RegError = RegOpenKeyExA( openkey,  subkey.toAscii() , 0, KEY_READ, ( PHKEY ) &hSubkey );
  if ( RegError != 0 ) 
  {
    LPWSTR lpBuffer= (LPWSTR)malloc(65000);
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,RegError,LANG_SYSTEM_DEFAULT,lpBuffer,65000,NULL);
    free(lpBuffer);
    RegCloseKey( openkey );
    return ;
  }

  RegError = RegSetValueExA( hSubkey, valuename.toAscii(), NULL, REG_SZ,(const BYTE*)valueexpr.toAscii().constData(), valueexpr.length()+1  );
  if ( RegError != 0 ) 
  {
    LPWSTR lpBuffer= (LPWSTR)malloc(65000);
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,RegError,LANG_SYSTEM_DEFAULT,lpBuffer,65000,NULL);
    free(lpBuffer);
    RegCloseKey( hSubkey );
    RegCloseKey( openkey );
    return ;
  }


  RegCloseKey( hSubkey );
  RegCloseKey( openkey );
}
#endif
