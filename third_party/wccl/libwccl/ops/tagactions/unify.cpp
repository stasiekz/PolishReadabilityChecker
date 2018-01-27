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

#include <libwccl/ops/tagactions/unify.h>
#include <boost/foreach.hpp>
#include <sstream>

namespace Wccl {

Bool Unify::execute(const ActionExecContext& context) const
{
	Bool changed(false);
	SentenceContext& sc = context.sentence_context();

	const boost::shared_ptr<const Position>& range_left = pos_begin_->apply(context);
	if (range_left->get_value() == Position::Nowhere) {
		return Bool(false);
	}
	const boost::shared_ptr<const Position>& range_right = pos_end_->apply(context);
	if (range_right->get_value() == Position::Nowhere) {
		return Bool(false);
	}
	int abs_left, abs_right;
	if (!sc.validate_range(*range_left, *range_right, abs_left, abs_right)) {
		return Bool(false);
	}

	const boost::shared_ptr<const TSet>& attribs = attribs_expr_->apply(context);

	std::vector<std::vector<Corpus2::Lexeme> > remainings;
	std::vector<std::vector<Corpus2::Lexeme> > underspecifieds;

	// first pass - cut lexemes that don't match agreement attribs at all
	for (int i = abs_left; i <= abs_right; ++i) {
		Corpus2::Token& curr_tok = *sc.at(i);
		std::vector<Corpus2::Lexeme> remaining;
		std::vector<Corpus2::Lexeme> underspecified;
		BOOST_FOREACH (Corpus2::Lexeme& lexeme, curr_tok.lexemes()) {
			if (attribs->matching_categories(lexeme.tag()) > 0) {
				remaining.push_back(lexeme);
			} else {
				underspecified.push_back(lexeme);
			}
		}
		remainings.push_back(remaining);
		underspecifieds.push_back(underspecified);
	}
	// second pass - leave only lexemes that are in agreement
	// with at least one lexeme from previous non-empty-after-1st-pass token
	int previous = remainings.size() - 1;
	// at first, "previous" is the last token not emptied after 1st pass
	while ((previous > 0) && (remainings[previous].empty())) {
		previous--;
	}
	for (size_t i = 0; i < remainings.size(); ++i) {
		// proceed only for tokens that have lexemes that could go into agreement
		// (i.e. are not emptied after 1st pass)
		if (!remainings[i].empty()) {
			const std::vector<Corpus2::Lexeme>& prev_rem = remainings[previous];
			std::vector<Corpus2::Lexeme> curr_rem;
			// get only lexemes that agree with a lexeme from previous token
			BOOST_FOREACH (const Corpus2::Lexeme& curr_lex, remainings[i]) {
				int curr_match_cats = attribs->matching_categories(curr_lex.tag());
				BOOST_FOREACH (const Corpus2::Lexeme& prev_lex, prev_rem) {
					Corpus2::Tag inter = curr_lex.tag().get_masked(prev_lex.tag());
					int min_to_match = std::min(
								curr_match_cats,
								attribs->matching_categories(prev_lex.tag()));
					if (attribs->matching_categories(inter) == min_to_match) {
						curr_rem.push_back(curr_lex);
						break;
					}
				}
			}
			if (curr_rem.empty()) {
				if (underspecifieds[i].empty()) {
					// there is no agreement possible, abort with no changes
					changed.set_value(false);
					return changed;
				}
			} else {
				// some non-underspecified lexemes are left, so
				// next time around, "previous" token is going to be the current one
				previous = i;
			}
			// there were changes only if we actually removed something
			if (curr_rem.size() != remainings[i].size()) {
				changed.set_value(true);
				remainings[i] = curr_rem;
			}
		}
	}
	// finally assign remaining lexemes to original tokens
	if (changed.get_value()) {
		for (size_t i = 0; i < remainings.size(); ++i) {
			std::vector<Corpus2::Lexeme>& lexemes = sc.at(i + abs_left)->lexemes();
			lexemes = remainings[i];
			// underspecced lexemes meet agreement too, so leave them in the end, if any
			BOOST_FOREACH (const Corpus2::Lexeme& lex, underspecifieds[i]) {
				lexemes.push_back(lex);
			}
		}
	}
	return changed;
}

std::string Unify::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << name() << "(" << pos_begin_->to_string(tagset) << ", "
			<< pos_end_->to_string(tagset) << ", "
			<< attribs_expr_->to_string(tagset) << ")";
	return os.str();
}

std::ostream& Unify::write_to(std::ostream& os) const
{
	os << name() << "(" << *pos_begin_ << ", " << *pos_end_ << ", " << *attribs_expr_ << ")";
	return os;
}


} /* end ns Wccl */
