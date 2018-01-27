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

#ifndef LIBMACA_CONV_PREDICATE_H
#define LIBMACA_CONV_PREDICATE_H

#include <libcorpus2/tagset.h>
#include <libcorpus2/token.h>
#include <unicode/unistr.h>

namespace Maca {
namespace Conversion {

/**
 * Class for checking if a tag contains some value, has an attribute not
 * set, or has a specific POS. See the constructor for details.
 *
 * Applying the predicate has teh effect of setting the appropriate value
 * or POS so that the tag matches the predicate.
 */
class TagPredicate : public std::pair<Corpus2::mask_t, Corpus2::mask_t>
{
public:
	/**
	 * Create a tag predicate from a tagset and a string identifier of
	 * either a value, attribute or POS.
	 *
	 * If the name resolves to a value, the predicate will check for that
	 * value. If the name resolves to an attribute, the predicate will
	 * match tags with that attribute not set. Finally, if the name
	 * resolves to a POS, the predicate will check for that POS.
	 */
	TagPredicate(const std::string& name, const Corpus2::Tagset& tagset);

	/**
	 * Check the tag matches this predicate
	 */
	bool check(const Corpus2::Tag& tag) const;

	/**
	 * Check all tags of the token match this predicate
	 */
	bool token_match(const Corpus2::Token& t) const;

	/**
	 * Apply the predicate to the tag, modyfying it so it matches the predicate
	 */
	void apply(Corpus2::Tag& tag) const;
};

/**
 * Helper function to apply a number of predicates on a token
 */
void apply_predicates(const std::vector<TagPredicate>& v, Corpus2::Token& t);

class PosOrthPredicate : public std::pair<Corpus2::mask_t, UnicodeString>
{
public:
	PosOrthPredicate();

	PosOrthPredicate(Corpus2::mask_t pos, const UnicodeString& orth);

	bool check(const Corpus2::Token& token) const;
};

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_CONV_PREDICATE_H
