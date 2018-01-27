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

#ifndef LIBTOKI_INPUTBUFFERLAYER_H
#define LIBTOKI_INPUTBUFFERLAYER_H

#include <libtoki/layers/tokenlayer.h>
#include <queue>

namespace Toki {

/**
 * A base class for layers that might want to look ahead into the token
 * source but might not always want to consume all the tokens.
 */
class InputBufferLayer : public TokenLayer
{
public:
	/**
	 * Constructor. No config keys recognized.
	 */
	InputBufferLayer(TokenSource* input, const Config::Node& props);

	/// Destructor
	~InputBufferLayer();

	/// reset function
	void restart();

	/// TokenLayer override
	std::string long_info() const;

protected:
	/**
	 * TokenLayer override that uses the buffer.
	 */
	Token* get_token_from_input();

	/**
	 * Put back a received token into the buffer
	 */
	void put_back_token(Token* t);

private:
	/// The token buffer
	std::queue<Token*> buffer_;
};

} //end namespace Toki

#endif // LIBTOKI_INPUTBUFFERLAYER_H
