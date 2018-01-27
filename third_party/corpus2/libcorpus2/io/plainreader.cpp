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

#include <libcorpus2/io/plainreader.h>
#include <boost/foreach.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <fstream>

namespace Corpus2 {

bool PlainReader::registered = TokenReader::register_reader<PlainReader>("plain",
	"ign,loose,strict");


PlainReader::PlainReader(const Tagset& tagset, std::istream& is)
	: BufferedSentenceReader(tagset), is_(&is)
{
}

PlainReader::PlainReader(const Tagset& tagset, const std::string& filename)
	: BufferedSentenceReader(tagset), is_()
{
	is_owned_.reset(new std::ifstream(filename.c_str(), std::ifstream::in));
	if (!this->is_owned_->good()) {
		throw FileNotFound(filename);
	}
	else {
		this->is_ = is_owned_.get();
	}
}

Sentence::Ptr PlainReader::actual_next_sentence()
{
	std::string line;
	Sentence::Ptr s;
	size_t line_no = 0;
	while (is().good()) {
		std::getline(is(), line);
		++line_no;
		if (line.empty()) {
			return s;
		} else {
			std::vector<std::string> fields;
			boost::algorithm::split(fields, line, boost::is_any_of("\t"));
			assert(!fields.empty());
			if (fields[0].empty()) { //lexeme
				if (s->empty()) {
					throw Corpus2Error("PlainReader lexemes without a token at "
							+ boost::lexical_cast<std::string>(line_no));
				}
				if (fields.size() < 3) {
					throw Corpus2Error("PlainReader not enough fields at "
							+ boost::lexical_cast<std::string>(line_no));
				}
				const std::string& lemma = fields[1];
				const std::string& tag_string = fields[2];
				Tag tag = parse_tag(tag_string);
				Token* last_token = s->tokens().back();
				last_token->add_lexeme(Lexeme(UnicodeString::fromUTF8(lemma), tag));
				if (fields.size() > 3 && fields[3] == "disamb") {
					last_token->lexemes().back().set_disamb(true);
				}
			} else { // orth-ws
				Token* t = new Token();
				const std::string& orth = fields[0];
				t->set_orth(UnicodeString::fromUTF8(orth));
				PwrNlp::Whitespace::Enum wa = PwrNlp::Whitespace::Space;
				if (!s) {
					s = make_sentence();
					wa = PwrNlp::Whitespace::Newline;
				}
				if (fields.size() > 1) {
					wa = PwrNlp::Whitespace::from_string(fields[1]);
				}
				t->set_wa(wa);
				s->append(t);
			}
		}
	}
	return s;
}

void PlainReader::set_option(const std::string &option)
{
	BufferedSentenceReader::set_option(option);
}

std::string PlainReader::get_option(const std::string &option) const
{
	return BufferedSentenceReader::get_option(option);
}

} /* end ns Corpus2 */
