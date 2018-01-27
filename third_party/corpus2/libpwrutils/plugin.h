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

#ifndef LIBPWRNLP_PLUGIN_H
#define LIBPWRNLP_PLUGIN_H

#include <boost/function.hpp>

namespace PwrNlp {
namespace Plugin {

/**
 * Convert a plugin name to a shared library name that is expected to
 * contain the plugin.
 */
std::string make_soname(const std::string& scope, const std::string& name);

/**
 * Load a plugin
 */
bool load(const std::string& scope, const std::string& name, bool quiet);

/**
 * Load a plugin, checking if a counter increases after the load,
 * and outputting a disgnostic message if it does not
 */
bool load_check(const std::string& scope, const std::string& name, bool quiet,
		boost::function<size_t(void)> counter, const std::string& what);

} /* end ns Plugin */
} /* end ns PwrNlp */

#endif // LIBPWRNLP_PLUGIN_H
