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

#include <libtoki/layers/outputqueue.h>
#include <libtoki/token.h>

#include <cassert>

namespace Toki {

OutputQueueLayer::OutputQueueLayer(TokenSource* input,
		const Config::Node& props)
	: TokenLayer(input, props), queue_()
{
}

OutputQueueLayer::~OutputQueueLayer()
{
	clear_queue();
}

void OutputQueueLayer::enqueue_output_token(Token *t)
{
	assert(t);
	queue_.push(t);
}

Token* OutputQueueLayer::get_next_token()
{
	if (queue_.empty()) {
		Token* t = get_token_from_input();
		if (t) {
			if (should_process_token_type(t->type())) {
				prepare_more_tokens(t);
			} else {
				enqueue_output_token(t);
			}
		}
	}
	if (queue_.empty()) {
		return NULL;
	} else {
		Token* t = queue_.front();
		queue_.pop();
		return t;
	}
}

void OutputQueueLayer::clear_queue()
{
	while (!queue_.empty()) {
		Token* t = queue_.front();
		delete t;
		queue_.pop();
	}
}

void OutputQueueLayer::restart()
{
	clear_queue();
}

std::string OutputQueueLayer::long_info() const
{
	std::stringstream ss;
	ss << ", out_queue(" << queue_.size() << ")";
	return TokenLayer::long_info() + ss.str();
}

} /* end namespace Toki */
