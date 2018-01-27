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

#ifndef LIBSORPUS2_IO_CCLWRITER_H
#define LIBCORPUS2_IO_CCLWRITER_H

#define CHUNKS_PER_SAVE 3000

#include <libcorpus2/io/xmlwriter.h>

namespace Corpus2 {

class CclWriter : public XmlWriter
{
public:
	CclWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params);
    CclWriter(std::ostream& os, const Tagset& tagset,
            const string_range_vector& params, bool disable_compression);

	~CclWriter();

	void write_sentence(const Sentence &s);

	void write_chunk(const Chunk &c);

	static bool registered;

protected:
    void finish();

	void write_sentence_int(const Sentence &s);

	void do_header();

	void do_footer();

	void paragraph_head();

	void paragraph_head(const Chunk& c);
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_CCLWRITER_H
