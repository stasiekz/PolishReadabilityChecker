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

#ifndef LIBTOKI_SRX_RULE_H
#define LIBTOKI_SRX_RULE_H

#include <unicode/regex.h>

#include <string>

namespace Toki {
namespace Srx {

/// Simple struct to hold a regex matcher and a bool flag
struct CompiledRule
{
	RegexMatcher* matcher;
	bool breaks;
};

/// SImple struct to hold the before and after patterns and the break flag
struct Rule
{
	/// the SRX rule "before" regex pattern
	std::string before;

	/// the SRX rule "after" regex pattern
	std::string after;

	/// the SRX rule "breaks" parameter
	bool breaks;

	std::string create_lookbehind_pattern() const;

	/// combine the patterns into a grouped single pattern
	UnicodeString create_pattern() const;

	/// compile the rule
	CompiledRule compile(UErrorCode& ue) const;
};

} /* end ns Srx */ } /* end ns Toki */

#endif // RULE_H
