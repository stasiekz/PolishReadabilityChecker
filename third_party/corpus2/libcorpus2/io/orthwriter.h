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

#ifndef LIBCORPUS2_IO_ORTHWRITER_H
#define LIBCORPUS2_IO_ORTHWRITER_H

#include <libcorpus2/io/writer.h>

namespace Corpus2 {

class OrthWriter : public TokenWriter {
public:
	OrthWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params);

	~OrthWriter();

	void write_token(const Token &t);

	void write_sentence(const Sentence &s);

	void write_chunk(const Chunk &c);

	bool actual_ws_;

	bool end_nl_;

	static bool registered;

};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_ORTHWRITER_H
