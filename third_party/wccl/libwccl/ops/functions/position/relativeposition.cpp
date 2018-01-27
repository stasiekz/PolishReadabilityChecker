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

#include <libwccl/ops/functions/position/relativeposition.h>
#include <libwccl/ops/functions/constant.h>

namespace Wccl {

RelativePosition::BaseRetValPtr RelativePosition::apply_internal(
	const FunExecContext &context) const
{
	const RetValPtr& orig_pos = pos_expr_->apply(context);
	if(orig_pos->get_value() == Position::Nowhere) {
		return detail::DefaultFunction<Position>()->apply(context);
	}
	const SentenceContext& sc = context.sentence_context();
	return RetValPtr(new Position(offset_ + sc.get_rel_position(*orig_pos)));
}

std::string RelativePosition::to_string(const Corpus2::Tagset &tagset) const
{
	std::ostringstream ss;
	ss << pos_expr_->to_string(tagset);
	if(offset_ >= 0) {
		ss << " + " << offset_;
	} else {
		ss << " - " << -offset_;
	}
	return ss.str();
}

std::ostream& RelativePosition::write_to(std::ostream& ostream) const
{
	ostream << *pos_expr_;
	if(offset_ >= 0) {
		ostream << " + " << offset_;
	} else {
		ostream << " - " << -offset_;
	}
	return ostream;
}

} /* end ns Wccl */
