/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

#include <libtoki/tokenizer/layertokenizer.h>
#include <libtoki/sentencesplitter.h>
#include <boost/foreach.hpp>

#include <libmaca/morph/dispatchanalyser.h>
#include <libmaca/util/settings.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>

int main(int argc, char** argv)
{
	std::string config;
	if (argc > 1) config = argv[1];
	else config = "morfeusz";	
	Maca::Config::Node cfg = Maca::get_named_config(config);
	
	Toki::LayerTokenizer tokr(std::cin);
	Toki::SentenceSplitter sentr(tokr);
	
	Maca::DispatchAnalyser analyser(cfg);
	
	while (sentr.has_more()) {
		boost::scoped_ptr<Toki::Sentence> sentence(sentr.get_next_sentence());
		assert(!sentence->empty());
		boost::scoped_ptr<Corpus2::Sentence> analysed_sentence(analyser.process_dispose(sentence.get()));
		std::cout << "{\n";
		BOOST_FOREACH(Corpus2::Token *tok, analysed_sentence->tokens())
		{
			std::cout << "\t" << tok->orth_utf8();
			BOOST_FOREACH(const Corpus2::Lexeme &lex, tok->lexemes())
			{
				std::cout << "\n\t\t" << lex.lemma_utf8();
			}
			std::cout << "\n";
		};
		std::cout << "}\n";
	}
}
