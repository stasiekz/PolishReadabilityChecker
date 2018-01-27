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

#include <libcorpus2/io/orthwriter.h>
#include <boost/foreach.hpp>

namespace Corpus2 {

bool OrthWriter::registered = TokenWriter::register_writer<OrthWriter>(
		"orth", "actual_ws,end_nl");

OrthWriter::OrthWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: TokenWriter(os, tagset, params), actual_ws_(false), end_nl_(false)
{
	BOOST_FOREACH(const string_range& param, params) {
		std::string p = boost::copy_range<std::string>(param);
		if (p == "actual_ws") {
			actual_ws_ = true;
		} else if (p == "end_nl") {
			end_nl_ = true;
		}
	}
}

OrthWriter::~OrthWriter()
{
	if (end_nl_) {
		os() << "\n";
	}
}

void OrthWriter::write_token(const Token &t)
{
	os() << PwrNlp::Whitespace::to_whitespace(t.wa()) << t.orth_utf8();
}

void OrthWriter::write_sentence(const Sentence &s)
{
	if (!s.tokens().empty()) {
		if (actual_ws_) {
			write_token(*s[0]);
		} else {
			os() << s[0]->orth_utf8();
		}
	}
	for (size_t i = 1; i < s.tokens().size(); ++i) {
		write_token(*s[i]);
	}
	if (!actual_ws_) {
		os() << "\n";
	}
}

void OrthWriter::write_chunk(const Chunk &c)
{
	BOOST_FOREACH(const Sentence::Ptr& s, c.sentences()) {
		write_sentence(*s);
		if (!actual_ws_) {
			os() << "\n";
		}
	}
}

} /* end ns Corpus2 */
