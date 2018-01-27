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

#include <libtoki/util/xmlutil.h>

#include <libxml++/nodes/element.h>
#include <libxml++/nodes/textnode.h>

namespace Toki {
namespace XmlUtil {

const xmlpp::Node* get_child_or_null(const xmlpp::Node* n, const char* name)
{
	const xmlpp::Node::NodeList list = n->get_children(name);
	if (list.empty()) return NULL;
	return list.front();
}

std::string get_child_text_or_empty(const xmlpp::Node* n, const char* name)
{
	const xmlpp::Element* el = dynamic_cast<const xmlpp::Element*>(
		get_child_or_null(n, name));
	if (el) {
		const xmlpp::TextNode* t = el->get_child_text();
		if (t) return t->get_content();
	}
	return "";
}

} /* end ns XmlUtil */
} /* end ns Toki */
