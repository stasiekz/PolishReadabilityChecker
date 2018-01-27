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

#include <libtoki/layers/combine.h>
#include <libtoki/token.h>

namespace Toki {

CombineLayer::CombineLayer(TokenSource* input, const Config::Node& props)
	: OutputQueueLayer(input, props)
{
}

void CombineLayer::prepare_more_tokens(Token* t)
{
	Token* t2 = get_token_from_input();
	if (t2) {
		t->set_orth(t->orth() + t2->orth());
		delete t2;
	}
	enqueue_output_token(t);
}

} /* end namespace Toki */
