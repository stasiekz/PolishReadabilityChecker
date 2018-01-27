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

#ifndef LIBTOKI_ICUSTRINGSOURCE_H
#define LIBTOKI_ICUSTRINGSOURCE_H

#include <libtoki/unicode/source.h>

#include <unicode/schriter.h>
#include <unicode/unistr.h>

namespace Toki {

/**
 * A thin wrapper around UnicodeString (using aStringCharacterIterator) to
 * adapt it to the UnicodeSource interface.
 */
class UnicodeIcuStringWrapper : public UnicodeSource
{
public:
	/**
	 * The constructor.
	 *
	 * @param u The Unicode string to wrap. The string is copied.
	 */
	UnicodeIcuStringWrapper(const UnicodeString& u);

	/// The destructor
	~UnicodeIcuStringWrapper();

	/// Override from UnicodeSource
	UChar peek_next_char();

	/// Override from UnicodeSource
	UChar get_next_char();

	/// Override from UnicodeSource
	bool has_more_chars();

private:
	/// The internal StringCharacterIterator object
	StringCharacterIterator iter_;
};

} /* end ns Toki */


#endif // LIBTOKI_ICUSTRINGSOURCE_H
