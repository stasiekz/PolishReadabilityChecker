/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

#include <libmaca/morph/guesser.h>
#include <boost/algorithm/string.hpp>
#include <Corpus/guesser_api.h>
#include <algorithm>
#include <morfeusz.h>


namespace Maca {

const char* GuesserAnalyser::identifier = "guesser";

bool GuesserAnalyser::registered =
		MorphAnalyser::register_analyser<GuesserAnalyser>();

GuesserAnalyser::GuesserAnalyser(const Config::Node &cfg)
	: MorphAnalyser(cfg)
{
	setup_corpus1();
}

GuesserAnalyser::GuesserAnalyser(const Corpus2::Tagset *tagset)
	: MorphAnalyser(tagset)
{
	setup_corpus1();
}

void GuesserAnalyser::setup_corpus1()
{
	// we operate in UTF8
	SetCorpusEncoding(GUESSER_UTF8);
	// if unexpected tags come out of Guesser (or actually
	// Morfeusz+Guesser system), just warn instead of halting
	AllowMorfErrors();
}

GuesserAnalyser::~GuesserAnalyser()
{
}

GuesserAnalyser* GuesserAnalyser::clone() const
{
	GuesserAnalyser* copy = new GuesserAnalyser(*this);
	return copy;
}

bool GuesserAnalyser::process_functional(const Toki::Token &t,
	boost::function<void (Corpus2::Token *)> sink)
{
	std::string orthu8 = t.orth_utf8();
	// feed the orth through the guesser
	const char* guesser_response = GuessForm(orthu8.c_str());
	if (!guesser_response) {
		throw MacaError("Guesser returned NULL");
	}
	std::string gs(guesser_response);
	std::vector<std::string> lines;
	boost::algorithm::split(lines, gs, boost::is_any_of("\n"));
	PwrNlp::Whitespace::Enum wa = t.preceeding_whitespace();
	bool sunk = false;
	// NOTE: the Guesser API may also run Morfeusz.
	// If the output is multi-token, it clearly comes from Morfeusz,
	// so we immediately reject multi-line guesser responses.
	// Also note that single-token output may be from the guesser,
	// but it might also come from Morfeusz -- we can't check it via
	// this simple API.
	std::vector<std::string> nonempty_lines;
	BOOST_FOREACH(const std::string& line, lines) {
		if (!line.empty()) {
			nonempty_lines.push_back(line);
		}
	}
	
	if (nonempty_lines.size() == 1 ) {
		const std::string line = nonempty_lines[0];
		std::vector<std::string> olt; // orth-lemma-tag
		boost::algorithm::split(olt, line, boost::is_any_of(" \t"));
		if (((olt.size() - 1) % 2 != 0) || (olt.size() < 3)) {
			throw MacaError("Unexpected orth-lemma-tag line returned by Guesser: " + line);
		}
		// UnicodeString the_orth = UnicodeString::fromUTF8(olt[0]);
		// take orth from input unchanged
		std::auto_ptr<Corpus2::Token> tt(new Corpus2::Token(t.orth(), wa));
		wa = PwrNlp::Whitespace::None;
		size_t lexeme_start_idx = 1;
		while (lexeme_start_idx + 1 < olt.size()) {
			if (olt[lexeme_start_idx + 1] != "ign") {
				UnicodeString the_lemma = UnicodeString::fromUTF8(olt[lexeme_start_idx]);
				Corpus2::Tag the_tag = tagset().parse_simple_tag(olt[lexeme_start_idx + 1]);
				Corpus2::Lexeme lex(the_lemma, the_tag);
				tt->add_lexeme(lex);
			}
			lexeme_start_idx += 2;
		}
		if (sunk || !tt->lexemes().empty()) {
			sink(tt.release());
			sunk = true;
		}
	}
	return sunk;
}

} /* end ns Maca */
