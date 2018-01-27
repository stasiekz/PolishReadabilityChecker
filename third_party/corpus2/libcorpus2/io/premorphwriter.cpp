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

#include <libcorpus2/io/premorphwriter.h>
#include <boost/foreach.hpp>

// for entity encoding
#include <libcorpus2/io/xcescommon.h>

namespace Corpus2 {

bool PremorphWriter::registered = TokenWriter::register_writer<PremorphWriter>(
		"premorph", "chunk");

PremorphWriter::PremorphWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: TokenWriter(os, tagset, params), cid_(0), force_chunk_(false)
{
	BOOST_FOREACH(const string_range& param, params) {
		std::string p = boost::copy_range<std::string>(param);
		if (p == "chunk") {
			force_chunk_ = true;
		}
	}
	do_header();
}

PremorphWriter::~PremorphWriter()
{
	finish();
}

void PremorphWriter::write_token(const Token &t)
{
	os() << PwrNlp::Whitespace::to_whitespace(t.wa());
    encode_xml_entities_into(os(), t.orth_utf8());
}

void PremorphWriter::write_sentence(const Sentence &s)
{
	os() << "<chunk type=\"s\">";
	if (!s.tokens().empty()) {
        encode_xml_entities_into(os(), s[0]->orth_utf8());
	}
	for (size_t i = 1; i < s.tokens().size(); ++i) {
		write_token(*s[i]);
	}
	os() << "</chunk>\n";
}

void PremorphWriter::write_chunk(const Chunk &c)
{
	paragraph_head(c);
	BOOST_FOREACH(const Sentence::ConstPtr& s, c.sentences()) {
		write_sentence(*s);
	}
	os() << "</chunk>\n";
}

void PremorphWriter::do_header()
{
	os() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	os() << "<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">\n";
	os() << "<cesAna";
	os() << " xmlns:xlink=\"http://www.w3.org/1999/xlink\"";
	os() << " version=\"1.0\" type=\"premorph\">\n";
	os() << "<chunkList>\n";
	if (force_chunk_) {
		paragraph_head();
	}
}

void PremorphWriter::do_footer()
{
    needs_footer_ = false;
	if (force_chunk_) {
		os() << "</chunk>\n";
	}
	os() << "</chunkList>\n";
	os() << "</cesAna>\n";
}

void PremorphWriter::paragraph_head()
{
	os() << "<chunk id=\"ch" << ++cid_ << "\""
		<< " type=\"p\">\n";
}

void PremorphWriter::paragraph_head(const Chunk& c)
{
	os() << "<chunk";
	BOOST_FOREACH(const Chunk::attr_map_t::value_type& v, c.attributes()) {
		os() << " " << v.first << "=\"" << v.second << "\"";
	}
	os() << ">\n";
}


} /* end ns Corpus2 */
