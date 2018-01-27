/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.CORPUS2, LICENSE.POLIQARP, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBSORPUS2_IO_PATHWRITER_H
#define LIBCORPUS2_IO_PATHWRITER_H

#include <libcorpus2/io/writer.h>
#include <boost/scoped_ptr.hpp>

namespace Corpus2 {

/**
 * A wrapper class for the output stream Writers that holds an open stream
 * and closes it on destruction. All write calls are passed to the
 * wrapped Writer.
 */
class PathWriter : public TokenWriter
{
public:
	/**
	 * The constructor. It is assumed that the underlying writer operates
	 * on the same ostream as the pointer passed.
	 */
	PathWriter(const boost::shared_ptr<TokenWriter>& underlying,
			const boost::shared_ptr<std::ostream>& os);

	~PathWriter();

	void write_token(const Token& t);

	virtual void write_sentence(const Sentence& s);

	virtual void write_chunk(const Chunk& p);

	boost::shared_ptr<TokenWriter> get_underlying() {
		return underlying_;
	}

	boost::shared_ptr<std::ostream> get_ostream_pointer() {
		return os_;
	}

protected:
	/// The owned ostream pointer
	boost::shared_ptr<std::ostream> os_;
	/// The wrapped Writer (Note: must come after the ostream ptr in the class!)
	boost::shared_ptr<TokenWriter> underlying_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_PATHWRITER_H
