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

#include <libcorpus2/tagging.h>
#include <libcorpus2/lexeme.h>


#include <boost/foreach.hpp>
#include <libpwrutils/bitset.h>

namespace Corpus2 {

Tag get_attribute_mask(const Tagset& tagset, std::string attr_name)
{
	if(attr_name.empty())
	{
		// return all-POS mask
		Tag t;

		for (idx_t pos_idx = 0; pos_idx < tagset.pos_count(); ++pos_idx) {
			t.add_pos(tagset.get_pos_mask(pos_idx));
		}

		return t;
	}
	else
	{
		return Tag(0, tagset.get_attribute_mask(attr_name));
	}
}

Tag mask_token(const Token& token, const Tag& mask, bool disamb_only)
{
	Tag t;
	BOOST_FOREACH(const Corpus2::Lexeme& lexeme, token.lexemes()) {
		if(lexeme.is_disamb() || !disamb_only) {
			t.combine_with(lexeme.tag().get_masked(mask));
		}
	}
	return t;
}

int mask_card(const Tag& mask)
{
	return PwrNlp::count_bits_set(mask.get_pos())
			+ PwrNlp::count_bits_set(mask.get_values());
}

Tag with_values_masked(Tag input, Tag attr_value, Tag attr_mask) {
	Tag output(input);
	output.add_values_masked(attr_value.get_values(), attr_mask.get_values());
	return output;
}

bool select_preferred_disamb(const Tagset& tagset, Token* token)
{
	size_t lex_idx = token->get_preferred_lexeme_index(tagset);
	if(!token->lexemes()[lex_idx].is_disamb()) {
		return false; // disamb would've taken precedence => no disamb at all
	}

	for (size_t other_idx = 0;
		 other_idx < token->lexemes().size();
		 ++other_idx) {
		if (other_idx != lex_idx) {
			token->lexemes()[other_idx].set_disamb(false);
		}
	}
	return true;
}

void select_preferred_lexeme(const Tagset& tagset, Token* token)
{
	BOOST_FOREACH(Lexeme& lex, token->lexemes()) {
		lex.set_disamb(true);
	}
	if (token->lexemes().size() > 1) {
		std::vector<Lexeme> one;
		one.push_back(token->get_preferred_lexeme(tagset));
		token->replace_lexemes(one);
	}
}

bool select_preferred_disamb_tag(const Tagset& tagset, Token* token)
{
	const Corpus2::Lexeme &prototypical = token->get_preferred_lexeme(tagset);
	if(!prototypical.is_disamb()) {
		return false; // disamb would've taken precedence => no disamb at all
	}
	BOOST_FOREACH(Lexeme& lex, token->lexemes()) {
		if (lex.tag() != prototypical.tag()) {
			lex.set_disamb(false);
		}
	}
	return true;
}

void select_preferred_tag(const Tagset& tagset, Token* token)
{
	BOOST_FOREACH(Lexeme& lex, token->lexemes()) {
		lex.set_disamb(true);
	}
	if (token->lexemes().size() > 1) {
		const Corpus2::Tag tag_wanted = token->get_preferred_lexeme(tagset).tag();
		std::vector<Lexeme> wanted;
		BOOST_FOREACH(const Lexeme& lex, token->lexemes()) {
			if (lex.tag() == tag_wanted) {
				wanted.push_back(lex);
			}
		}
		assert(!wanted.empty());
		token->replace_lexemes(wanted);
	}
}

void expand_optional_attrs(const Tagset& tagset, Token* token)
{
	BOOST_FOREACH(Lexeme& lex, token->lexemes()) {
		lex.set_tag(tagset.expand_optional_attrs(lex.tag()));
	}
}

void select_singular_tags(const Tagset& tagset, Token* token)
{
	BOOST_FOREACH(Lexeme& lex, token->lexemes()) {
		lex.set_tag(tagset.select_singular(lex.tag()));
	}
}

bool disambiguate_equal(Token* token, const Tag& mask_where,
						const Tag& mask_wanted)
{
	std::vector<Lexeme> wanted;
	BOOST_FOREACH(const Lexeme& lex, token->lexemes()) {
		Tag mask_theirs = lex.tag().get_masked(mask_where);
		if (mask_theirs == mask_wanted) {
			wanted.push_back(lex);
		}
	}
	if (wanted.empty()) {
		return false;
	}
	token->replace_lexemes(wanted);
	return true;
}

bool disambiguate_subset(Token* token, const Tag& mask_where,
						const Tag& mask_wanted)
{
	std::vector<Lexeme> wanted;
	BOOST_FOREACH(const Lexeme& lex, token->lexemes()) {
		Tag mask_theirs = lex.tag().get_masked(mask_where);
		if (mask_theirs.get_masked(mask_wanted) == mask_theirs) {
			wanted.push_back(lex);
		}
	}
	if (wanted.empty()) {
		return false;
	}
	token->replace_lexemes(wanted);
	return true;
}

bool disambiguate_lemma(Token* token, const std::string& lemma_utf8)
{
	std::vector<Lexeme> wanted;
	UnicodeString u_lemma(UnicodeString::fromUTF8(lemma_utf8));
	BOOST_FOREACH(const Lexeme& lex, token->lexemes()) {
		if (lex.lemma() == u_lemma) {
			wanted.push_back(lex);
		}
	}
	if (wanted.empty()) {
		return false;
	}
	token->replace_lexemes(wanted);
	return true;
}

void overwrite_lemmas(Token* token, const std::string& lemma_utf8)
{
	UnicodeString u_lemma(UnicodeString::fromUTF8(lemma_utf8));
	BOOST_FOREACH(Lexeme& lex, token->lexemes()) {
		lex.set_lemma(u_lemma);
	}
}

void lowercase_lemmas(Token* token)
{
	BOOST_FOREACH(Lexeme& lex, token->lexemes()) {
		lex.lowercase_lemma();
	}
}

void set_disambs(Token *token, const Tag& wanted_tag)
{
	BOOST_FOREACH(Lexeme& lex, token->lexemes()) {
		lex.set_disamb(lex.tag() == wanted_tag);
	}
}

} /* end ns Corpus2 */
