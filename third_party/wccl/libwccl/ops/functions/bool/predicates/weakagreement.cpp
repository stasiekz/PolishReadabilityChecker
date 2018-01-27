/*
    Copyright (C) 2011 Adam Wardyński, Tomasz Śniatowski, Paweł Kędzia,
    Adam Radziszewski, Bartosz Broda
    Part of the WCCL project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <libwccl/ops/functions/bool/predicates/weakagreement.h>
#include <boost/foreach.hpp>

namespace Wccl {

std::string WeakAgreement::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream ss;
	ss << name(tagset) << "("
		<< left_pos_expr_->to_string(tagset) << ", "
		<< right_pos_expr_->to_string(tagset) << ", "
		<< attribs_expr_->to_string(tagset) << ")";
	return ss.str();
}

std::ostream& WeakAgreement::write_to(std::ostream& os) const
{
	return os << raw_name() << "("
			<< *left_pos_expr_ << ", "
			<< *right_pos_expr_ << ", "
			<< *attribs_expr_ << ")";
}

WeakAgreement::BaseRetValPtr WeakAgreement::apply_internal(const FunExecContext& context) const
{
	const SentenceContext& sc = context.sentence_context();

	const boost::shared_ptr<const Position>& range_left = left_pos_expr_->apply(context);
	if (range_left->get_value() == Position::Nowhere) {
		return Predicate::False(context);
	}
	const boost::shared_ptr<const Position>& range_right = right_pos_expr_->apply(context);
	if (range_right->get_value() == Position::Nowhere) {
		return Predicate::False(context);
	}
	int abs_left, abs_right;
	if (!sc.validate_range(*range_left, *range_right, abs_left, abs_right)) {
		return Predicate::False(context);
	}
	const boost::shared_ptr<const TSet>& attribs = attribs_expr_->apply(context);

	int min_card = attribs->categories_count(*tagset_);
	const Corpus2::Token* t1 = sc.at(abs_left);
	const Corpus2::Token* t2 = sc.at(abs_right);
	// to optimize a bit, make sure t1 is the one with less lexemes
	if (t1->lexemes().size() > t2->lexemes().size()) {
		std::swap(t1, t2);
	}
	// Check strong agreement between range endpoints.
	// For each possible agreement between the endpoints,
	// check if remaining tokens meet that agreement too,
	// but instead of looking for strong agreement i.e.
	// matching on exact number of categories, look for
	// weak agreement i.e. matching only on those categories
	// that are present.
	// Specifically, if there is a lexeme that does not
	// match any of the categories, that means the token
	// does meet the weak agreement.
	BOOST_FOREACH (const Corpus2::Lexeme& t1_lex, t1->lexemes()) {
		const Corpus2::Tag& t1_tag = t1_lex.tag();
		// don't bother checking t2 unless current t1_tag matches enough categories
		if (attribs->matching_categories(t1_tag) >= min_card) {
			BOOST_FOREACH (const Corpus2::Lexeme& t2_lex, t2->lexemes()) {
				Corpus2::Tag inter = t1_tag.get_masked(t2_lex.tag());
				// if the intersection matches enough categories we have agreement
				if (attribs->matching_categories(inter) >= min_card) {
					// Check if selected agreement is met by all remaining tokens
					bool agreement_met = true;
					for(int i = abs_left + 1; agreement_met && (i < abs_right); ++i) {
						BOOST_FOREACH (const Corpus2::Lexeme& i_lex, sc.at(i)->lexemes()) {
							// Check if agreement is met, but taking into account
							// only categories actually matched in current tag,
							// without requirement to match all categories in the
							// agreement.
							Corpus2::Tag i_inter = i_lex.tag().get_masked(inter);
							agreement_met =
								(attribs->matching_categories(i_lex.tag())
								 == attribs->matching_categories(i_inter));
							if(agreement_met) {
								break;
							}
						}
					}
					if (agreement_met) {
						return Predicate::True(context);
					}
				}
			}
		}
	}
	// None of possible agreements between endpoints carried over to all tokens
	// that had enough categories specified, or there was no such possible
	// agreement at all, so return False.
	return Predicate::False(context);
}

} /* end ns Wccl */
