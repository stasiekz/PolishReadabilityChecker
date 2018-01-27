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

#include <libwccl/ops/functions/tset/getsymbols.h>
#include <libwccl/ops/formatters.h>
#include <libwccl/ops/functions/constant.h>

namespace Wccl {

std::string GetSymbols::to_string(const Corpus2::Tagset& tagset) const
{
	return UnaryFunctionFormatter::to_string(tagset, *this, *pos_expr_, "[", "]");
}

std::ostream& GetSymbols::write_to(std::ostream& os) const
{
	return os << raw_name() << "[" << *pos_expr_ << "]";
}

std::string GetSymbols::name(const Corpus2::Tagset &tagset) const
{
	return tagset.get_attribute_name(mask_.get_values());
}

std::string GetSymbols::raw_name() const
{
	return mask_.raw_dump();
}

GetSymbols::BaseRetValPtr GetSymbols::apply_internal(const FunExecContext& context) const
{
	const boost::shared_ptr<const Position>& pos = pos_expr_->apply(context);
	const SentenceContext& sc = context.sentence_context();
	if(sc.is_outside(*pos)) {
		return detail::DefaultFunction<TSet>()->apply(context);
	}

	boost::shared_ptr<TSet> tset = boost::make_shared<TSet>();
	const Corpus2::Token* token = sc.at(*pos);
	BOOST_FOREACH (const Corpus2::Lexeme& lexeme, token->lexemes()) {
		tset->combine_with(lexeme.tag());
	}
	tset->contents().mask_with(mask_);
	return tset;
}

} /* end ns Wccl */
