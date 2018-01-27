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
#ifndef LIBMACA_CONV_REMOVEDUPES_H
#define LIBMACA_CONV_REMOVEDUPES_H

#include <libmaca/conv/layer.h>
#include <libmaca/util/confignode.h>

namespace Maca {
namespace Conversion {

/**
 * A layer that removes any lexeme duplicates in tokens
 */
class RemoveDupesLayer : public OneTagsetLayer
{
public:
        /**
         * Constructor for a RemoveDupesLayer working within a tagset.
         */
        RemoveDupesLayer(const Corpus2::Tagset& tagset);

        /**
         * Config node constructor. The config node is passed to the parent
         * class, and then a RemoveDupesLayer is created from it.
         */
        RemoveDupesLayer(const Config::Node& cfg);

        /// Destructor
        ~RemoveDupesLayer();

        /// Cloning
        RemoveDupesLayer* clone() const;

        /// Layer override
        Corpus2::Token* get_next_token();

private:
};


} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_CONV_REMOVEDUPES_H
