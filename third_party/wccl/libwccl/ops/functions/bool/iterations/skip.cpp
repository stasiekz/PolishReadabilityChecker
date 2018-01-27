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

#include <libwccl/ops/functions/bool/iterations/skip.h>
#include <libwccl/ops/functions/bool/predicate.h>

namespace Wccl {

std::string Skip::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream ss;
	ss << name(tagset) << "("
		<< central_pos_expr_->to_string(tagset) << ", "
		<< Position::var_repr(iter_var_acc_.get_name()) << ", "
		<< evaluating_expr_->to_string(tagset) << ", "
		<< offset_ << ")";
	return ss.str();
}

std::ostream& Skip::write_to(std::ostream& os) const
{
	return os << raw_name() << "("
			<< *central_pos_expr_ << ", "
			<< Position::var_repr(iter_var_acc_.get_name()) << ", "
			<< *evaluating_expr_ << ","
			<< offset_ << ")";
}

Skip::BaseRetValPtr Skip::apply_internal(const FunExecContext& context) const
{
	const boost::shared_ptr<Position>& iter_var =
		context.variables()->get_fast(iter_var_acc_);
	const boost::shared_ptr<const Position>& central_pos =
										central_pos_expr_->apply(context);

	if (context.sentence_context().is_inside(*central_pos)) {
		// set iter_var to central_pos and iterate from here
		iter_var->set_value(central_pos->get_value());
		int offset_now = offset_;

		while (offset_now != 0) {
			// go one step towards the right direction
			int direction = (offset_now > 0) ? +1 : -1;
			iter_var->set_value(iter_var->get_value() + direction);
			// if it's already out of sentence, fail
			if (context.sentence_context().is_outside(*iter_var)) {
				iter_var->set_value(Position::Nowhere);
				return Predicate::False(context);
			}
			// if the skip-pred not satisfied here, decrement the steps to go
			if(!evaluating_expr_->apply(context)->get_value()) {
				offset_now -= direction;
			}
		}
		// offset reached, the iteration is complete
		return Predicate::True(context);

	}
	// Failure. Set iteration variable to Nowhere and return False.
	iter_var->set_value(Position::Nowhere);
	return Predicate::False(context);
}

} /* end ns Wccl */
