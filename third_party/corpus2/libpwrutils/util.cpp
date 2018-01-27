/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENCE, COPYING.LESSER and COPYING files for more details.
*/

#include <libpwrutils/util.h>

namespace PwrNlp {

std::string unescape_utf8(const std::string& str)
{
	std::string out;
	UnicodeString u = UnicodeString::fromUTF8(str).unescape();
	u.toUTF8String(out);
	return out;
}

std::string to_utf8(const UnicodeString &ustr)
{
	std::string s;
	ustr.toUTF8String(s);
	return s;
	}

} /* end namespace PwrNlp */
