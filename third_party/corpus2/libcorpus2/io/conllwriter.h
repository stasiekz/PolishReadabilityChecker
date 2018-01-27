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

#ifndef CONLLWRITER_H
#define CONLLWRITER_H

#include <libcorpus2/io/writer.h>

namespace Corpus2 {

/**
 * Writer in the CONLL format (as required by MALT parser). The writer
 * assumes that the tagset used employs an attribute named 'superpos'
 * (this naming is obligatory) and the attribute is defined as first
 * and required for each grammatical class. This attribute is used to
 * designate a more general POS category for each token (e.g. all verb
 * classes could be marked as VERB there).
 */
class ConllWriter : public TokenWriter
{
public:
	ConllWriter(std::ostream& os, const Tagset& tagset,
				const string_range_vector& params);
	~ConllWriter();

	void write_token(const Token &t);
	void write_sentence(const Sentence &s);
	void write_chunk(const Chunk &c);

	static bool registered;

protected:
	void do_header();
	void do_footer();

	std::vector<std::string> get_columns_from_tag(const Tag& t);
	bool tag_has_superpos(const Tag& tag);

private:
	Tagset myTagset;
};

} /* end ns Corpus2 */

#endif // CONLLWRITER_H
