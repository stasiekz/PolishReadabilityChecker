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

#include <libcorpus2/io/xcescommon.h>
#include <libcorpus2/io/xceswriter.h>
#include <boost/foreach.hpp>

namespace Corpus2 {

bool XcesWriter::registered = TokenWriter::register_writer<XcesWriter>("xces",
		"flat,chunk,nochunk,nodisamb,sorttags,split,ws");

XcesWriter::XcesWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: XmlWriter(os, tagset, params)
	, force_chunk_(false)
	, split_chunks_on_newlines_(false)
{
	BOOST_FOREACH(const string_range& param, params) {
		std::string p = boost::copy_range<std::string>(param);
		if (p == "chunk") {
			force_chunk_ = true;
		} else if (p == "nochunk") {
			force_chunk_ = false;
		} else if (p == "split") {
			split_chunks_on_newlines_ = true;
		}
	}
	do_header();
}

XcesWriter::~XcesWriter()
{
	finish();
}

void XcesWriter::write_sentence(const Sentence& s)
{
	osi() << "<chunk type=\"s\">\n";
	XmlWriter::write_sentence(s);
	osi() << "</chunk>\n";
}

void XcesWriter::write_chunk(const Chunk &c)
{
	bool new_chunk = true;
	BOOST_FOREACH(const Sentence::ConstPtr& s, c.sentences()) {
		if (split_chunks_on_newlines_ && !s->tokens().empty()) {
			const Token* first = s->first_token();
			if (first->wa() == PwrNlp::Whitespace::ManyNewlines) {
				new_chunk = true;
			}
		}
		if (new_chunk) {
			paragraph_head(c);
			if (use_indent_) indent_more();
			new_chunk = false;
		}
		write_sentence(*s);
	}
	if (new_chunk) { //no sentences in chunk
		paragraph_head(c);
	}
	if (use_indent_ && !new_chunk) indent_less();
	osi() << "</chunk>\n";
}

void XcesWriter::do_header()
{
	XmlWriter::do_header();
	os() << "<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">\n";
	os() << "<cesAna";
	os() << " xmlns:xlink=\"http://www.w3.org/1999/xlink\"";
	os() << " version=\"1.0\" type=\"lex disamb\">\n";
	os() << "<chunkList>\n";
	if (use_indent_) indent_more();
	if (force_chunk_) {
		paragraph_head();
		if (use_indent_) indent_more();
	}
}

void XcesWriter::do_footer()
{
    needs_footer_ = false;
	if (force_chunk_) {
		if (use_indent_) indent_less();
		osi() << "</chunk>\n";
	}
	if (use_indent_) indent_less();
	os() << "</chunkList>\n";
	os() << "</cesAna>\n";
}


//void XcesWriter::paragraph_head()
//{
//	osi() << "<chunk id=\"ch" << ++cid_ << "\""
//		<< " type=\"p\">\n";
//}

//void XcesWriter::paragraph_head(const Chunk& c)
//{
//	osi() << "<chunk";
//	BOOST_FOREACH(const Chunk::attr_map_t::value_type& v, c.attributes()) {
//		os() << " " << v.first << "=\"" << v.second << "\"";
//	}
//	os() << ">\n";
//}

} /* end ns Corpus2 */
