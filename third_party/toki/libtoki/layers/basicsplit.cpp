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

#include <libtoki/layers/basicsplit.h>
#include <libtoki/token.h>
#include <libpwrutils/util.h>

namespace Toki {

BasicSplitLayer::BasicSplitLayer(TokenSource* input,
		const Config::Node& props)
	: OutputQueueLayer(input, props), split_chars_(), sep_type_()
{
	sep_type_ = props.get<std::string>("separator_token_type", "sep");
	std::string separators = props.get("separators", "");
	PwrNlp::utf8_string_to_uchar_container(separators, split_chars_);
}

std::string BasicSplitLayer::info() const
{
	std::stringstream ss;
	ss << "split{" << sep_type_ << "|" << split_chars_.size() << "}";
	return ss.str();
}

std::string BasicSplitLayer::long_info() const
{
	std::stringstream ss;
	ss << ", split: " << split_chars_.size() << " separators, type "
		<< sep_type_;
	return OutputQueueLayer::long_info() + ss.str();
}

void BasicSplitLayer::prepare_more_tokens(Token* t)
{
	const UnicodeString& orth = t->orth();
	int splitfrom = 0;
	for (int i = 0; i < orth.length(); ++i) {
		if (is_split_char(orth.charAt(i))) {
			if (i > splitfrom) {
				UnicodeString part;
				orth.extractBetween(splitfrom, i, part);
				Token* pre = t->clone_changed(part);
				t->mark_as_cut();
				enqueue_output_token(pre);
			}
			Token* sep = t->clone_changed(orth.charAt(i), sep_type_);
			t->mark_as_cut();
			enqueue_output_token(sep);
			splitfrom = i + 1;
		}
	}
	if (splitfrom == 0) {
		enqueue_output_token(t);
	} else {
		if (splitfrom < orth.length()) {
			UnicodeString part;
			orth.extractBetween(splitfrom, orth.length(), part);
			t->set_orth(part);
			enqueue_output_token(t);
		} else {
			delete t;
		}
	}
}

bool BasicSplitLayer::is_split_char(UChar c)
{
	return split_chars_.find(c) != split_chars_.end();
}

} /* end namespace Toki */
