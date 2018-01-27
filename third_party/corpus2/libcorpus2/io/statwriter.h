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

#ifndef LIBCORPUS2_IO_STATWRITER_H
#define LIBCORPUS2_IO_STATWRITER_H

#include <libcorpus2/io/writer.h>

namespace Corpus2 {

class StatWriter : public TokenWriter
{
public:
	StatWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params);

	void write_token(const Token& t);

	void write_sentence(const Sentence& t);

	void write_chunk(const Chunk& c);

	static bool registered;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_STATWRITER_H
