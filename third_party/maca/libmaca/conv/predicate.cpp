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

#include <libmaca/conv/predicate.h>
#include <boost/foreach.hpp>
#include <libmaca/exception.h>

namespace Maca {
namespace Conversion {

TagPredicate::TagPredicate(const std::string& name, const Corpus2::Tagset& tagset)
{
	second = tagset.get_value_mask(name);
	if (second.any()) {
		first = tagset.get_attribute_mask(tagset.get_value_attribute(second));
	} else {
		first = tagset.get_attribute_mask(name);
		if (first.none()) {
			second = tagset.get_pos_mask(name);
			if (second.none()) {
				throw MacaError("Predicate string invalid: '" + name +
						"' in tagset " + tagset.name());
			}
		}
	}
}

bool TagPredicate::check(const Corpus2::Tag &tag) const
{
	if (first.any()) {
		return tag.get_values_for(first) == second;
	} else {
		return tag.get_pos() == second;
	}
}

bool TagPredicate::token_match(const Corpus2::Token& t) const
{
	if (first.any()) {
		BOOST_FOREACH(const Corpus2::Lexeme& lex, t.lexemes()) {
			if (lex.tag().get_values_for(first) != second) return false;
		}
	} else {
		BOOST_FOREACH(const Corpus2::Lexeme& lex, t.lexemes()) {
			if (lex.tag().get_pos() != second) return false;
		}
	}
	return true;
}

void TagPredicate::apply(Corpus2::Tag &tag) const
{
	if (first.any()) {
		tag.add_values_masked(second, first);
	} else {
		tag.set_pos(second);
	}
}

void apply_predicates(const std::vector<TagPredicate>& v, Corpus2::Token& t)
{
	BOOST_FOREACH(Corpus2::Lexeme& lex, t.lexemes()) {
		Corpus2::Tag newtag = lex.tag();
		BOOST_FOREACH(const TagPredicate& tp, v) {
			tp.apply(newtag);
		}
		lex.set_tag(newtag);
	}
}

PosOrthPredicate::PosOrthPredicate()
{
}

PosOrthPredicate::PosOrthPredicate(Corpus2::mask_t pos, const UnicodeString &orth)
	: std::pair<Corpus2::mask_t, UnicodeString>(pos, orth)
{
}

bool PosOrthPredicate::check(const Corpus2::Token &token) const
{
	return token.orth_pos_match(first, second);
}

} /* end ns Conversion */
} /* end ns Maca */
