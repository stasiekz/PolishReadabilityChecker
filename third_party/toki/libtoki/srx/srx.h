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

#ifndef LIBTOKI_SRX_SRX_H
#define LIBTOKI_SRX_SRX_H

#include <libtoki/srx/document.h>
#include <libtoki/srx/segmenter.h>
#include <libtoki/unicode/source.h>

#include <boost/smart_ptr.hpp>
#include <boost/utility.hpp>

namespace Toki {
namespace Srx {

/**
 * A SRX parser wrapping a UnicodeSource.
 *
 * Characters from the wrapped source are put in a buffer, treated as
 * a "window" with margins one each side (so the total size of the buffer
 * is window + margin * 2. SRX sentence breaks that happen to lay inside
 * the window are computed and later available through the UnicodeSource
 * overriden peek_begins_sentence().
 *
 * The buffer layout is: left_margin window right_margin
 *
 * Initially, the left buffer is empty.
 *
 * Note that this class does not understand SRX, it just takes a SRX
 * segmenter and runs it repeatedly on a buffer. Characters are always
 * returned from the window area of the buffer, advancing the internal
 * pointer. When the pointer starts pointing to the beginning of the right
 * margin, the buffer is moved so that the last margin_width characters of
 * the window form the new left margin, and new characters are streamed
 * from the wrapped source. SRX segment breaks are then computed again and
 * the internal pointer is reset to point at the beginning of the window.
 *
 * Larger windows should yield more performance, and large margin sizes
 * will result in preformance drop. However, the margin size must be large
 * enough to contain the longest regex match expected, otherwise results
 * may be off. Technically the restriction is slightly more loose, but the
 * aforementioned rule of thumb is reasonable anyway.
 */
class SourceWrapper : public UnicodeSource, private boost::noncopyable {
public:
	/**
	 * Constructor for the SRX UnicodeSource wrapper.
	 * Takes ownerhip of the wrapped source and the segmenter
	 * (through a shared pointer)
	 */
	SourceWrapper(UnicodeSource* src, Segmenter* segm,
			int window = 200, int margin = 100);

	SourceWrapper(const boost::shared_ptr<UnicodeSource>& src,
			const boost::shared_ptr<Segmenter>& segm,
			int window = 200, int margin = 100);

	~SourceWrapper();

	/// Override from UnicodeSource
	UChar get_next_char();

	/// Override from UnicodeSource
	UChar peek_next_char();

	/// Override from UnicodeSource
	bool has_more_chars();

	/// Override from UnicodeSource
	bool peek_begins_sentence();

	/**
	 * Setter for the SRX segmenter object -- raw pointer version.
	 * Takes ownership of the segmenter.
	 */
	void set_segmenter(Segmenter* s);

	/**
	 * Setter for the SRX segmenter object -- shared pointer version.
	 */
	void set_segmenter(const boost::shared_ptr<Segmenter>& s);

	/**
	 * Setter for the wrapped source object -- raw pointer version.
	 * Takes ownership of the source.
	 */
	void set_source(UnicodeSource* s);

	/**
	 * Setter for the wrapped source object -- shared pointer version.
	 */
	void set_source(const boost::shared_ptr<UnicodeSource>& s);

	/**
	 * Getter for the used segmenter
	 */
	boost::shared_ptr<Segmenter> get_segmenter() const {
		return segmenter_;
	}

private:
	/// helper initialisation function
	void init();

	/// check function for the output index position
	bool buffer_ok() {
		return (out_idx_ < buffer_end_idx_) &&
				(out_idx_ < window_size_ + margin_size_);
	}

	/// buffer initialisation
	void init_buffer();

	/// helper function used wherever it is possible thatthe buffer will
	/// not be valid, makes the buffer valid if possible. If the buffer is
	/// still invalid ater a call to ensure_more, there are no more
	/// characters
	void ensure_more();

	/// move the buffer area so that new data can be processed
	void move_buffer();

	/// calculate breaks in the window area of the buffer
	void calculate_breaks();

	/// the wrapper source
	boost::shared_ptr<UnicodeSource> source_;

	/// the segmenter
	boost::shared_ptr<Segmenter> segmenter_;

	/// window size
	int window_size_;

	/// margin size
	int margin_size_;

	/// buffer size, equal to window_size_ + 2 * margin_size
	int buffer_size_;

	/// the buffer
	UChar* buffer_;

	/// the output index
	int out_idx_;

	/// index of the start of actual data in the buffer
	/// apart from the very beginning, when there is no left margin data,
	/// this will be always zero
	int buffer_start_idx_;

	/// index beyond the end of actual data in the buffer
	int buffer_end_idx_;

	/// the computed breaks in the window area
	std::vector<bool> breaks_;
};

} /* end ns Srx */ } /* end ns Toki */

#endif // LIBTOKI_SRX_SRX_H
