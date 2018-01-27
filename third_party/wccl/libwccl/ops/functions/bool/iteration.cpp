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

#include <libwccl/ops/functions/bool/iteration.h>
#include <libwccl/ops/functions/bool/predicate.h>

namespace Wccl {

std::string Iteration::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream ss;
	ss << name(tagset) << "("
		<< left_pos_expr_->to_string(tagset) << ", "
		<< right_pos_expr_->to_string(tagset) << ", "
		<< Position::var_repr(iter_var_acc_.get_name()) << ", "
		<< evaluating_expr_->to_string(tagset) << ")";
	return ss.str();
}

std::ostream& Iteration::write_to(std::ostream& os) const
{
	return os << raw_name() << "("
			<< *left_pos_expr_ << ", "
			<< *right_pos_expr_ << ", "
			<< Position::var_repr(iter_var_acc_.get_name()) << ", "
			<< *evaluating_expr_ << ")";
}

Iteration::BaseRetValPtr Iteration::apply_internal(const FunExecContext& context) const
{
	const boost::shared_ptr<Position>& iter_var =
		context.variables()->get_fast(iter_var_acc_);
	const SentenceContext& sc = context.sentence_context();
	// Proceed only if range extremes are not "nowhere".
	const boost::shared_ptr<const Position>& range_left =
										left_pos_expr_->apply(context);
	if (range_left->get_value() != Position::Nowhere) {
		const boost::shared_ptr<const Position>& range_right =
										right_pos_expr_->apply(context);
		int left, right;
		if (sc.validate_range(*range_left, *range_right,
							  left, right, may_cross)) {
			// Change range from absolute to relative and iterate
			left -= sc.get_position();
			right -= sc.get_position();
			if (iterate(left, right, *iter_var, context)) {
				return Predicate::True(context);
			}
		}
	}
	// Failure. Set iteration variable to Nowhere and return False
	iter_var->set_value(Position::Nowhere);
	return Predicate::False(context);
}

} /* end ns Wccl */
