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

#ifndef LIBTOKI_ICUSTREAMWRAPPER_H
#define LIBTOKI_ICUSTREAMWRAPPER_H

#include <libtoki/unicode/source.h>

#include <unicode/unistr.h>
#include <unicode/ucnv.h>

#include <istream>

namespace Toki {

/**
 * This class is a ICU-Unicode wrapper for a std::istream that allows
 * getting Unicode characters from the stream in a reliable and efficient
 * way.
 *
 * Note that this class will read ahead from the stream and maintain a
 * buffer of characters to return.
 */
class UnicodeIstreamWrapper : public UnicodeSource
{
public:
	/**
	 * The constructor.
	 * @param is             The istream to wrap
	 * @param input_encoding String representation of the encoding to
	 *                       expect in the input stream. ICU format, so
	 *                       case, dashes, whitespace etc. don't matter.
	 * @param buf_size       Internal buffer size. Small values might be
	 *                       terribly inefficient, but everything will work
	 *                       even with a 1 byte buffer.
	 */
	UnicodeIstreamWrapper(std::istream& is, int buf_size = 200,
					 const char* input_encoding = "UTF8");

	/// The destructor.
	~UnicodeIstreamWrapper();

	/// Override from UnicodeSource
	UChar peek_next_char();

	/// Override from UnicodeSource
	UChar get_next_char();

	/**
	 * Get the UnicodeString constructed from the current buffer contents.
	 * The buffer is marked as used up so the next call to get_next_char or
	 * similar will have to read from the input stream.
	 *
	 * May return an empty string if there are no characters in the input
	 * stream remaining.
	 */
	UnicodeString get_buffer();

	/// Override from UnicodeSource
	bool has_more_chars();

private:
	/**
	 * The main conversion function.
	 *
	 * Reads a batch of characters from the source stream, converts it and
	 * stores in the target buffer so characters can be returned by
	 * get_next_char or similar.
	 *
	 * @return the number of characters read from the input stream
	 */
	int more();

	/**
	 * For very small buffer sizes (<4) it is possible that a single more()
	 * call will not be enough to end up with at least one character in the
	 * target buffer, confusing other code. This function takes care to
	 * call more() repeatedly until at least one character ends up in the
	 * target buffer, or the input stream is exhausted.
	 */
	void ensure_more();

	/// The source input stream
	std::istream& is_;

	/// The ICU converter handle
	UConverter* converter_;

	/// Conversion buffer size
	int buf_size_;

	/// "Source" buffer -- characters read from the input stream
	char* source_buf_;

	/// "Target" buffer -- converted Unicode characters
	UChar* target_buf_;

	/// State variable -- pointer into the target buffer, pointing one item
	/// beyond the last converted character
	UChar* target_;

	/// State variable -- pointer into the taret buffer pointing to the
	/// next Unicode character to be returned by get_next_char and similar
	UChar* out_;
};

} /* end ns Toki */

#endif // LIBTOKI_ICUSTREAMWRAPPER_H
