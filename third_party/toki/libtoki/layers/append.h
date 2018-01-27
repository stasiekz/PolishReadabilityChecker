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

#ifndef LIBTOKI_APPENDLAYER_H
#define LIBTOKI_APPENDLAYER_H

#include <libtoki/layers/tokenlayer.h>

#include <unicode/unistr.h>

namespace Toki {

/**
 * A sample layer that appends a constant bit of text to the orth of each
 * token.
 */
class AppendLayer : public TokenLayer
{
public:
	/**
	 * Constructor.
	 *
	 * Keys recognized in the configuration:
	 * - append - Text to append, treated as UTF-8 and unescaped. Defaults
	 *            to an exlamation mark (!).
	 */
	AppendLayer(TokenSource* input, const Config::Node& props);

	/// TokenLayer override
	Token* process_token(Token *t);

	/// TokenLayer override
	virtual std::string info() const;

	/// TokenLayer override
	std::string long_info() const;
private:
	/// The string to append
	UnicodeString append_;
};

} //end namespace Toki

#endif // LIBTOKI_APPENDLAYER_H
