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

#ifndef LIBMACA_DEBUG_H
#define LIBMACA_DEBUG_H

#include <libcorpus2/token.h>
#include <ostream>

namespace Maca {

std::string lexeme_string(const Corpus2::Lexeme& l);

std::string token_string(const Corpus2::Token& t);

void token_output(const Corpus2::Tagset& tagset, std::ostream& os, Corpus2::Token* t);

} /* end ns Maca */

#endif // LIBMACA_DEBUG_H
