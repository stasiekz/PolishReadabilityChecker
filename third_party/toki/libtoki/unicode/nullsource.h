/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBTOKI_NULLUNICODESOURCE_H
#define LIBTOKI_NULLUNICODESOURCE_H

#include <libtoki/exception.h>
#include <libtoki/unicode/source.h>

namespace Toki {

/**
 * A null UnicodeSource that never returns anything and always claims there
 * are no more characters. Will throw when peek/get next char is requested.
 */
class NullUnicodeSource : public UnicodeSource
{
public:
	/// The constructor
	NullUnicodeSource() {}

	/// UnicodeSource override
	UChar peek_next_char() {
		throw TokenizerImpossibleError(
				"NullUnicodeSource::peek_next_char called");
		return 0;
	}

	/// UnicodeSource override
	UChar get_next_char() {
		throw TokenizerImpossibleError(
				"NullUnicodeSource::get_next_char called");
		return 0;
	}

	/// UnicodeSource override
	bool has_more_chars() {
		return false;
	}
};

} /* end ns Toki */

#endif // LIBTOKI_NULLUNICODESOURCE_H
