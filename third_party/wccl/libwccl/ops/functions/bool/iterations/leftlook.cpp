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

#include <libwccl/ops/functions/bool/iterations/leftlook.h>

namespace Wccl {

bool LeftLook::iterate(
	int left,
	int right,
	Wccl::Position &p,
	const Wccl::FunExecContext &context) const
{
	for(int i = right; i >= left; --i) {
		p.set_value(i);
		if(evaluating_expr_->apply(context)->get_value()) {
			return true;
		}
	}
	return false;
}

std::string LeftLook::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream ss;
	ss << name(tagset) << "("
		<< right_pos_expr_->to_string(tagset) << ", "
		<< left_pos_expr_->to_string(tagset) << ", "
		<< Position::var_repr(iter_var_acc_.get_name()) << ", "
		<< evaluating_expr_->to_string(tagset) << ")";
	return ss.str();
}

std::ostream& LeftLook::write_to(std::ostream& os) const
{
	return os << raw_name() << "("
			<< *right_pos_expr_ << ", "
			<< *left_pos_expr_ << ", "
			<< Position::var_repr(iter_var_acc_.get_name()) << ", "
			<< *evaluating_expr_ << ")";
}

} /* end ns Wccl */
