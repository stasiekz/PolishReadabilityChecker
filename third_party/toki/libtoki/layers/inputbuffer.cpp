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

#include <libtoki/layers/inputbuffer.h>
#include <libtoki/token.h>

namespace Toki {

InputBufferLayer::InputBufferLayer(TokenSource* input,
		const Config::Node& props)
	: TokenLayer(input, props), buffer_()
{
}

InputBufferLayer::~InputBufferLayer()
{
	restart();
}

void InputBufferLayer::restart()
{
	while (!buffer_.empty()) {
		delete buffer_.front();
		buffer_.pop();
	}
}

std::string InputBufferLayer::long_info() const
{
	std::stringstream ss;
	ss << ", input_buffer(" << buffer_.size() << ")";
	return TokenLayer::long_info() + ss.str();
}

Token* InputBufferLayer::get_token_from_input()
{
	if (buffer_.empty()) {
		return TokenLayer::get_token_from_input();
	} else {
		Token* t = buffer_.front();
		buffer_.pop();
		return t;
	}
}

void InputBufferLayer::put_back_token(Token *t)
{
	buffer_.push(t);
}

} /* end namespace Toki */
