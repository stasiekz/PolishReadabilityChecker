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

#ifndef LIBMACA_CONV_ATTRIBUTECOPIER_H
#define LIBMACA_CONV_ATTRIBUTECOPIER_H

#include <libcorpus2/token.h>

namespace Maca {
namespace Conversion {

/**
 * Helper function to creat a list of attributes given a tagset and a
 * string of atttribute names, colon or space separated.
 */
Corpus2::mask_t make_attributes_mask(const Corpus2::Tagset& tagset,
		const std::string& str);

/**
 * Helper function to creat a list of attributes given a tagset and a
 * string of atttribute names, colon or space separated. The attributes are
 * appended to the passed vector.
 */
Corpus2::mask_t append_attributes_mask(Corpus2::mask_t& v,
		const Corpus2::Tagset& tagset, const std::string& str);

/**
 * Helper function to copy some attributes from one tag to another
 */
void copy_attributes(const Corpus2::Tag& from,
		const Corpus2::mask_t& amask, Corpus2::Tag& to);

/**
 * Helper function to copy some attributes from one tag to another, for all
 * tags in a token.
 */
void copy_attributes(const Corpus2::Token& from,
		const Corpus2::mask_t& amask, Corpus2::Token& to);

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_CONV_ATTRIBUTECOPIER_H
