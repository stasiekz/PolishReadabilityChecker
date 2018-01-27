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

#ifndef LIBTOKI_TOKENSOURCE_H
#define LIBTOKI_TOKENSOURCE_H

#include <libpwrutils/tokensource.h>

namespace Toki {

class Token;

/// Typedef for a PwrNlp TokenSource
typedef PwrNlp::TokenSourceTemplate<Token> TokenSource;

/// pull RangeSource and make_range_source into our namespace
using PwrNlp::RangeSource;
using PwrNlp::make_range_source;


} /* end ns Toki */


#endif // LIBTOKI_TOKENSOURCE_H
