/* This file is part of WCRFT
  Copyright (C) 2014 Radosław Warzocha, Adam Radziszewski.
  WCRFT is free software; you can redistribute and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option)
  any later version.

  WCRFT is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.

  See the LICENCE and COPYING files for more details
 */

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <libwcrft/settings.h>
#include "pathsearcher.h"

namespace Wcrft {

WcrftPathSearcher::WcrftPathSearcher()
	: PwrNlp::PathSearcher<FileNotFound>(WCRFT_PATH_SEPARATOR)
{
#ifdef WCRFT_DATA_DIR
	this->set_search_path(WCRFT_DATA_DIR);
#else
	this->set_search_path(".");
#endif
}

std::string WcrftPathSearcher::find_dir_or_throw(const std::string& filename, const std::string& info) const
{
	std::string fn = find_dir(filename, info);
	if (fn.empty()) {
		throw FileNotFound(filename, get_search_path_string(), info);
	}
	return fn;
}


std::string WcrftPathSearcher::find_dir(const std::string& filename, const std::string& info) const
{
	boost::filesystem::path i(filename);
	if (i.is_complete()) {
		if (boost::filesystem::exists(i) && boost::filesystem::is_directory(i)) {
			if (this->get_verbose()) {
				std::cerr << "Found " << info << " file: "
						<< i.string() << "\n";
			}
			return i.string();
		}
		return "";
	}
	BOOST_FOREACH(const std::string& s, this->get_search_path()) {
		boost::filesystem::path pi = s / i;
		if (boost::filesystem::exists(pi) && boost::filesystem::is_directory(pi)) {
			if (this->get_verbose()) {
				std::cerr << "Found " << info << " file: "
						<< pi.string() << "\n";
			}
			return pi.string();
		}
	}
	return "";
}

}
