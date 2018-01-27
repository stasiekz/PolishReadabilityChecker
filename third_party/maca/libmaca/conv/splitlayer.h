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

#ifndef LIBMACA_CONV_SPLITLAYER_H
#define LIBMACA_CONV_SPLITLAYER_H

#include <libmaca/conv/layer.h>
#include <libmaca/conv/predicate.h>
#include <libmaca/util/confignode.h>
#include <deque>
#include <unicode/regex.h>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace Maca {
namespace Conversion {

/**
 * A layer to split tokens matching specific criteria.
 *
 * Some precondition predicates are first checked, then a regexp
 * is matched on the token's orth. If the regexp matches, tokens will be
 * created with orths corresponding to the capturing groups (in this case,
 * the regexp should have exactly two capturing groups). The resulting
 * tokens are referred to as token 1 and token 2.
 *
 * Corpus2::Token 1 retains the lexemes of the original token. Token 2 is created
 * with a pre-set lexeme, with some attributes possibly copied from the
 * original token's lexemes.
 *
 * Finally, some postcondition predicates are appliked on output token 1.
 */
class TwoSplitLayer : public OneTagsetLayer, boost::noncopyable
{
public:
	/// Constructor for an empty TwoSplitLayer working within a tagset
	TwoSplitLayer(const Corpus2::Tagset& tagset);

	/**
	 * Config node constructor. Recognized keys are:
	 * - regexp - the regular expression used for orth spliiting, should
	 *            have exactly two capturing groups.
	 * - pre - precondition predicates, can appear multiple times
	 * - t1_post - postconditions predicates of token 1, can appear
	 *             multiple times
	 * - copy_attrs_to_t2 - attributes to copy to token 2 from the original
	 *                      token's lexemes, can appear multiple times
	 * - t2_lemma - lemma of the token 2 lexeme
	 * - t2_tag - tag of the token 2 lexeme
	 */
	TwoSplitLayer(const Config::Node& cfg);

	/// Destructor
	~TwoSplitLayer();

	TwoSplitLayer* clone() const;

	void set_orth_regexp(const std::string& regexp_string);

	void add_precondition(const TagPredicate& tp);

	void add_precondition(const std::string& pred_string);

	void add_t1_postcondition(const TagPredicate& tp);

	void add_t1_postcondition(const std::string& pred_string);

	void add_copy_attr_to_t2(Corpus2::idx_t a);

	void append_copy_attrs_to_t2(const std::string& a);

	void set_t2_lexeme(const Corpus2::Lexeme& lex);

	Corpus2::Token* get_next_token();

protected:
	void clone_helper(TwoSplitLayer* copy) const;

	std::deque<Corpus2::Token*> queue_;

	boost::scoped_ptr<RegexMatcher> orth_matcher_;

	boost::shared_ptr<RegexPattern> orth_pattern_;

	std::vector<TagPredicate> pre_;

	std::vector<TagPredicate> t1_post_;

	Corpus2::mask_t copy_attrs_to_t2_;

	Corpus2::Lexeme t2_lexeme_;
};

/**
 * Three-token variant split layer. Main difference from TwoSplitLayer
 * is that the regexp should have three capturing groups, otherwise this
 * behaves similarily. The resulting split tokens are referred to as tokens
 * 1, 2 and 3.
 */
class ThreeSplitLayer : public TwoSplitLayer
{
public:
	/// Constructor for an empty ThreeSplitLayer working within a tagset
	ThreeSplitLayer(const Corpus2::Tagset& tagset);

	/**
	 * Config node constructor. Recognized keys are:
	 * - copy_attrs_to_t3 - attributes to copy to token 3 from the original
	 *                      token's lexemes, can appear multiple times
	 * - t3_lemma - lemma of the token 3 lexeme
	 * - t3_tag - tag of the token 3 lexeme
	 */
	ThreeSplitLayer(const Config::Node& cfg);

	ThreeSplitLayer* clone() const;


	void add_copy_attr_to_t3(Corpus2::idx_t a);

	void append_copy_attrs_to_t3(const std::string& a);

	void set_t3_lexeme(const Corpus2::Lexeme& lex);

	Corpus2::Token* get_next_token();

protected:
	Corpus2::mask_t copy_attrs_to_t3_;

	Corpus2::Lexeme t3_lexeme_;
};

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_CONV_SPLITLAYER_H
