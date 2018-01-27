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

#ifndef LIBTOKI_COMBINELAYER_H
#define LIBTOKI_COMBINELAYER_H

#include <libtoki/layers/outputqueue.h>

namespace Toki {

/**
 * Sample layer to glue pairs of tokens together.
 */
class CombineLayer : public OutputQueueLayer
{
public:
	/**
	 * Constructor.
	 *
	 * No new configuration keys processed. See parent class.
	 */
	CombineLayer(TokenSource* input, const Config::Node& props);

protected:
	/// OutputQueueLayer override
	void prepare_more_tokens(Token* t);
};

} //end namespace Toki

#endif // LIBTOKI_COMBINELAYER_H
