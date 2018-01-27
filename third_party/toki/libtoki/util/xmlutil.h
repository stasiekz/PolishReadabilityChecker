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

#ifndef LIBTOKI_XMLUTIL_H
#define LIBTOKI_XMLUTIL_H

#include <libxml++/nodes/node.h>

namespace Toki {
namespace XmlUtil {

const xmlpp::Node* get_child_or_null(const xmlpp::Node* n, const char* name);

template<typename TE> inline
const xmlpp::Node* get_child_or_throw(const xmlpp::Node* n, const char* name)
{
	const xmlpp::Node* c = get_child_or_null(n, name);
	if (!c) {
		std::stringstream ss;
		ss << "no " << "<" << name << ">";
		throw TE(ss.str());
	}
	return c;
}

std::string get_child_text_or_empty(const xmlpp::Node* n, const char* name);

} /* end ns XmlUtil */
} /* end ns Toki */

#endif // XMLUTIL_H
