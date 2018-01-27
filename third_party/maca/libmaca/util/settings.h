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

#ifndef LIBMACA_SETTINGS_H
#define LIBMACA_SETTINGS_H

#include <libmaca/util/confignode.h>
#include <libmaca/exception.h>
#include <libpwrutils/pathsearch.h>
#include <loki/Singleton.h>

namespace Maca {

class MacaPathSearcher : public PwrNlp::PathSearcher<FileNotFound>
{
public:
	MacaPathSearcher();
};

typedef Loki::SingletonHolder<MacaPathSearcher> Path;

/**
 * Get a config from standard directories by name, shorthand function.
 */
Config::Node get_named_config(const std::string& id);

} /* end ns Maca */

#endif // LIBMACA_SETTINGS_H
