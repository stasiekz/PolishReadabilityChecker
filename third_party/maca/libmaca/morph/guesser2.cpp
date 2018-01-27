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

#include <libmaca/morph/guesser2.h>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <morfeusz.h>


namespace Maca {

const char* Guesser2Analyser::identifier = "guesser2";

bool Guesser2Analyser::registered =
		MorphAnalyser::register_analyser<Guesser2Analyser>();

Guesser2Analyser::Guesser2Analyser(const Config::Node &cfg)
	: MorphAnalyser(cfg),
      guesser(cfg.get<boost::filesystem::path>("guesser_data"), tagset())
{
	
}

Guesser2Analyser::Guesser2Analyser(const Corpus2::Tagset *tagset_, const boost::filesystem::path & data)
	: MorphAnalyser(tagset_),
      guesser(data, tagset())
{
}

Guesser2Analyser::~Guesser2Analyser()
{
}

Guesser2Analyser* Guesser2Analyser::clone() const
{
	throw "Not implemented yet";
	Guesser2Analyser* copy = new Guesser2Analyser(*this);
	return copy;
}

bool Guesser2Analyser::process_functional(const Toki::Token &t,
	boost::function<void (Corpus2::Token *)> sink)
{
	const UnicodeString & orth = t.orth();
	const PwrNlp::Whitespace::Enum wa = t.preceeding_whitespace();
	std::vector<Corpus2::Lexeme> lexemes = guesser.guess(orth);
	
	if (!lexemes.empty())
	{
		Corpus2::Token * token = new Corpus2::Token(orth, wa);
		token->replace_lexemes(lexemes);
		sink(token);
		return true;
	}
	
	return false;
}

} /* end ns Maca */
