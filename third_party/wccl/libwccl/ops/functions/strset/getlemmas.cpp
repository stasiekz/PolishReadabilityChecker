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

#include <libwccl/ops/functions/strset/getlemmas.h>
#include <libwccl/ops/formatters.h>
#include <libwccl/ops/functions/constant.h>

namespace Wccl {

std::string GetLemmas::to_string(const Corpus2::Tagset& tagset) const
{
	return UnaryFunctionFormatter::to_string(tagset, *this, *pos_expr_, "[", "]");
}

std::ostream& GetLemmas::write_to(std::ostream& os) const
{
	return os << raw_name() << "[" << *pos_expr_ << "]";
}

GetLemmas::BaseRetValPtr GetLemmas::apply_internal(const FunExecContext& context) const
{
	const boost::shared_ptr<const Position>& pos = pos_expr_->apply(context);
	const SentenceContext& sc = context.sentence_context();
	if (sc.is_outside(*pos)) {
		return detail::DefaultFunction<StrSet>()->apply(context);
	}
	boost::shared_ptr<StrSet> u_set = boost::make_shared<StrSet>();
	BOOST_FOREACH (const Corpus2::Lexeme& lexeme, sc.at(*pos)->lexemes()) {
		u_set->insert(lexeme.lemma());
	}
	return u_set;
}

} /* end ns Wccl */
