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

#ifndef LIBMACA_CONFIGNODE_H
#define LIBMACA_CONFIGNODE_H

#include <libtoki/util/confignode.h>
#include <libmaca/exception.h>

namespace Maca {

/// pull libtoki config namespace into our own
namespace Config = ::Toki::Config;

/**
 * Exception class used for signalling missing values in config objects
 */
class ConfigValueMissing : public MacaError
{
public:
	/// Constructor with an optional description string
	ConfigValueMissing(const std::string& attribute,
			const std::string& where = "");

	/// Destructor
	~ConfigValueMissing() throw();

	/// MacaError override
	std::string info() const;

	/// the missing item
	std::string attribute;

	/// optional circumstance info
	std::string where;
};

} /* end ns Maca */

#endif // LIBMACA_CONFIGNODE_H
