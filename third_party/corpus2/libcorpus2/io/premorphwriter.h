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

#ifndef LIBCORPUS2_IO_PREMORPHWRITER_H
#define LIBCORPUS2_IO_PREMORPHWRITER_H

#include <libcorpus2/io/writer.h>

namespace Corpus2 {

class PremorphWriter : public TokenWriter {
public:
	PremorphWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params);

	~PremorphWriter();

	void write_token(const Token &t);

	void write_sentence(const Sentence &s);

	void write_chunk(const Chunk &c);

protected:
	void do_header();

	void do_footer();

	void paragraph_head();

	void paragraph_head(const Chunk& c);

	int cid_;

	bool force_chunk_;

	static bool registered;

};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_PREMORPHWRITER_H
