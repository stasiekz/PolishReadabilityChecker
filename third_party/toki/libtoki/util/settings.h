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

#ifndef LIBTOKI_SETTINGS_H
#define LIBTOKI_SETTINGS_H

#include <libtoki/util/confignode.h>
#include <libpwrutils/pathsearch.h>
#include <libtoki/exception.h>

#include <unicode/unistr.h>

#include <boost/property_tree/ptree.hpp>

#include <map>
#include <ostream>
#include <string>
#include <vector>

#include <loki/Singleton.h>

namespace Toki {

	class TokiPathSearcher : public PwrNlp::PathSearcher<FileNotFound>
	{
	public:
		TokiPathSearcher();
	};

	typedef Loki::SingletonHolder<TokiPathSearcher> Path;


	/**
	 * The default configuration node
	 */
	const Config::Node& default_config();

	/**
	 * Get a config from standard directories by name, shorthand function.
	 */
	Config::Node get_named_config(const std::string& id);

	/**
	 * Get the default error stream used by the library to log errors
	 */
	std::ostream* get_default_error_stream();

	/**
	 * Set the default error stream used by the library to log errors.
	 * NULL disables error logging.
	 */
	void set_default_error_stream(std::ostream* os);


} /* end ns Toki */

#endif // LIBTOKI_SETTINGS_H
