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

#include <libtoki/util/settings.h>

#ifdef HAVE_CONFIG_D_H
#include <libtoki/config_d.h>
#endif

#include <libtoki/exception.h>
#include <boost/foreach.hpp>


#include <iostream>

namespace Toki {

	TokiPathSearcher::TokiPathSearcher()
		: PwrNlp::PathSearcher<FileNotFound>(LIBTOKI_PATH_SEPARATOR)
	{
#ifdef LIBTOKI_DATA_DIR
		set_search_path(LIBTOKI_DATA_DIR);
#else
		set_search_path(".");
#endif
	}

	const Config::Node& default_config()
	{
		bool initialized = false;
		static Config::Node cfg;
		if (!initialized) {
			try {
				cfg = get_named_config("config");
			} catch (TokiError& e) {
				throw TokiError("default config error! " + e.info());
			}
			initialized = true;
		}
		return cfg;
	}

	Config::Node get_named_config(const std::string &id)
	{
		std::string fn = Path::Instance().find_file_or_throw(
				id + ".ini", "tokenizer config");
		return Config::from_file(fn);
	}


	namespace {
		std::ostream* default_error_stream_ = NULL;
		bool des_init_ = false;
	} // end namespace

	std::ostream* get_default_error_stream()
	{
		if (!des_init_) {
			set_default_error_stream(&std::cerr);
		}
		return default_error_stream_;
	}

	void set_default_error_stream(std::ostream *os)
	{
		des_init_ = true;
		default_error_stream_ = os;
	}

} /* end namespace Toki */
