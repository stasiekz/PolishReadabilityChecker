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

#include <libmaca/conv/removedupes.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/token.h>

namespace Maca {
namespace Conversion {

RemoveDupesLayer::RemoveDupesLayer(const Corpus2::Tagset& tagset)
        : OneTagsetLayer(tagset)
{
}

RemoveDupesLayer::RemoveDupesLayer(const Config::Node& cfg)
        : OneTagsetLayer(Corpus2::get_named_tagset(cfg.get<std::string>("tagset")))
{
}

RemoveDupesLayer::~RemoveDupesLayer()
{
}

RemoveDupesLayer* RemoveDupesLayer::clone() const
{
        RemoveDupesLayer* copy = new RemoveDupesLayer(tagset());
        return copy;
}

Corpus2::Token* RemoveDupesLayer::get_next_token()
{
        Corpus2::Token* t = source()->get_next_token();
	if (t) {
		t->remove_duplicate_lexemes();
	}
	return t;
}

} /* end ns Conversion */
} /* end ns Maca */
