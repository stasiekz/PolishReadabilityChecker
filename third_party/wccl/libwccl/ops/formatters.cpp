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

#include <libwccl/ops/formatters.h>
#include <sstream>

namespace Wccl {

// ------ UnaryFunctionFormatter -----

std::string UnaryFunctionFormatter::to_string(
	const Corpus2::Tagset& tagset,
	const FunctionBase& f,
	const std::string& arg_str,
	const char* open_bracket,
	const char* close_bracket)
{
	std::stringstream ss;
	ss << f.name(tagset) << open_bracket << arg_str << close_bracket;
	return ss.str();
}

// ----- BinaryFunctionFormatter ------

std::string BinaryFunctionFormatter::to_string(
	const Corpus2::Tagset& tagset,
	const FunctionBase& f,
	const std::string& arg1_str,
	const std::string& arg2_str)
{
	std::stringstream ss;
	ss << f.name(tagset) << "(" << arg1_str << ", " << arg2_str << ")";
	return ss.str();
}

} /* end ns Wccl */
