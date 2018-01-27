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

#include <libcorpus2/io/plainwriter.h>
#include <boost/foreach.hpp>

namespace Corpus2 {

bool PlainWriter::registered = TokenWriter::register_writer<PlainWriter>(
	"plain", "nows,no_disamb_info,disamb_only,ds");

PlainWriter::PlainWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: TokenWriter(os, tagset, params), ws_(true), disamb_(true)
	, disamb_only_(false)
{
	BOOST_FOREACH(const string_range& param, params) {
		std::string p = boost::copy_range<std::string>(param);
		if (p == "nows") {
			ws_ = false;
		} else if (p == "no_disamb_info") {
			disamb_ = false;
		} else if (p == "disamb_only") {
			disamb_only_ = true;
		} else if (p == "ds") {
			disamb_ = false;
			disamb_only_ = true;
		}
	}
}

void PlainWriter::write_token(const Token &t)
{
	os() << t.orth_utf8();
	if (ws_) {
		os() << "\t" << PwrNlp::Whitespace::to_string(t.wa());
	}
	os() << "\n";
	BOOST_FOREACH(const Lexeme& lex, t.lexemes()) {
		if (!disamb_only_ || lex.is_disamb()) {
			os() << "\t" << lex.lemma_utf8() << "\t"
				<< tagset().tag_to_string(lex.tag());
			if (disamb_) {
				if (lex.is_disamb()) {
					os() << "\t";
					os() << "disamb";
				}
			}
			os() << "\n";
		}
	}
}
void PlainWriter::write_sentence(const Sentence &s)
{
	BOOST_FOREACH(const Token* t, s.tokens()) {
		write_token(*t);
	}
	os() << "\n";
}

void PlainWriter::write_chunk(const Chunk& c)
{
	BOOST_FOREACH(const boost::shared_ptr<Sentence>& s, c.sentences()) {
		write_sentence(*s);
	}
	os() << "\n";
}

} /* end ns Corpus2 */
