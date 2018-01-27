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

#ifndef LIBTOKI_TOKENIZER_H
#define LIBTOKI_TOKENIZER_H

#include <libtoki/tokensource.h>
#include <libtoki/util/settings.h>
#include <libtoki/unicode/sink.h>

#include <unicode/unistr.h>

#include <boost/shared_ptr.hpp>

#include <iosfwd>
#include <vector>

namespace Toki {

	/**
	 * A tokenizer represents a class that grabs unicode characters from a
	 * UnicodeSource and returns Tokens.
	 */
	class Tokenizer : public TokenSource, public UnicodeSink
	{
	public:
		/**
		 * Construct an empty Tokenizer. The input source will be a null object
		 * that will always claim there are no more characters. Consequently,
		 * get_next_token() will just return null until a new source is set.
		 */
		Tokenizer(const Config::Node& cfg = default_config());

		/**
		 * Constructor shorthand -- start with the given UnicodeSource, and
		 * take ownership (through a shared pointer).
		 * @see UnicodeSink::set_input_source
		 */
		Tokenizer(UnicodeSource* input,
				const Config::Node& cfg = default_config());

		/**
		 * Constructor shorthand -- start with the given UnicodeSource as a
		 * shared pointer
		 * @see UnicodeSink::set_input_source
		 */
		Tokenizer(const boost::shared_ptr<UnicodeSource>& input,
				const Config::Node& cfg = default_config());

		/**
		 * Constructor shorthand -- start with the given std::istream. Note no
		 * ownership is taken and the stream must live long enough.
		 * @see UnicodeSink::set_input_source
		 */
		explicit Tokenizer(std::istream& is,
				const Config::Node& cfg = default_config());

		/**
		 * Constructor shorthand -- start with the given UnicodeString, which is
		 * copied.
		 * @see UnicodeSink::set_input_source
		 */
		explicit Tokenizer(const UnicodeString& s,
				const Config::Node& cfg = default_config());

		/**
		 * Destructor
		 */
		~Tokenizer();

		/**
		 * Reset the tokenizer internal state (other than the input source).
		 * Call this when after setting a new input source if the processing
		 * should start from scratch.
		 *
		 * Derived classes should take care to always call the parent class'
		 * restart()
		 */
		virtual void restart();

	protected:
		/**
		 * Apply the configuration -- helper to avoid repeating code in ctors
		 */
		void apply_configuration(const Config::Node& cfg);
	};

} /* end ns Toki */

#endif // LIBTOKI_TOKENIZER_H
