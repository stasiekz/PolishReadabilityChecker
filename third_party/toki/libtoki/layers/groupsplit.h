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

#ifndef LIBTOKI_GROUPSPLITLAYER_H
#define LIBTOKI_GROUPSPLITLAYER_H

#include <libtoki/layers/basicsplit.h>

namespace Toki {

/**
 * A layer to split tokens containing one of the listed characters.
 * Matching characters are extracted into continous tokens with a given
 * type, remaining bits of the orth are made into tokens with the original
 * token's type. This layer works a lot like BasicSplitLayer, with the
 * difference being that the extracted separators are not split into
 * one-letter tokens but held together, so if a dot -- . -- is a separator
 * the token "aa..bb" will be split into three tokens: "aa", "..", and
 * "bb".
 */
class GroupSplitLayer : public BasicSplitLayer
{
public:
	/**
	 * Constructor.
	 *
	 * No new configuration keys processed. See parent class.
	 */
	GroupSplitLayer(TokenSource* input, const Config::Node& props);

	/// TokenLayer override
	std::string info() const;

	/// TokenLayer override
	std::string long_info() const;

protected:
	/// OutputQueueTokenLayer override
	void prepare_more_tokens(Token* t);
};

} /* end ns Toki */

#endif // LIBTOKI_GROUPSPLITLAYER_H
