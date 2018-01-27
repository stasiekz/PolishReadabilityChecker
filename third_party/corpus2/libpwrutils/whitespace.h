/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENCE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef PWRNLP_WHITESPACE_H
#define PWRNLP_WHITESPACE_H

#include <string>

namespace PwrNlp {

/**
 * A whitespace amount enumeration, namespaced.
 */
namespace Whitespace
{
	enum Enum {
		None,
		Space,
		ManySpaces,
		Newline,
		ManyNewlines,
		PostLast // last item guard
	};

	std::string to_string(Enum wa);

	const char* to_whitespace(Enum wa);

	Enum from_string(const std::string& s);

	bool is_valid(Enum w);
}

}  /* end ns PwrNlp */

#endif // PWRNLP_WHITESPACE_H
