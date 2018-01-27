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

#ifndef LIBTOKI_UNICODESOURCE_H
#define LIBTOKI_UNICODESOURCE_H

#include <unicode/utypes.h>

namespace Toki {

/**
 * This class represents something that we can get ICU Unicode characters
 * from and is generally used to abstract away a bit of the functionality
 * we need from a ICU-Unicode wrapper for an input stream.
 *
 * It is similar to ICU's ForwardCharacterIterator, but we probably don't
 * want to inherit directly from ICU, and we want the extra "peek"
 * functionality.
 */
class UnicodeSource
{
public:

	/// Constructor
	UnicodeSource() {}

	/// Destructor
	virtual ~UnicodeSource() {}

	/**
	 * Peek the next character, that is, return it without advancing to the
	 * next character.
	 *
	 * This function might change the internal status to make sure there is
	 * something available for returning, so it cannot be const.
	 *
	 * @return the next character to be returned, or 0x0 if there is none.
	 *         Calling code should generally check has_more_chars() first.
	 */
	virtual UChar peek_next_char() = 0;

	/**
	 * Get the next character. Functionally similar to peek_next_char that
	 * advances internally to the next character.
	 *
	 * @return the next character to be returned, or 0x0 if there is none.
	 *         Calling code should generally check has_more_chars() first.
	 */
	virtual UChar get_next_char() = 0;

	/**
	 * Check if there will be more characters to return.
	 *
	 * This function might change the internal status to make sure there is
	 * something available for returning, so it cannot be const.
	 *
	 * @return true if there is something to return, false otherwise.
	 */
	virtual bool has_more_chars() = 0;

	/**
	 * Return the begin_sentence flag for the character available
	 * via peek_next_char. By default always returns false. Derived classes
	 * might use this to do some character-level processing to discover
	 * sentence breaks before tokenization.
	 */
	virtual bool peek_begins_sentence();
};

} /* end ns Toki */

#endif // LIBTOKI_UNICODESOURCE_H
