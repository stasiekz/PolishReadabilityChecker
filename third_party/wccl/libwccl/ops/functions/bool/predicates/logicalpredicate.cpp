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

#include <libwccl/ops/functions/bool/predicates/logicalpredicate.h>

namespace Wccl {

std::string LogicalPredicate::to_string(const Corpus2::Tagset& tagset) const
{
	std::stringstream ss;
	ss << name(tagset) << "(";
	BoolFunctionPtrVector::const_iterator it = expressions_->begin();
	while(it != expressions_->end()) {
		ss << (*it)->to_string(tagset);
		if(++it != expressions_->end()) {
			ss << ", ";
		}
	}
	ss << ")";
	return ss.str();
}

std::ostream& LogicalPredicate::write_to(std::ostream& ostream) const
{
	ostream << raw_name() << "(";
	BoolFunctionPtrVector::const_iterator it = expressions_->begin();
	while(it != expressions_->end()) {
		ostream << (*it)->to_raw_string();
		if(++it != expressions_->end()) {
			ostream << ", ";
		}
	}
	ostream << ")";
	return ostream;
}

} /* end ns Wccl */
