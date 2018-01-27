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

#include <libtoki/layers/groupsplit.h>
#include <libtoki/token.h>

namespace Toki {

GroupSplitLayer::GroupSplitLayer(TokenSource* input,
		const Config::Node& props)
	: BasicSplitLayer(input, props)
{
}

std::string GroupSplitLayer::info() const
{
	std::stringstream ss;
	ss << "group_split{" << separator_type()
		<< "|" << separators().size() << "}";
	return ss.str();
}

std::string GroupSplitLayer::long_info() const
{
	return BasicSplitLayer::long_info() + " (group)";
}


void GroupSplitLayer::prepare_more_tokens(Token *t)
{
	const UnicodeString& orth = t->orth();
	int i = 0;

	while (i < orth.length() && !is_split_char(orth.charAt(i))) ++i;
	if (i == orth.length()) {
		enqueue_output_token(t);
	} else {
		if (i > 0) {
			UnicodeString part;
			orth.extractBetween(0, i, part);
			Token* b = t->clone_changed(part);
			t->mark_as_cut();
			enqueue_output_token(b);
		}

		int split_begin = i;
		int split_end = i;
		do {
			++i;
			while (i < orth.length() && is_split_char(orth.charAt(i))) ++i;
			UnicodeString part;
			orth.extractBetween(split_begin, i, part);
			Token* s = t->clone_changed(part, separator_type());
			t->mark_as_cut();
			enqueue_output_token(s);
			split_end = i;
			if (split_end < orth.length()) {
				++i;
				while (i < orth.length()
					&& !is_split_char(orth.charAt(i))) ++i;
				UnicodeString part;
				orth.extractBetween(split_end, i, part);
				Token* b = t->clone_changed(part);
				t->mark_as_cut();
				enqueue_output_token(b);
				split_begin = i;
			}
		} while (i < orth.length());
		delete t;
	}
}

} /* end ns Toki */
