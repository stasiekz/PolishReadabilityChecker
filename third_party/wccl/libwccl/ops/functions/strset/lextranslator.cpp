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

#include <libwccl/ops/functions/strset/lextranslator.h>
#include <sstream>

namespace Wccl {

std::string LexTranslator::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << name(tagset) << "(" 
		<< strset_expr_->to_string(tagset) << ", \""
		<< lexicon_->name() << "\")";
	return os.str();
}

std::ostream& LexTranslator::write_to(std::ostream& os) const
{
	os << raw_name() << "(" << *strset_expr_ << ", \"" << lexicon_->name() << "\")";
	return os;
}

LexTranslator::BaseRetValPtr LexTranslator::apply_internal(
	const FunExecContext& context) const
{
	boost::shared_ptr<const StrSet> set = strset_expr_->apply(context);
	return lexicon_->translate(*set);
}

} /* end ns Wccl */
