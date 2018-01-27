/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENCE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef PWRNLP_PLURAL_H
#define PWRNLP_PLURAL_H

#include <string>

namespace PwrNlp {

/// Pluralize sg according to amount, regular English plural
std::string enpl(int amount, const std::string& sg);

/// Pluralize (English), output the number and the pluralized word
std::string enpln(int amount, const std::string& sg);

/// Pluralize according to amount, custom English-style plural
std::string enpl(int amount, const std::string& sg, const std::string& pl);

/// Pluralize (English), output the number and the pluralized word
std::string enpln(int amount, const std::string& sg, const std::string& pl);

/// Pluralize according to amount, Polish plural
std::string plpl(int amount, const std::string& gen0, const std::string& nom1, const std::string& acc2);

/// Pluralize (Polish) output the number and the pluralized word
std::string plpln(int amount, const std::string& gen0, const std::string& nom1, const std::string& acc2);

} /* end ns PwrNlp */

#endif // PWRNLP_PATHSEARCH_H
