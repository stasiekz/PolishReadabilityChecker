/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <libtoki/srx/rule.h>

#include <unicode/regex.h>

#include <sstream>

namespace Toki {
namespace Srx {

std::string Rule::create_lookbehind_pattern() const {
	std::stringstream ss;
	return ss.str();
}

UnicodeString Rule::create_pattern() const
{
	std::stringstream ss;
	ss << "(" << before << ")(" << after << ")";
	return UnicodeString::fromUTF8(ss.str());
}

CompiledRule Rule::compile(UErrorCode& ue) const
{
	CompiledRule cr;
	cr.matcher = new RegexMatcher(create_pattern(), 0, ue);
	cr.breaks = breaks;
	return cr;
}

} /* end ns Srx */
} /* end ns Toki */
