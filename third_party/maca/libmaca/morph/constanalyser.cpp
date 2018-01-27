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

#include <libmaca/morph/constanalyser.h>

namespace Maca {

const char* ConstAnalyser::identifier = "const";

bool ConstAnalyser::registered =
		MorphAnalyser::register_analyser<ConstAnalyser>();

ConstAnalyser::ConstAnalyser(const Corpus2::Tagset *tagset, const std::string &tag)
	: MorphAnalyser(tagset), tag_(tagset->parse_simple_tag(tag)), lower_lemma_(false)
{
}

ConstAnalyser::ConstAnalyser(const Corpus2::Tagset *tagset, const Corpus2::Tag &tag)
	: MorphAnalyser(tagset), tag_(tag), lower_lemma_(false)
{
}

ConstAnalyser::ConstAnalyser(const Config::Node& cfg)
	: MorphAnalyser(cfg), tag_(), lower_lemma_(false)
{
	std::string tag_string = cfg.get("tag", "");
	if (tag_string.empty()) {
		throw ConfigValueMissing("tag", "ConstAnalyser");
	}
	tag_ = tagset().parse_simple_tag(tag_string);
	lower_lemma_ = cfg.get("lower_lemma", false);
}

ConstAnalyser* ConstAnalyser::clone() const
{
	return new ConstAnalyser(*this);
}

bool ConstAnalyser::process_functional(const Toki::Token &t,
		boost::function<void (Corpus2::Token*)> sink)
{
	Corpus2::Token* tt = create_from_toki(t);
	UnicodeString lemma = t.orth();
	if (lower_lemma_) {
		lemma.toLower();
	};
	tt->add_lexeme(Corpus2::Lexeme(lemma, tag_));
	sink(tt);
	return true;
}

} /* end ns Maca */
