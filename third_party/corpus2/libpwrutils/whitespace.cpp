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

#include <libpwrutils/whitespace.h>

namespace PwrNlp {

std::string Whitespace::to_string(Whitespace::Enum wa)
{
	switch (wa) {
	case Whitespace::None:
		return "none";
	case Whitespace::Space:
		return "space";
	case Whitespace::ManySpaces:
		return "spaces";
	case Whitespace::Newline:
		return "newline";
	case Whitespace::ManyNewlines:
		return "newlines";
	default:
		return "???";
	}
}

const char* Whitespace::to_whitespace(Whitespace::Enum wa)
{
	switch (wa) {
	case Whitespace::None:
		return "";
	case Whitespace::Space:
		return " ";
	case Whitespace::ManySpaces:
		return "  ";
	case Whitespace::Newline:
		return "\n";
	case Whitespace::ManyNewlines:
		return "\n\n";
	default:
		return "???";
	}
}

Whitespace::Enum Whitespace::from_string(const std::string &s)
{
	int w = Whitespace::None;
	while (w < Whitespace::PostLast
			&& Whitespace::to_string((Whitespace::Enum)w) != s) ++w;
	return (Whitespace::Enum)w;
}

bool Whitespace::is_valid(Whitespace::Enum w) {
	return w >= 0 && w < Whitespace::PostLast;
}

} /* end namespace PwrNlp */
