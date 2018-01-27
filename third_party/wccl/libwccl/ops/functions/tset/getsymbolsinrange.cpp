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

#include <libwccl/ops/functions/tset/getsymbolsinrange.h>
#include <libwccl/ops/functions/constant.h>

#include <sstream>

namespace Wccl {

std::string GetSymbolsInRange::to_string(const Corpus2::Tagset& tagset) const
{
	std::stringstream ss;
	ss << name(tagset) << "("
		<< tagset.get_attribute_name(mask_.get_values()) << ", "
		<< rbegin_expr_->to_string(tagset) << ", "
		<< rend_expr_->to_string(tagset) << ")";
	return ss.str();
}

std::ostream& GetSymbolsInRange::write_to(std::ostream& os) const
{
	return os << raw_name() << "("
			  << mask_.raw_dump() << ", "
			  << *rbegin_expr_ << ", "
			  << *rend_expr_ << ")";
}

GetSymbolsInRange::BaseRetValPtr GetSymbolsInRange::apply_internal(const FunExecContext& context) const
{
	const boost::shared_ptr<const Position>& range_begin = rbegin_expr_->apply(context);
	const SentenceContext& sc = context.sentence_context();

	if (range_begin->get_value() == Position::Nowhere ) {
		return detail::DefaultFunction<TSet>()->apply(context);
	}
	const boost::shared_ptr<const Position>& range_end = rend_expr_->apply(context);
	int abs_begin, abs_end;
	if (!sc.validate_range(*range_begin, *range_end, abs_begin, abs_end)) {
		return detail::DefaultFunction<TSet>()->apply(context);
	}

	boost::shared_ptr<TSet> tset = boost::make_shared<TSet>();
	for(int abs_pos = abs_begin; abs_pos <= abs_end; abs_pos++) {
		const Corpus2::Token* token = sc.at(abs_pos);
		BOOST_FOREACH (const Corpus2::Lexeme& lexeme, token->lexemes()) {
			tset->combine_with(lexeme.tag());
		}
	}
	tset->contents().mask_with(mask_);
	return tset;
}

} /* end ns Wccl */
