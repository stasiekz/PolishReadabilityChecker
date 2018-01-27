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

#include <libmaca/util/settings.h>

#ifdef HAVE_CONFIG_D_H
#include <libmaca/config_d.h>
#endif


namespace Maca {

MacaPathSearcher::MacaPathSearcher()
	: PwrNlp::PathSearcher<FileNotFound>(LIBMACA_PATH_SEPARATOR)
{
#ifdef LIBMACA_DATA_DIR
	set_search_path(LIBMACA_DATA_DIR);
#else
	set_search_path(".");
#endif
}

Config::Node get_named_config(const std::string &id)
{
	std::string fn = Path::Instance().find_file_or_throw(
			id + ".ini", "analyser config");
	return Config::from_file(fn);
}

} /* end namespace Maca */
