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

#include <libcorpus2/token.h>
#include <boost/foreach.hpp>
#include <libcorpus2/tokenmetadata.h>
#include <sstream>
#include <boost/bind.hpp>

namespace Corpus2 {

Token::Token()
	: orth_(), wa_(), lexemes_(), metadata_()
{
}

Token::Token(const UnicodeString &orth, PwrNlp::Whitespace::Enum wa)
	: orth_(orth), wa_(wa), lexemes_()
{
}

Token* Token::create_utf8(const std::string& orth_utf8,
		PwrNlp::Whitespace::Enum wa /*= PwrNlp::Whitespace::Space*/)
{
	return new Token(UnicodeString::fromUTF8(orth_utf8), wa);
}

Token* Token::clone() const
{
	Token* t = new Token();
	t->orth_ = orth_;
	t->wa_ = wa_;
	t->lexemes_ = lexemes_;
	if (metadata_) {
		t->set_metadata_ptr(metadata_->clone());
	}
	return t;
}

const Lexeme& Token::get_preferred_lexeme(const Tagset& tagset) const
{
	size_t idx = get_preferred_lexeme_index(tagset);
	if (idx < lexemes_.size()) {
		return lexemes_[idx];
	} else {
		throw Corpus2Error("No lexemes but best lexeme requested");
	}
}

struct preferred_lexeme_cmp
{
	const Tagset* tagset;

	bool operator()(const Lexeme& l1, const Lexeme& l2) const {
		return (!l1.is_disamb() && l2.is_disamb())
				|| (l1.is_disamb() == l2.is_disamb()
				&& (tagset->get_original_pos_index(l1.tag().get_pos_index()) >
					tagset->get_original_pos_index(l2.tag().get_pos_index())
				|| (l1.tag().get_pos() == l2.tag().get_pos()
				&& l1 > l2)));
	}
};

size_t Token::get_preferred_lexeme_index(const Tagset& tagset) const
{
	if (lexemes_.empty()) {
		return static_cast<size_t>(-1);
	}
	preferred_lexeme_cmp cmp;
	cmp.tagset = &tagset;
	std::vector<Lexeme>::const_iterator pref;
	pref = std::max_element(lexemes_.begin(), lexemes_.end(), cmp);
	return std::distance(lexemes_.begin(), pref);
}

bool Token::has_disamb_lexeme() const
{
	return std::find_if(lexemes().begin(), lexemes().end(),
		boost::bind(&Lexeme::is_disamb, _1)) != lexemes().end();
}

int Token::count_disamb_lexemes() const
{
	return std::count_if(lexemes().begin(), lexemes().end(),
		boost::bind(&Lexeme::is_disamb, _1));
}

std::pair<Token::lexeme_filter_iterator, Token::lexeme_filter_iterator> Token::disamb_lexemes() const
{
	lexeme_filter_iterator f1(boost::bind(&Lexeme::is_disamb, _1), lexemes().begin(), lexemes().end());
	lexeme_filter_iterator f2(boost::bind(&Lexeme::is_disamb, _1), lexemes().end(), lexemes().end());
	return std::make_pair(f1, f2);
}

void Token::make_ign(const Tagset& tagset)
{
	lexemes_.clear();
	add_ign(tagset);
}

void Token::add_ign(const Tagset &tagset)
{
	Lexeme lex(orth_, tagset.make_ign_tag());
	lexemes_.push_back(lex);
}

bool Token::operator ==(const Token& other) const
{
	return orth_ == other.orth_ && wa_ == other.wa_
			&& lexemes_ == other.lexemes_;
}

bool Token::check_duplicate_lexemes() const
{
	std::set<Lexeme> s(lexemes_.begin(), lexemes_.end());
	return s.size() != lexemes_.size();
}

bool Token::remove_duplicate_lexemes()
{
	size_t old_size = lexemes_.size();
	std::sort(lexemes_.begin(), lexemes_.end());
	lexemes_.erase(std::unique(lexemes_.begin(), lexemes_.end(), Lexeme::DisamblessComparator()),
			lexemes_.end());
	return old_size != lexemes_.size();
}

bool Token::orth_pos_match(mask_t pos, const UnicodeString &orth) const
{
	if (orth.length() > 0) {
		if (orth.caseCompare(orth_, 0) != 0) return false;
	}
	if (pos.any()) {
		BOOST_FOREACH(const Lexeme& lex, lexemes_) {
			if (lex.tag().get_pos() != pos) return false;
		}
	}
	return true;
}

void Token::create_metadata()
{
	// metadata_.reset(new TokenMetaData);
	metadata_ = boost::make_shared<TokenMetaData>();
}

} /* end ns Corpus2 */
