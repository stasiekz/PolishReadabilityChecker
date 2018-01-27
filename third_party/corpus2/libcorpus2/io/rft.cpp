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

#include <libcorpus2/io/rft.h>

#include <boost/foreach.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/make_shared.hpp>
#include <fstream>

namespace Corpus2 {

bool RftWriter::registered = TokenWriter::register_writer<RftWriter>(
		"rft", "mbt,nowarn,colon,alltags,opt,latin2");

RftWriter::RftWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: TokenWriter(os, tagset, params), warn_on_no_lexemes_(true)
	, mbt_dialect_(false), colon_(false), opt_(false), alltags_(false)
{
	BOOST_FOREACH(const string_range& param, params) {
		std::string p = boost::copy_range<std::string>(param);
		if (p == "nowarn") {
			warn_on_no_lexemes_ = false;
		} else if (p == "mbt") {
			mbt_dialect_ = true;
			colon_ = true;
			opt_ = false;
		} else if (p == "alltags") {
			alltags_ = true;
		} else if (p == "opt") {
			opt_ = true;
		} else if (p == "colon") {
			colon_ = true;
		} else if (p == "latin2") {
			encoding_ = p;
		}

	}
}

void RftWriter::write_token(const Token& t)
{
	if (encoding_.empty()) {
		os() << t.orth_utf8();
	} else {
		char buf[256];
		int len = t.orth().extract(0, t.orth().length(), buf, 255, encoding_.c_str());
		if (len < 256) {
			os() << buf;
		} else {
			std::cerr << "Characetr encoding error in codepage rft output\n";
			os() << "???";
		}
	}
	if (t.lexemes().empty()) {
		if (warn_on_no_lexemes_) {
			std::cerr << "No lexemes for token!";
		}
	} else if (alltags_) {
		BOOST_FOREACH(const Lexeme& lex, t.lexemes()) {
			os() << "\t";
			write_tag(lex.tag());
		}
	} else {
		const Lexeme& pref = t.get_preferred_lexeme(tagset());
		os() << "\t";
		write_tag(pref.tag());
	}
	os() << "\n";
}

void RftWriter::write_sentence(const Sentence& s)
{
	BOOST_FOREACH(const Token* t, s.tokens()) {
		write_token(*t);
	}
	if (mbt_dialect_) {
		os() << "<utt>";
	}
	os() << "\n";
}

void RftWriter::write_chunk(const Chunk& c)
{
	BOOST_FOREACH(const Sentence::ConstPtr& s, c.sentences()) {
		write_sentence(*s);
	}
}

void RftWriter::write_tag(const Tag& tag)
{
	std::string tag_str = opt_ ?
		tagset().tag_to_string(tag) : tagset().tag_to_no_opt_string(tag);
	os() << (colon_
			 ? tag_str // when MBT-compliant, suppress colon substitution
			 : boost::algorithm::replace_all_copy(tag_str, ":", "."));
}

bool RftReader::registered = TokenReader::register_reader<RftReader>("rft",
	"ign,loose,strict,set_disamb,mbt");


RftReader::RftReader(const Tagset& tagset, std::istream& is, bool disamb,
		bool mbt_dialect)
	: BufferedSentenceReader(tagset), is_(&is), disamb_(disamb)
	, mbt_dialect_(mbt_dialect)
{
}

RftReader::RftReader(const Tagset& tagset, const std::string& filename, bool disamb,
		bool mbt_dialect)
	: BufferedSentenceReader(tagset), is_(), disamb_(disamb)
	, mbt_dialect_(mbt_dialect)
{
	is_owned_.reset(new std::ifstream(filename.c_str(), std::ifstream::in));
	if (!this->is_owned_->good()) {
		throw FileNotFound(filename);
	}
	else {
		this->is_ = is_owned_.get();
	}
}

Sentence::Ptr RftReader::actual_next_sentence()
{
	std::string line;
	Sentence::Ptr s;
	while (is().good()) {
		std::getline(is(), line);
		if (line.empty()
			|| (mbt_dialect_ && boost::starts_with(line, "<utt>"))) {
			return s;
		} else {
			size_t tab = line.find('\t');
			if (tab == line.npos || tab == 0 || (tab == line.size() - 1)) {
				std::cerr << "Invalid line: " << line << "\n";
			} else {
				std::string orth = line.substr(0, tab);
				std::string tag_string = line.substr(tab + 1);
				if (!mbt_dialect_) {
					boost::algorithm::replace_all(tag_string, ".", ":");
				}
				Tag tag = parse_tag(tag_string);
				Token* t = new Token();
				t->set_orth(UnicodeString::fromUTF8(orth));
				t->set_wa(PwrNlp::Whitespace::Space);
				if (!s) {
					s = make_sentence();
					t->set_wa(PwrNlp::Whitespace::Newline);
				}
				t->add_lexeme(Lexeme(t->orth(), tag));
				if (disamb_) {
					t->lexemes().back().set_disamb(true);
				}
				s->append(t);
			}
		}
	}
	return s;
}

void RftReader::set_option(const std::string &option)
{
	if (option == "mbt") {
		mbt_dialect_ = true;
	} else if (option == "set_disamb") {
		disamb_ = true;
	} else {
		BufferedSentenceReader::set_option(option);
	}
}

std::string RftReader::get_option(const std::string &option) const
{
	if (option == "mbt") {
		return mbt_dialect_ ? option : "";
	} else if (option == "set_disamb") {
		return disamb_ ? option : "";
	}
	return BufferedSentenceReader::get_option(option);
}

} /* end ns Corpus2 */
