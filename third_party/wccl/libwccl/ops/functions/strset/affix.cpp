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

#include <libwccl/ops/functions/strset/affix.h>
#include <boost/foreach.hpp>

namespace Wccl {

std::string Affix::to_string(const Corpus2::Tagset& tagset) const
{
	std::stringstream str;
	str << name(tagset) << "(" << strset_expr_->to_string(tagset)
		<< ", " << affix_length_ << ")";
	return str.str();
}

std::ostream& Affix::write_to(std::ostream& os) const
{
	return os << raw_name() << "(" << *strset_expr_ << ", " << affix_length_ << ")";
}

Affix::BaseRetValPtr Affix::apply_internal(const FunExecContext& context) const
{
	if(affix_length_ == 0) {
		return strset_expr_->apply(context);
	}
	const boost::shared_ptr<const StrSet>& set = strset_expr_->apply(context);
	boost::shared_ptr<StrSet> a_set = boost::shared_ptr<StrSet>(new StrSet());
	if(affix_length_ < 0) {
		BOOST_FOREACH (const UnicodeString& s, set->contents()) {
			a_set->insert(UnicodeString(s).remove(0, s.length() + affix_length_));
		}
	} else {
		BOOST_FOREACH (const UnicodeString& s, set->contents()) {
			UnicodeString prefixed(s);
			prefixed.truncate(affix_length_);
			a_set->insert(prefixed);
		}
	}
	return a_set;
}

} /* end ns Wccl */
