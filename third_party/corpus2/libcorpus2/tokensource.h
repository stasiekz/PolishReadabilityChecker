/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.CORPUS2, LICENSE.POLIQARP, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBCORPUS2_TOKEN_SOURCE_H
#define LIBCORPUS2_TOKEN_SOURCE_H

#include <libpwrutils/tokensource.h>

namespace Corpus2 {

/// forward declaration
class Token;

/// typedef for a Corpus2 Token TokenSource, based on the general template
typedef PwrNlp::TokenSourceTemplate<Token> TokenSource;

/// pull RangeSource and make_range_source into our namespace
using PwrNlp::RangeSource;
using PwrNlp::make_range_source;

} /* end ns Corpus2 */

#endif // LIBCORPUS2_TOKEN_SOURCE_H
