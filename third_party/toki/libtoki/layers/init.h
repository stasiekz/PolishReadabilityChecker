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

#ifndef LIBTOKI_INITTOKENLAYERS_H
#define LIBTOKI_INITTOKENLAYERS_H

namespace Toki {

/**
 * Classes derived from TokenLayer in the library that want to be avaliable
 * via the factory should add relevant register calls in the implementation
 * of this function. @see TokenLayer for details.
 */
bool init_token_layers();

} //end namespace Toki

#endif // LIBTOKI_INITTOKENLAYERS_H
