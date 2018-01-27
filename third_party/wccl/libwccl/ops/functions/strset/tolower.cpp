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

#include <libwccl/ops/functions/strset/tolower.h>
#include <libwccl/ops/formatters.h>

namespace Wccl {

std::string ToLower::to_string(const Corpus2::Tagset& tagset) const
{
	return UnaryFunctionFormatter::to_string(tagset, *this, *strset_expr_);
}

std::ostream& ToLower::write_to(std::ostream& os) const
{
	return os << raw_name() << "(" << *strset_expr_ << ")";
}

ToLower::BaseRetValPtr ToLower::apply_internal(const FunExecContext& context) const
{	
	const boost::shared_ptr<const StrSet >& set = strset_expr_->apply(context);
	boost::shared_ptr<StrSet > l_set = boost::make_shared<StrSet>();
	//TODO: should tolower be a method of StrSet as well?
	BOOST_FOREACH (const UnicodeString& s, set->contents()) {
		//TODO: what about locale? is default ok? should the context hold it?
		l_set->insert(UnicodeString(s).toLower());
	}
	return l_set;
}

} /* end ns Wccl */
