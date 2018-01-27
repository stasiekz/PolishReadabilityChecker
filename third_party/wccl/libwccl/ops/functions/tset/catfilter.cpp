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

#include <libwccl/ops/functions/tset/catfilter.h>
#include <libwccl/ops/functions/constant.h>

namespace Wccl {

std::string CatFilter::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << name(tagset) << "("
		<< pos_expr_->to_string(tagset) << ", "
		<< selector_expr_-> to_string(tagset) << ", "
		<< mask_expr_-> to_string(tagset) << ")";
	return os.str();
}

std::ostream& CatFilter::write_to(std::ostream& os) const
{
	return os << raw_name() << "("
			<< *pos_expr_ << ", "
			<< *selector_expr_ << ", "
			<< *mask_expr_ << ")";
}

CatFilter::BaseRetValPtr CatFilter::apply_internal(const FunExecContext& context) const
{
	const boost::shared_ptr<const Position>& pos = pos_expr_->apply(context);
	const SentenceContext& sc = context.sentence_context();
	if (sc.is_outside(*pos)) {
		return detail::DefaultFunction<TSet>()->apply(context);
	}

	const boost::shared_ptr<const TSet>& selector_tset = selector_expr_->apply(context);
	const boost::shared_ptr<const TSet>& mask_tset = mask_expr_->apply(context);
	const Corpus2::Tag& selector = selector_tset->get_value();
	const Corpus2::Tag& mask = mask_tset->get_value();

	boost::shared_ptr<TSet> tset = boost::make_shared<TSet>();
	const Corpus2::Token* token = sc.at(*pos);
	BOOST_FOREACH (const Corpus2::Lexeme& lexeme, token->lexemes()) {
		if (!lexeme.tag().get_masked(selector).is_null()) {
			tset->combine_with(lexeme.tag());
		}
	}
	tset->contents().mask_with(mask);
	return tset;
}

} /* end ns Wccl */
