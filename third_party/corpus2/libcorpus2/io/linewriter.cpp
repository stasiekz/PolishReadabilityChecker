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

#include <libcorpus2/io/linewriter.h>
#include <libcorpus2/io/linewriter.h>
#include <libcorpus2/ann/annotatedsentence.h>
#include <iomanip>
#include <boost/foreach.hpp>

namespace Corpus2 {

bool LineWriter::registered = TokenWriter::register_writer<LineWriter>(
	"line");

LineWriter::LineWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: TokenWriter(os, tagset, params)
{
}

void LineWriter::write_token(const Token& t)
{
	os() << t.orth_utf8();
}


void LineWriter::write_sentence(const Sentence& s)
{
	const std::string tok_name("Tokens");
	const std::string empty_char(" ");
	const std::string chunk_char("\xe2\x94\x80");
	//const std::string head_char("\xe2\x94\x81"); // hardcoded utf-8
	const std::string head_char("\xe2\x95\x90"); // hardcoded utf-8

	const AnnotatedSentence* as = dynamic_cast<const AnnotatedSentence*>(&s);
	// get longest channel name for padding
	int name_padding = tok_name.length();

	if (as) {
		BOOST_FOREACH(const AnnotatedSentence::chan_map_t::value_type& vt, as->all_channels()) {
			const int that_len = vt.first.length();
			if (that_len > name_padding) {
				name_padding = that_len;
			}
		}
	}

	// dump token orths and remember orth lengths
	std::vector<int> orth_lens;
	os() << std::left << std::setw(name_padding) << tok_name;
	BOOST_FOREACH(const Token* t, s.tokens()) {
		os() << " " << t->orth_utf8();
		orth_lens.push_back(t->orth().length());
	}
	os() << "\n";

	// dump channel line representations
	if (as) {
		AnnotatedSentence* hax = const_cast<AnnotatedSentence*>(as); // sorry
		BOOST_FOREACH(const AnnotatedSentence::chan_map_t::value_type& vt, hax->all_channels()) {
			os() << std::left << std::setw(name_padding) << vt.first;
			// use IOB2 representation internally
			AnnotationChannel &chan = hax->get_channel(vt.first);
			chan.make_iob_from_segments();
			// write line representation
			for (int idx = 0; idx < chan.size(); idx++) {
				IOB::Enum this_tag = chan.get_iob_at(idx);
				if (this_tag == IOB::I) {
					os() << chunk_char;
				}
				else {
					os() << empty_char;
				}
				std::string now(empty_char);
				if (this_tag != IOB::O) {
					if (chan.is_head_at(idx)) {
						now = head_char;
					}
					else {
						now = chunk_char;
					}
				}
				for (int line_pos = orth_lens[idx]; line_pos > 0; line_pos--) {
					os() << now;
				}
			}
			os() << "\n";
		}
	}
	os() << "\n";
}

void LineWriter::write_chunk(const Chunk& c)
{
	BOOST_FOREACH(const Sentence::Ptr s, c.sentences()) {
		write_sentence(*s);
	}
	os() << "\n";
}


} /* end ns Corpus2 */
