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

#include <libpwrutils/plural.h>
#include <sstream>

namespace PwrNlp {

std::string enpl(int amount, const std::string& sg)
{
	if (amount == 1) {
		return sg;
	} else {
		return sg + "s";
	}
}

std::string enpln(int amount, const std::string& sg)
{
	std::stringstream ss;
	ss << amount << " " << enpl(amount, sg);
	return ss.str();
}

std::string enpl(int amount, const std::string& sg, const std::string& pl)
{
	if (amount == 1) {
		return sg;
	} else {
		return pl;
	}
}

std::string enpln(int amount, const std::string& sg, const std::string& pl)
{
	std::stringstream ss;
	ss << amount << " " << enpl(amount, sg, pl);
	return ss.str();
}


std::string plpl(int amount, const std::string& gen0, const std::string& nom1, const std::string& acc2)
{
	if (amount == 1) {
		return nom1;
	} else {
		amount %= 100;
		if (amount > 10 && amount < 20) {
			return gen0;
		} else {
			amount %= 10;
			if (amount == 2 || amount == 3 || amount == 4) {
				return acc2;
			} else {
				return gen0;
			}
		}
	}
}

std::string plpln(int amount, const std::string& gen0, const std::string& nom1, const std::string& acc2)
{
	std::stringstream ss;
	ss << amount << " " << plpl(amount, gen0, nom1, acc2);
	return ss.str();
}

} /* end namespace PwrNlp */
