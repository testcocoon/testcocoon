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

#include <vector>
#include <wchar.h>
#include <string.h>
#include "utf.h"
#include "debug.h"


std::wstring utf8_to_utf16(const std::string &in)
{
  std::wstring out;
  int len=in.length();
  int i = 0;
  unsigned long temp;
  if (len>=3 
      && in[0]==char(0xEF)
      && in[1]==char(0xBB)
      && in[2]==char(0xBF))
  {
    out.push_back(0xFEFF);
    i+=3;
  }

  while (i < len) 
  {
    if ((in[i] & 0x80) == 0) 
    { // ASCII : 0000 0000-0000 007F 0xxxxxxx
      temp = in[i];
      ++i;
    } 
    else if ( ((in[i] & 0xE0) == 0xC0) && (i+1 < len) )
    { // 0000 0080-0000 07FF 110xxxxx 10xxxxxx
      temp = (in[i] & 0x1F);
      temp <<= 6;
      temp += (in[i + 1] & 0x3F);
      i += 2;
    } 
    else if ( ((in[i] & 0xF0) == 0xE0) && (i+2 < len) )
    { // 0000 0800-0000 FFFF 1110xxxx 10xxxxxx 10xxxxxx
      temp = (in[i] & 0x0F);
      temp <<= 6;
      temp += (in[i + 1] & 0x3F);
      temp <<= 6;
      temp += (in[i + 2] & 0x3F);
      i += 3;
    }
    else if ( ((in[i] & 0xF8) == 0xF0) && (i+3 < len) )
    { // 0001 0000-001F FFFF 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
      temp = (in[i] & 0x07);
      temp <<= 6;
      temp += (in[i + 1] & 0x3F);
      temp <<= 6;
      temp += (in[i + 2] & 0x3F);
      temp <<= 6;
      temp += (in[i + 3] & 0x3F);
      i += 4;
    } 
    else if ( ((in[i] & 0xFC) == 0xF8) && (i+4 < len) )
    { // 0020 0000-03FF FFFF 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
      temp = (in[i] & 0x03);
      temp <<= 6;
      temp += (in[i + 1] & 0x3F);
      temp <<= 6;
      temp += (in[i + 2] & 0x3F);
      temp <<= 6;
      temp += (in[i + 3] & 0x3F);
      temp <<= 6;
      temp += (in[i + 4] & 0x3F);
      i += 5;
    }
    else if ( ((in[i] & 0xFE) == 0xFC) && (i+5<len) )
    { // 0400 0000-7FFF FFFF 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
      temp = (in[i] & 0x01);
      temp <<= 6;
      temp += (in[i + 1] & 0x3F);
      temp <<= 6;
      temp += (in[i + 2] & 0x3F);
      temp <<= 6;
      temp += (in[i + 3] & 0x3F);
      temp <<= 6;
      temp += (in[i + 4] & 0x3F);
      temp <<= 6;
      temp += (in[i + 5] & 0x3F);
      i += 6;
    } 
    else
    {
      temp = '?';
      ++i;
    }
    // Need to transform to UTF-16 > handle surrogates
    if (temp > 0xFFFF) 
    {
      // First high surrogate
      out.push_back(0xD800 + wchar_t(temp >> 10));
      // Now low surrogate
      out.push_back(0xDC00 + wchar_t(temp & 0x3FF));
    }
    else
    {
      out.push_back(wchar_t(temp));
    }
  }
  return out;
}

std::string utf16_to_utf8(const std::wstring &in)
{
  std::string out;
  int len = in.length();
  int i = 0;
  unsigned long temp;
  if (len==0)
    return out;

  if (in[0]==0xFEFF)
  {
    out.push_back(char(0xEF));
    out.push_back(char(0xBB));
    out.push_back(char(0xBF));
    i++;
  }

  while (i < len) 
  {
    if ( ((in[i] & 0xD800) == 0xD800))
    {
      if (i+1<len)
      { // surrogate
        temp = (in[i] - 0xD800);
        temp <<= 10;

        temp += (in[i + 1] - 0xDC00);
        i += 2;
      }
      else
        return out; // too short, ignore last char
    }
    else 
    {
      temp = in[i];
      ++i;
    }
    if (temp < 0x00000080) 
    { // ASCII : 0000 0000-0000 007F 0xxxxxxx
      out.push_back(char(temp));
    } 
    else if (temp < 0x00000800)
    { // 0000 0080-0000 07FF 110xxxxx 10xxxxxx
      out.push_back(char(0xC0 | (temp >> 6)));
      out.push_back(char(0x80 | (temp & 0x3F)));
    }
    else if (temp < 0x00010000) 
    { // 0000 0800-0000 FFFF 1110xxxx 10xxxxxx 10xxxxxx
      out.push_back(char(0xE0 | (temp >> 12)));
      out.push_back(char(0x80 | ((temp >> 6) & 0x3F)));
      out.push_back(char(0x80 | (temp & 0x3F)));
    }
    else if (temp < 0x00200000)
    { // 0001 0000-001F FFFF 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
      out.push_back(char(0xF0 | (temp >> 18)));
      out.push_back(char(0x80 | ((temp >> 12) & 0x3F)));
      out.push_back(char(0x80 | ((temp >> 6) & 0x3F)));
      out.push_back(char(0x80 | (temp & 0x3F)));
    } 
    else if (temp < 0x04000000)
    { // 0020 0000-03FF FFFF 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
      out.push_back(char(0xF8 | (temp >> 24)));
      out.push_back(char(0x80 | ((temp >> 18) & 0x3F)));
      out.push_back(char(0x80 | ((temp >> 12) & 0x3F)));
      out.push_back(char(0x80 | ((temp >> 6) & 0x3F)));
      out.push_back(char(0x80 | (temp & 0x3F)));
    }
    else if (temp < 0x80000000)
    { // 0400 0000-7FFF FFFF 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
      out.push_back(char(0xFC | (temp >> 30)));
      out.push_back(char(0x80 | ((temp >> 24) & 0x3F)));
      out.push_back(char(0x80 | ((temp >> 18) & 0x3F)));
      out.push_back(char(0x80 | ((temp >> 12) & 0x3F)));
      out.push_back(char(0x80 | ((temp >> 6) & 0x3F)));
      out.push_back(char(0x80 | (temp & 0x3F)));
    }
  }
  return out;
}

std::string utf16_to_ascii(const std::wstring &in)
{
  std::string out;
  int lg=in.length();
  char *text=(char*)MALLOC(sizeof(char)*(lg+1));
  int offset;
  for (offset=0;offset<lg;offset++)
    if (in[offset]!=0xFEFF)
      break;
  wcstombs(text,in.c_str()+offset,in.length()-offset);
  text[in.length()-offset]='\0';
  out=std::string(text);
  FREE(text);
  return out;
}

text_encoding_t text_encoding(const std::string &in)
{
  if (in.length()>=2 
      && in[0]==char(0xFF)
      && in[1]==char(0xFE))
    return ENC_UTF16;
  else if (in.length()>=3 
      && in[0]==char(0xEF)
      && in[1]==char(0xBB)
      && in[2]==char(0xBF))
    return ENC_UTF8;
  else
    return ENC_ASCII;
}

std::string utf8_remove_byte_mark(const std::string &in)
{
  if (in.length()>=3 
      && in[0]==char(0xEF)
      && in[1]==char(0xBB)
      && in[2]==char(0xBF))
  {
    return in.substr(3);
  }
  else
    return in;
}

std::wstring utf16_remove_byte_mark(const std::wstring &in)
{
  if (in.length()>=1 
      && in[0]==0xFEFF)
  {
    return in.substr(1);
  }
  else
    return in;
}

std::string ascii_correct_eol(const std::string &in)
{
  std::string out;
  for (std::string::const_iterator it=in.begin();it!=in.end();++it)
  {
    if (*it!='\r')
      out+=*it;
  }
  return out;
}

