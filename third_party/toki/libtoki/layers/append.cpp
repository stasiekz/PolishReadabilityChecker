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

#include <libtoki/layers/append.h>
#include <libtoki/token.h>
#include <libpwrutils/util.h>

#include <boost/property_tree/ptree.hpp>

namespace Toki {

AppendLayer::AppendLayer(TokenSource *input, const Config::Node &props)
	: TokenLayer(input, props), append_()
{
	append_ = UnicodeString::fromUTF8(props.get("append", "!")).unescape();
}

std::string AppendLayer::info() const
{
	return "append";
}

std::string AppendLayer::long_info() const
{
	return TokenLayer::long_info() + ", append: " + PwrNlp::to_utf8(append_);
}

Token* AppendLayer::process_token(Token* t)
{
	t->set_orth(t->orth() + append_);
	return t;
}

} /* end namespace Toki */
