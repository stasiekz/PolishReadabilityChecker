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

#include <libtoki/layers/groupaffixsplit.h>
#include <libtoki/token.h>

namespace Toki {

GroupAffixSplitLayer::GroupAffixSplitLayer(TokenSource *input,
		const Config::Node &props)
	: AffixSplitLayer(input, props)
{
}

std::string GroupAffixSplitLayer::info() const
{
	std::stringstream ss;
	ss << "group_affix{" << prefix_type() << "|" << prefix_chars().size()
		<< "||" << suffix_type() << "|" << suffix_chars().size() << "}";
	return ss.str();
}

std::string GroupAffixSplitLayer::long_info() const
{
	return AffixSplitLayer::long_info() + " (group)";
}

void GroupAffixSplitLayer::prepare_more_tokens(Token* t)
{
	int body_begin_index = 0;
	while (body_begin_index < t->orth().length() &&
			is_prefix_char(t->orth().charAt(body_begin_index))) {
		++body_begin_index;
	}
	int body_end_index = t->orth().length();
	while (body_end_index > body_begin_index &&
			is_suffix_char(t->orth().charAt(body_end_index - 1))) {
		--body_end_index;
	}
	if (body_begin_index > 0) {
		UnicodeString pre_orth;
		t->orth().extractBetween(0, body_begin_index, pre_orth);
		Token* pre = t->clone_changed(pre_orth, prefix_type());
		t->mark_as_cut();
		enqueue_output_token(pre);
	}
	if (body_end_index > body_begin_index) {
		if (body_begin_index == 0 &&
				body_end_index == t->orth().length()) {
			enqueue_output_token(t);
			return;
		} else {
			UnicodeString body_orth;
			t->orth().extractBetween(body_begin_index, body_end_index,
					body_orth);
			Token* body = t->clone_changed(body_orth);
			t->mark_as_cut();
			enqueue_output_token(body);
		}
	}
	if (body_end_index < t->orth().length()) {
		UnicodeString suf_orth;
		t->orth().extractBetween(body_end_index, t->orth().length(),
				suf_orth);
		Token* post = t->clone_changed(suf_orth, suffix_type());
		enqueue_output_token(post);
	}
	delete t;
}

} /* end ns Toki */
