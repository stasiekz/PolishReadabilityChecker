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
#include <libcorpus2/io/xmlwriter.h>
#include <boost/foreach.hpp>

namespace Corpus2 {

XmlWriter::XmlWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: TokenWriter(os, tagset, params), cid_(0)
	, use_indent_(true), output_disamb_(true)
	, sort_tags_(false), whitespace_info_(false)
{
	BOOST_FOREACH(const string_range& param, params) {
		std::string p = boost::copy_range<std::string>(param);
		if (p == "flat") {
			use_indent_ = false;
		} else if (p == "nodisamb") {
			output_disamb_ = false;
		} else if (p == "sorttags") {
			sort_tags_ = true;
		} else if (p == "ws") {
			whitespace_info_ = true;
		}
	}
}

XmlWriter::~XmlWriter()
{
}

void XmlWriter::write_token(const Token &t)
{
    token_as_xces_xml(sstream(), tagset(), t, use_indent_ ? indent_level() : -1,
            output_disamb_, sort_tags_, whitespace_info_);
}

void XmlWriter::write_sentence(const Sentence& s)
{
	if (use_indent_) indent_more();
	BOOST_FOREACH(const Token* t, s.tokens()) {
		write_token(*t);
	}
	if (use_indent_) indent_less();
}


void XmlWriter::do_header()
{
    sstream() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
}

void XmlWriter::do_footer()
{
}

void XmlWriter::paragraph_head()
{
    ssi() << "<chunk id=\"autoch";
    sstream() << ++cid_ << "\"" << " type=\"p\">\n";
}

void XmlWriter::paragraph_head(const Chunk& c)
{
    ssi() << "<chunk";
	BOOST_FOREACH(const Chunk::attr_map_t::value_type& v, c.attributes()) {
        sstream() << " " << v.first << "=\"" << v.second << "\"";
	}
    sstream() << ">\n";
}

} /* end ns Corpus2 */
