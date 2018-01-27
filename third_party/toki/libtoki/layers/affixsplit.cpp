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

#include <libtoki/layers/affixsplit.h>
#include <libtoki/token.h>
#include <libpwrutils/util.h>

namespace Toki {

AffixSplitLayer::AffixSplitLayer(TokenSource* input,
		const Config::Node& props)
	: OutputQueueLayer(input, props)
{
	prefix_type_ = props.get<std::string>("prefix_token_type", "pre");
	suffix_type_ = props.get<std::string>("suffix_token_type", "suf");
	std::string prefixes = props.get("prefix_chars", "");
	PwrNlp::utf8_string_to_uchar_container(prefixes, prefix_chars_);
	std::string suffixes = props.get("suffix_chars", "");
	PwrNlp::utf8_string_to_uchar_container(suffixes, suffix_chars_);
}

std::string AffixSplitLayer::info() const
{
	std::stringstream ss;
	ss << "affix{" << prefix_type_ << "|" << prefix_chars_.size()
		<< "||" << suffix_type_ << "|" << suffix_chars_.size() << "}";
	return ss.str();
}

std::string AffixSplitLayer::long_info() const
{
	std::stringstream ss;
	ss << ", affix: "
		<< prefix_chars_.size() << " prefixes(" << prefix_type_ << ") "
		<< suffix_chars_.size() << " suffixes(" << suffix_type_ << ")";
	return OutputQueueLayer::long_info() + ss.str();
}

void AffixSplitLayer::prepare_more_tokens(Token* t)
{
	int body_begin_index = 0;
	while (body_begin_index < t->orth().length() &&
			is_prefix_char(t->orth().charAt(body_begin_index))) {
		Token* pre = t->clone_changed(
			t->orth().charAt(body_begin_index), prefix_type_);
		t->mark_as_cut();
		enqueue_output_token(pre);
		++body_begin_index;
	}
	int body_end_index = t->orth().length();
	while (body_end_index > body_begin_index &&
			is_suffix_char(t->orth().charAt(body_end_index - 1))) {
		--body_end_index;
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
	while (body_end_index < t->orth().length()) {
		Token* post = t->clone_changed(
			t->orth().charAt(body_end_index), suffix_type_);
		t->mark_as_cut();
		enqueue_output_token(post);
		++body_end_index;
	}
	delete t;
}

bool AffixSplitLayer::is_prefix_char(UChar c)
{
	return prefix_chars_.find(c) != prefix_chars_.end();
}

bool AffixSplitLayer::is_suffix_char(UChar c)
{
	return suffix_chars_.find(c) != suffix_chars_.end();
}

} /* end namespace Toki */
