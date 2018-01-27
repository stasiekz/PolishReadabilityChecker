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

#include <libmaca/util/confignode.h>
#include <sstream>

namespace Maca {

ConfigValueMissing::ConfigValueMissing(const std::string& attribute,
		const std::string& where)
	: MacaError("Value for required attribute '" + attribute + "' missing")
	, attribute(attribute), where(where)
{
}

ConfigValueMissing::~ConfigValueMissing() throw ()
{
}

std::string ConfigValueMissing::info() const
{
	std::stringstream ss;
	ss << "Value for required config attribute '" << attribute
		<< "' missing";
	if (!where.empty()) {
		ss << " in " << where;
	}
	return ss.str();
}

} /* end ns Maca */
