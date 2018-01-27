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

#include <libwccl/ops/functions/bool/predicates/issingular.h>
#include <libwccl/ops/formatters.h>

namespace Wccl {

FunctionBase::BaseRetValPtr IsSingular::apply_internal(const FunExecContext& context) const {
	const boost::shared_ptr<const TSet>& tag = arg_expr_->apply(context);
	return Predicate::evaluate(tagset_->tag_is_singular(tag->get_value()), context);
}

std::string IsSingular::to_string(const Corpus2::Tagset &tagset) const {
	return UnaryFunctionFormatter::to_string(tagset, *this, *arg_expr_);
}

std::ostream& IsSingular::write_to(std::ostream &os) const {
	return os << this->raw_name() << "(" << *this->arg_expr_ << ")";
}

} /* end ns Wccl */
