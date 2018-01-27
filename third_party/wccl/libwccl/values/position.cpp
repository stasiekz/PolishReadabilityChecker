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

#include <libwccl/values/position.h>
#include <libwccl/sentencecontext.h>
#include <boost/lexical_cast.hpp>

namespace Wccl {

const char* Position::type_name = "Position";

std::string Position::to_raw_string() const
{
	switch (val_) {
	case Nowhere:
		return "nowhere";
	case Begin:
		return "begin";
	case End:
		return "end";
	default:
		return boost::lexical_cast<std::string>(val_);
	}
}

std::string Position::var_repr(const std::string &var_name)
{
	std::stringstream ss;
	ss << "$" << var_name;
	return ss.str();
}

bool Position::equals(const Position& other, const SentenceContext& context) const
{
	return context.get_abs_position(*this) == context.get_abs_position(other);
}

} /* end ns Wccl */
