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

#include <libwccl/ops/match/applyoperator.h>
#include <libwccl/values/match.h>
#include <libwccl/ops/match/conditions/conjconditions.h>
#include <libwccl/ops/match/matchaction.h>
#include <boost/foreach.hpp>

namespace Wccl {
namespace Matching {

ApplyOperator::ApplyOperator(
	const VariableAccessor<Match>& matches,
	const boost::shared_ptr<const ConjConditions>& match_conditions,
	const boost::shared_ptr<const std::vector<boost::shared_ptr<MatchAction> > >& actions,
	const boost::shared_ptr<const std::vector<boost::shared_ptr<Function<Bool> > > >& apply_conditions)
		: matches_(matches),
		  match_conditions_(match_conditions),
		  actions_(actions),
		  apply_conditions_(apply_conditions)
{
	BOOST_ASSERT(match_conditions_);
	BOOST_ASSERT(actions_->size() > 0);
}

void ApplyOperator::execute(const ActionExecContext &context) const
{
	MatchVector* matches =
		dynamic_cast<MatchVector*>(
			&context.variables()->get_fast(matches_)->get_value());
	BOOST_ASSERT(matches);
	context.sentence_context().goto_start();
	while(context.sentence_context().is_current_inside()) {
		int orig_pos = context.sentence_context().get_position();
		// Set initial value for $m:_M variable for this iteration and launch the match:
		matches->clear();
		MatchResult match = match_conditions_->apply(context);
		// Execute the actions only if there was a match and all post-conditions are met:
		if (!match.matched()) {
			// no match - advance current position by 1.
			context.sentence_context().set_position(orig_pos + 1);
		} else {
			// there was a match - check conditions of apply
			matches->append(match.get_match()); // the match goes to $m:_M[0]
			bool should_act = true;
			for(size_t i = 0; should_act && i < apply_conditions_->size(); ++i) {
				should_act = (*apply_conditions_)[i]->apply(context)->get_value();
			}
			if (should_act) {
				BOOST_FOREACH (const boost::shared_ptr<MatchAction>& action, *actions_) {
					action->execute(context);
				}
			}
		}
		// Inner operators are responsible for properly advancing current sentence position.
		BOOST_ASSERT(context.sentence_context().get_position() > orig_pos);
	}
}

std::string ApplyOperator::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream ostream;
	ostream << name() << "(match" << match_conditions_->to_string(tagset) << ", ";
	if (!apply_conditions_->empty()) {
		ostream << "cond(";
		for(size_t i = 0; i < apply_conditions_->size(); ++i) {
			if (i != 0) {
				ostream << ", ";
			}
			ostream << (*apply_conditions_)[i]->to_string(tagset);
		}
		ostream << "), ";
	}
	ostream << "actions(";
	for(size_t i = 0; i < actions_->size(); ++i) {
		if (i != 0) {
			ostream << ", ";
		}
		ostream << (*actions_)[i]->to_string(tagset);
	}
	ostream << "))";
	return ostream.str();
}

std::ostream& ApplyOperator::write_to(std::ostream &ostream) const
{
	ostream << name() << "(match" << *match_conditions_ << ", ";
	if (!apply_conditions_->empty()) {
		ostream << "cond(";
		for(size_t i = 0; i < apply_conditions_->size(); ++i) {
			if (i != 0) {
				ostream << ", ";
			}
			ostream << *(*apply_conditions_)[i];
		}
		ostream << "), ";
	}
	ostream << "actions(";
	for(size_t i = 0; i < (*actions_).size(); ++i) {
		if (i != 0) {
			ostream << ", ";
		}
		ostream << *(*actions_)[i];
	}
	ostream << "))";
	return ostream;
}

} /* end ns Matching */
} /* end ns Wccl */
