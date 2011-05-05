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

#ifndef _UTF_H___
#define _UTF_H___

#include <string>
enum text_encoding_t
{
  ENC_ASCII,
  ENC_UTF8,
  ENC_UTF16
};

std::string utf8_remove_byte_mark(const std::string &in);
std::wstring utf16_remove_byte_mark(const std::wstring &in);

std::wstring utf8_to_utf16(const std::string &in);
std::string utf16_to_utf8(const std::wstring &in);
std::string utf16_to_ascii(const std::wstring &in);
std::string ascii_correct_eol(const std::string &in);
text_encoding_t text_encoding(const std::string &in);

#endif
