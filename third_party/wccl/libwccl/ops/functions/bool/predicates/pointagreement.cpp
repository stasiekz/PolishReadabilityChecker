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

#include <libwccl/ops/functions/bool/predicates/pointagreement.h>
#include <boost/foreach.hpp>

namespace Wccl {

std::string PointAgreement::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream ss;
	ss << name(tagset) << "("
		<< pos1_expr_->to_string(tagset) << ", "
		<< pos2_expr_->to_string(tagset) << ", "
		<< attribs_expr_->to_string(tagset) << ")";
	return ss.str();
}

std::ostream& PointAgreement::write_to(std::ostream& os) const
{
	return os << raw_name() << "("
			<< *pos1_expr_ << ", "
			<< *pos2_expr_ << ", "
			<< *attribs_expr_ << ")";
}

PointAgreement::BaseRetValPtr PointAgreement::apply_internal(const FunExecContext& context) const
{
	const SentenceContext& sc = context.sentence_context();

	const boost::shared_ptr<const Position>& pos1 = pos1_expr_->apply(context);
	if (sc.is_outside(*pos1)) {
		return Predicate::False(context);
	}
	const boost::shared_ptr<const Position>& pos2 = pos2_expr_->apply(context);
	if (sc.is_outside(*pos2)) {
		return Predicate::False(context);
	}

	const boost::shared_ptr<const TSet>& attribs = attribs_expr_->apply(context);

	int min_card = attribs->categories_count(*tagset_);

	const Corpus2::Token* t1 = sc.at(*pos1);
	const Corpus2::Token* t2 = sc.at(*pos2);
	// to optimize a bit, make sure t1 is the one with less lexemes
	if (t1->lexemes().size() > t2->lexemes().size()) {
		std::swap(t1, t2);
	}

	BOOST_FOREACH (const Corpus2::Lexeme& t1_lex, t1->lexemes()) {
		const Corpus2::Tag& t1_tag = t1_lex.tag();
		// don't bother checking t2 unless current t1_tag matches enough categories
		if (attribs->matching_categories(t1_tag) >= min_card) {
			BOOST_FOREACH (const Corpus2::Lexeme& t2_lex, t2->lexemes()) {
				Corpus2::Tag intersection = t1_tag.get_masked(t2_lex.tag());
				// if the intersection matches enough categories we have agreement
				if (attribs->matching_categories(intersection) >= min_card) {
					return Predicate::True(context);
				}
			}
		}
	}

	return Predicate::False(context);
}

} /* end ns Wccl */
