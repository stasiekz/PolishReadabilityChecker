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

#include <libwccl/ops/functions/match/submatch.h>
#include <libwccl/ops/functions/vargetter.h>

namespace Wccl {

Submatch::BaseRetValPtr Submatch::apply_internal(
	const FunExecContext &context) const
{
	const RetValPtr& match = match_expr_->apply(context);
	const MatchData& md = match->get_value();
	return RetValPtr(md.submatch(index_ ));
}

std::string Submatch::to_string(const Corpus2::Tagset &tagset) const
{
	std::ostringstream ss;
	boost::shared_ptr<VarGetter<Match> > getvar =
		boost::dynamic_pointer_cast<VarGetter<Match> >(match_expr_);
	if (!getvar) {
		ss << match_expr_->to_string(tagset);
	} else {
		std::string s = match_expr_->to_string(tagset);
		if (s == Match::var_repr("_M")) {
			ss << "M";
		} else {
			ss << s;
		}
	}
	ss << " " << name(tagset) << " " << index_;
	return ss.str();
}

std::ostream& Submatch::write_to(std::ostream& ostream) const
{
	boost::shared_ptr<VarGetter<Match> > getvar =
		boost::dynamic_pointer_cast<VarGetter<Match> >(match_expr_);
	if (!getvar) {
		ostream << *match_expr_;
	} else {
		std::string s = match_expr_->to_raw_string();
		if (s == Match::var_repr("_M")) {
			ostream << "M";
		} else {
			ostream << s;
		}
	}
	ostream << " " << raw_name() << " " << index_;
	return ostream;
}

} /* end ns Wccl */
