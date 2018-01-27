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

#ifndef LIBMACA_CONV_JOINRULE_H
#define LIBMACA_CONV_JOINRULE_H

#include <libcorpus2/tagset.h>
#include <libcorpus2/token.h>
#include <libmaca/conv/predicate.h>
#include <boost/function.hpp>
#include <libmaca/util/confignode.h>

namespace Maca {
namespace Conversion {

/**
 * A single rule for joining two tokens together and forming another token,
 * all within one tagset.
 *
 * First, some conditions are checked on both tokens that are considered
 * for joining (not that the no-space requirement os checked elsewhere).

 * Then, if the checks pass, the orths are appended and the lexemes taken
 * from token 1.

 * Then some attrobute values from token 2 lexemes are put into the joined
 * token. If token 2 lexemes contain more than one value for some of the
 * attributes, the joined token will have all the possible combinations.
 *
 * Then some postcondition predicates are applied on the joined token.
 */
class JoinRule
{
public:
	/**
	 * Constructor.
	 *
	 * A newly created JoinRule has no conditions and will always join
	 * tokens.
	 *
	 */
	explicit JoinRule(const Corpus2::Tagset& tagset);

	/**
	 * Constructor from a Config::Node.
	 *
	 * The recognized keys are:
	 * - tagset - the tagset name
	 * - t1_pos - POS string to match in all token 1 tags
	 * - t1_orth - orth string to match in token 1
	 * - t2_pos - POS string to match in all token 2 tags
	 * - t2_orth - orth string to match in token 2
	 * - copy_attr - attribute to copy from token 2 tags into the joine
	 *               token tags, can appear multiple times to copy more
	 *               than one attribute
	 * - post - postcondition predicate to apply on the resulting token,
	 *          can appear multiple times
	 */
	explicit JoinRule(const Config::Node& cfg);

	const Corpus2::Tagset& tagset() const {
		return *tagset_;
	}

	/**
	 * The main token joining function -- try to join the two given tokens
	 * and return the resulting token, or NULL if the joing did not happen.
	 *
	 * If the tokens are joined, the passed tokens are disposed of by the
	 * JoinRule. Otherwise, the return value is NULL and the passed tokens
	 * are unchanged.
	 */
	Corpus2::Token* try_join(Corpus2::Token* t1, Corpus2::Token* t2) const;

	/// Setter for token 1 preconditions
	void set_token1_preconditions(const PosOrthPredicate& pre);

	/// Setter for token 1 preconditions, string version
	void set_token1_preconditions(const std::string& pos,
			const UnicodeString& orth);

	/// Setter for token 2 preconditions
	void set_token2_preconditions(const PosOrthPredicate& pre);

	/// Setter for token 2 preconditions, string version
	void set_token2_preconditions(const std::string& pos,
			const UnicodeString& orth);

	/// Setter for the list of attriutes to copy from token2 to the joined
	/// token
	void set_copy_attrs(Corpus2::mask_t mask);

	/// Adder for the copy attributes list, string version
	void append_copy_attrs(const std::string& names);

	/// Postcondidtion adder
	void add_postcondition(const TagPredicate& tp);

	/// Postcondition adder, string version
	void add_postcondition(const std::string& pred_string);

private:
	/// The tagset the joining takes place in
	const Corpus2::Tagset* tagset_;

	/// Precondition for token 1
	PosOrthPredicate pre1_;

	/// Precondition for token 2
	PosOrthPredicate pre2_;

	/// Attributes to copy from token 2 tags into the resulting token tags
	Corpus2::mask_t copy_t2_attrs_;

	/// Postconditions to apply on the resulting token's tags
	std::vector<TagPredicate> post_;
};

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_CONV_JOINRULE_H
