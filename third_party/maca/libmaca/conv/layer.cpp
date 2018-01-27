/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

#include <libmaca/conv/layer.h>

namespace Maca {
namespace Conversion {

Layer::Layer()
	: source_(NULL)
{
}

Layer::~Layer()
{
}

void Layer::set_source(Corpus2::TokenSource *src)
{
	source_ = src;
}

Corpus2::TokenSource* Layer::source()
{
	return source_;
}

OneTagsetLayer::OneTagsetLayer(const Corpus2::Tagset& tagset)
	: tagset_(tagset)
{
}

const Corpus2::Tagset& OneTagsetLayer::tagset_from() const
{
	return tagset_;
}

const Corpus2::Tagset& OneTagsetLayer::tagset_to() const
{
	return tagset_;
}

} /* end ns Conversion */
} /* end ns Maca */
