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

    See the LICENCE, COPYING.LESSER and COPYING files for more details.
*/

// no namespace or header guard, should not be used directly

typedef boost::iterator_range<std::string::const_iterator> string_range;

typedef std::vector<string_range> string_range_vector;

typedef boost::iterator_range<std::string::iterator> string_range_mutable;

