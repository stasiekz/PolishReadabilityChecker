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

#include <libwccl/ops/functions/strset/propval.h>
#include <boost/foreach.hpp>
#include <libwccl/ops/functions/constant.h>

namespace Wccl {

std::string PropVal::to_string(const Corpus2::Tagset& tagset) const
{
	std::stringstream str;
	str << name(tagset) << "("
		<< *pos_expr_ << ", "
		<< strset_expr_->to_string(tagset)
		<< ")";
	return str.str();
}

std::ostream& PropVal::write_to(std::ostream& os) const
{
	return os
			<< raw_name()
			<< "(" << *pos_expr_ << ", " << *strset_expr_ << ")";
}

PropVal::BaseRetValPtr PropVal::apply_internal(const FunExecContext& context) const
{
	// check if position inside sentence boundaries
	const boost::shared_ptr<const Position>& pos = pos_expr_->apply(context);
	const SentenceContext& sc = context.sentence_context();
	if (sc.is_outside(*pos)) {
		// outside, return an empty set
		return detail::DefaultFunction<StrSet>()->apply(context);
	}
	// inside
	// check if any metadata there
	boost::shared_ptr<Corpus2::TokenMetaData> md = sc.at(*pos)->get_metadata();
	if (!md) {
		// no metadata assigned, return an empty set
		return detail::DefaultFunction<StrSet>()->apply(context);
	}
	// at least there is data structure for storing metadata
	// create empty output StrSet and get the wanted keys
	boost::shared_ptr<StrSet> ret_set =
			boost::shared_ptr<StrSet>(new StrSet());
	const boost::shared_ptr<const StrSet>& keyset =
			strset_expr_->apply(context);
	// gather values that are assigned to keys present
	BOOST_FOREACH (const UnicodeString& u_key, keyset->contents()) {
		const std::string str_key(PwrNlp::to_utf8(u_key));
		if (md->has_attribute(str_key)) {
			ret_set->insert_utf8(md->get_attribute(str_key));
		}
	}
	return ret_set;
}

} /* end ns Wccl */
