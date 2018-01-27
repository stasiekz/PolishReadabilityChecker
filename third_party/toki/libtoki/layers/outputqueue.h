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

#ifndef LIBTOKI_OUTPUTQUEUELAYER_H
#define LIBTOKI_OUTPUTQUEUELAYER_H

#include <libtoki/layers/tokenlayer.h>

#include <queue>

namespace Toki {

/**
 * A base class for queue-based token layers that work by consuming tokens
 * from the source, processing them, and adding processed tokens to an
 * output queue.
 */
class OutputQueueLayer : public TokenLayer
{
public:
	/// Consructor
	OutputQueueLayer(TokenSource* input, const Config::Node& props);

	/// Destructor
	~OutputQueueLayer();

	/**
	 * TokenSource override. Returns the front element from the queue if
	 * any, otherwise grabs a token from he input, checks it for NULL and
	 * checks should_process_token_type, and then calls prepare_more_tokens
	 * before trying to return the front element from the queue again.
	 * @return a Token, or NULL if the queue was empty and
	 *         prepare_more_tokens did not call enqueue_output_token() at
	 *         least once.
	 */
	Token* get_next_token();

	/**
	 * TokenLayer override. Destroys the output token queue.
	 */
	void restart();

	/// TokenLayer override
	std::string long_info() const;

protected:
	/**
	 * Add an token to the output queue. Derived classes should use this to
	 * push ready tokens to the output from within prepare_more_tokens().
	 */
	void enqueue_output_token(Token* t);

	/**
	 * Derived classes should override this with code that consumes tokens
	 * from the source as many times as it is necessary to have at least
	 * one token ready (=call enqueue_output_token() at least once). The
	 * argument is a token grabbed from the input source, guaranteed to be
	 * not null, and type-checked by should_process_token_type.
	 *
	 * The derived class should either delete this token or enqueue it, and
	 * may call get_token_from_input to get more tokens if necessary (but
	 * should check for NULL and should_process_token_type() in that case).
	 *
	 * If enqueue_output_token() is not called, get_next_token will end up
	 * returning NULL which will signal an end of tokens to the client.
	 * @param t token to process, guaranteed to be not NULL
	 */
	virtual void prepare_more_tokens(Token* t) = 0;

private:
	/**
	 * Remove all tokens from the queue and dispose of them
	 */
	void clear_queue();

	/// The queue of tokens ready to be returned by get_next_token
	std::queue<Token*> queue_;
};

} //end namespace Toki

#endif // LIBTOKI_OUTPUTQUEUELAYER_H
