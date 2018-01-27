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

#ifndef LIBSORPUS2_IO_XMLWRITER_H
#define LIBCORPUS2_IO_XMLWRITER_H

#include <libcorpus2/io/writer.h>

namespace Corpus2 {

/**
 * Base class for xml-ish writers
 */
class XmlWriter : public TokenWriter {
public:
	XmlWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params);

	~XmlWriter();

	void write_token(const Token &t);

	void write_sentence(const Sentence &s);

protected:
    void do_header();

    void do_footer();

	virtual void paragraph_head();

	void paragraph_head(const Chunk& c);

	int cid_;

	bool use_indent_;

	bool output_disamb_;

	bool sort_tags_;

	bool whitespace_info_;
};


} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_XMLWRITER_H
