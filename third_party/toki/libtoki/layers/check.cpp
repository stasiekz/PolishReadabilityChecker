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

#include <libtoki/layers/check.h>
#include <libtoki/token.h>

#include <unicode/uchar.h>

namespace Toki {

CheckLayer::CheckLayer(TokenSource* input, const Config::Node& props)
	: TokenLayer(input, props)
	, max_token_size_(props.get("max_token_size", 500))
	, max_sentence_size_(props.get("max_sentence_size", 1000))
	, huge_token_warn_(props.get("huge_token_warn", true))
	, huge_sentence_warn_(props.get("huge_sentence_warn", true))
	, huge_sentence_split_(props.get("huge_sentence_split", false))
	, check_spaces_(props.get("check_spaces", false))
	, emit_0_on_no_warnings_(false)
	, emit_1_on_warnings_(false)
	, token_counter_(0)
	, sentence_size_counter_(0)
{
	std::string format = props.get("warn_format", "text");
	if (format == "1") {
		emit_1_on_warnings_ = true;
	} else if (format == "01") {
		emit_0_on_no_warnings_ = true;
		emit_1_on_warnings_ = true;
	}
}

CheckLayer::~CheckLayer()
{
}

void CheckLayer::restart()
{
	token_counter_ = 0;
	sentence_size_counter_ = 0;
}

Token* CheckLayer::process_token(Token *t)
{
	bool w = false;
	++token_counter_;
	if (t->begins_sentence()) {
		sentence_size_counter_ = 1;
	} else {
		if (sentence_size_counter_++ == max_sentence_size_) {
			w = warn("Huge sentence");
			if (huge_sentence_split_) {
				t->set_begins_sentence(true);
				sentence_size_counter_ = 1;
			}
		}
	}
	if (t->orth().length() > max_token_size_) {
		w = warn("Huge token");
	}
	if (check_spaces_) {
		for (int i = 0; i < t->orth().length(); ++i) {
			if (u_isUWhiteSpace(t->orth().charAt(i))) {
				w = warn("Whitespace in orth");
				break;
			}
		}
	}
	if (!w && emit_0_on_no_warnings_ && error_stream_) {
		(*error_stream_) << '0';
	}
	return t;
}

bool CheckLayer::warn(const std::string &msg)
{
	if (error_stream_) {
		if (emit_1_on_warnings_) {
			(*error_stream_) << '1';
		} else {
			(*error_stream_) << msg << "(token " << token_counter_ << ") ";
		}
	}
	return true;
}

std::string CheckLayer::info() const
{
	std::stringstream ss;
	ss << "check{"
		<< max_token_size_ << "|"
		<< max_sentence_size_ << "|"
		<< (huge_token_warn_ ? 'T' : 't')
		<< (huge_sentence_warn_ ? 'S' : 's')
		<< (huge_sentence_split_ ? "X" : "")
		<< (emit_0_on_no_warnings_ ? "0" : "")
		<< (emit_1_on_warnings_ ? "1" : "")
		<< "}";
	return ss.str();
}

std::string CheckLayer::long_info() const
{
	std::stringstream ss;
	ss << "check{"
		<< "Huge token:" << max_token_size_ << ", "
		<< "sentence:" << max_sentence_size_ << " ";
	if (huge_token_warn_) {
		if (huge_sentence_warn_) {
			ss << "Warn on both";
		} else {
			ss << "Warn on token";
		}
	} else if (huge_sentence_warn_) {
		ss << "Warn on sentence";
	} else {
		ss << "No warn";
	}
	ss << ". ";
	if (huge_sentence_split_) {
		ss << "Split";
	}
	ss << (emit_0_on_no_warnings_ ? "0" : "")
		<< (emit_1_on_warnings_ ? "1" : "");
	return TokenLayer::long_info() + ss.str();
}

} /* end ns Toki */
