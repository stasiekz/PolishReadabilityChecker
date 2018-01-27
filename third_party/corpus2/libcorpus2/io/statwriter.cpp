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

#include <libcorpus2/io/statwriter.h>
#include <libcorpus2/ann/annotatedsentence.h>
#include <iomanip>
#include <boost/foreach.hpp>

namespace Corpus2 {

bool StatWriter::registered = TokenWriter::register_writer<StatWriter>(
	"stat");

StatWriter::StatWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: TokenWriter(os, tagset, params)
{
}

void StatWriter::write_token(const Token& t)
{
	if (t.orth().length() == 0) {
		os() << "~";
	} else {
		UnicodeString o1(t.orth().charAt(0));
		os() << PwrNlp::to_utf8(o1);
	}
}


void StatWriter::write_sentence(const Sentence& s)
{
	os() << std::setw(8) << "tokens" << " ";
	BOOST_FOREACH(const Token* t, s.tokens()) {
		write_token(*t);
	}
	os() << "\n";
	const AnnotatedSentence* as = dynamic_cast<const AnnotatedSentence*>(&s);
	if (as) {
		BOOST_FOREACH(const AnnotatedSentence::chan_map_t::value_type& vt, as->all_channels()) {
			os() << std::setw(8) << vt.first << " ";
			os() << vt.second.dump_alpha();
			os() << "\n";
		}
	}
	os() << "\n";
}

void StatWriter::write_chunk(const Chunk& c)
{
	BOOST_FOREACH(const Sentence::Ptr s, c.sentences()) {
		write_sentence(*s);
	}
	os() << "\n";
}


} /* end ns Corpus2 */
