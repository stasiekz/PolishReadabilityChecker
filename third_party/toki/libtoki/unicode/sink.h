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

#ifndef LIBTOKI_UNICODESINK_H
#define LIBTOKI_UNICODESINK_H

#include <libtoki/unicode/source.h>

#include <unicode/unistr.h>

#include <boost/shared_ptr.hpp>

namespace Toki {

/**
 * This class is a base class for classes that can take a UnicodeSource
 * for processing.
 */
class UnicodeSink
{
public:
	/**
	 * Construct a sink with no input source (a null object that
	 * will always claim there are no more characters)
	 */
	explicit UnicodeSink();

	/**
	 * Constructor shorthand -- start with the given UnicodeSource, and
	 * take ownership (through a shared pointer).
	 * @see UnicodeSink::set_input_source
	 */
	explicit UnicodeSink(UnicodeSource* input);

	/**
	 * Constructor shorthand -- start with the given UnicodeSource as a
	 * shared pointer
	 * @see UnicodeSink::set_input_source
	 */
	explicit UnicodeSink(const boost::shared_ptr<UnicodeSource>& input);

	/**
	 * Constructor shorthand -- start with the given std::istream. Note no
	 * ownership is taken and the stream must live long enough.
	 * @see UnicodeSink::set_input_source
	 */
	explicit UnicodeSink(std::istream& is, int bufsize = 1000);

	/**
	 * Constructor shorthand -- start with the given UnicodeString, which
	 * is copied.
	 * @see UnicodeSink::set_input_source
	 */
	UnicodeSink(const UnicodeString& s);

	/// Destructor
	virtual ~UnicodeSink();

	/**
	 * Explicitly set the input source to be a null object that never
	 * returns any unicode characters
	 */
	void set_null_input_source();

	/**
	 * Input source setter. The tokenizer takes ownership of the source
	 * (through a shared pointer)and will delete it.
	 * Note, however, that if the source is a UnicodeIstreamWrapper, the
	 * underlying istream object will *not* be deleted -- see
	 * UnicodeIstreamWrapper, which also requires that the istream lives
	 * for as long as the UnicodeIstreamWrapper lives.
	 * @param us a valid UnicodeSource to use and take ownership of.
	 *           Behavior is undefined if the pointer is invalid or NULL.
	 */
	void set_input_source(UnicodeSource* us);

	/**
	 * Input source setter -- as a shared pointer. See the are pointer
	 * version for info on what happens on the atcual destruction.
	 */
	void set_input_source(const boost::shared_ptr<UnicodeSource>& us);

	/**
	 * Input source setter. The stream must live for as long as the sink
	 * lives.
	 */
	void set_input_source(std::istream& is, int bufsize = 1000);

	/**
	 * Input source setter. The string is copied.
	 */
	void set_input_source(const UnicodeString& s);

	/**
	 * Accessor for the used input source
	 */
	boost::shared_ptr<UnicodeSource> get_input_source() {
		return input_;
	}
protected:
	/**
	 * Shorthand to avoid derived classes having to explicitly use the pointer
	 */
	UnicodeSource& input() {
		return *input_;
	}

	/**
	 * Called when a new input source is created, derived classes might
	 * want to override this.
	 *
	 * WARNING: this will *NOT* be called during the initial construction
	 * of a UnicodeSink, or rather, the overriden version will not be
	 * called, so derived classess should manually call their handler in
	 * constructors that cause an actual input source to be set.
	 */
	virtual void new_input_source() {}

	//private:
	/**
	 * The source of the text.
	 */
	boost::shared_ptr<UnicodeSource> input_;

};

} /* end ns Toki */

#endif // LIBTOKI_UNICODESINK_H
