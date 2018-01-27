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

#ifndef LIBTOKI_GROUPAFFIXSPLITLAYER_H
#define LIBTOKI_GROUPAFFIXSPLITLAYER_H

#include <libtoki/layers/affixsplit.h>

namespace Toki {

/**
 * Layer to recognize and split prefix and posftix characters in tokens.
 * All prefix characters are extracted as one token, then the unrecognized
 * part is output as the body token (with the type unmodified), and then
 * the suffix characters are output as the suffix token. This layer works
 * a lot like AffixSpliLayer with the difference being that the extracted
 * pre- and suffixes are not split into one-letter tokens. Also see
 * GroupSplitLayer.
 */
class GroupAffixSplitLayer : public AffixSplitLayer
{
public:
	/**
	 * Constructor.
	 *
	 * No new configuration keys processed. See parent class.
	 */
	GroupAffixSplitLayer(TokenSource* input, const Config::Node& props);

	/// TokenLayer override
	std::string info() const;

	/// TokenLayer override
	std::string long_info() const;

protected:
	/// OutputQueueTokenLayer override
	void prepare_more_tokens(Token* t);
};

} /* end ns Toki */


#endif // LIBTOKI_GROUPAFFIXSPLITLAYER_H
