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

#include <libpwrutils/pathsearch.h>

#include <libpwrutils/exception.h>
#include <boost/foreach.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <fstream>
#include <iostream>

namespace PwrNlp {

PathSearcherBase::PathSearcherBase(const std::string& separator)
	: paths_(), separator_(separator), verbose_loading_(false)
{
	if (separator.empty()) {
		std::cerr << "No path separator! Defaulting to :\n";
	} else if (separator.size() > 1) {
		std::cerr << "Separator size > 1, truncating: '"
				<< separator << "'\n";
		separator_ = separator_[0];
	}
}

PathSearcherBase::~PathSearcherBase()
{
}

const std::vector<std::string>& PathSearcherBase::get_search_path() const
{
	return paths_;
}

std::string PathSearcherBase::get_search_path_string() const
{
	return boost::algorithm::join(paths_, separator_);
}

void PathSearcherBase::set_search_path(
		const std::vector<std::string> &paths)
{
	paths_ = paths;
}

void PathSearcherBase::set_search_path(const std::string &paths)
{
	paths_.clear();
	boost::algorithm::split(paths_, paths,
			boost::algorithm::is_any_of(separator_));
}

const std::string& PathSearcherBase::get_path_separator() const
{
	return separator_;
}

std::string PathSearcherBase::find_file(const std::string& filename,
		const std::string& info) const
{
	boost::filesystem::path i(filename);
	if (i.is_complete()) {
		if (boost::filesystem::exists(i) &&
				!boost::filesystem::is_directory(i)) {
			if (verbose_loading_) {
				std::cerr << "Found " << info << " file: "
						<< i.string() << "\n";
			}
			return i.string();
		}
		return "";
	}
	BOOST_FOREACH(const std::string& s, paths_) {
		boost::filesystem::path pi = s / i;
		if (boost::filesystem::exists(pi) &&
				!boost::filesystem::is_directory(pi)) {
			if (verbose_loading_) {
				std::cerr << "Found " << info << " file: "
						<< pi.string() << "\n";
			}
			return pi.string();
		}
	}
	return "";
}

bool PathSearcherBase::open_stream(const std::string& filename,
		std::ifstream& ifs, const std::string& info) const
{
	std::string f = find_file(filename, info);
	if (!f.empty()) {
		ifs.open(f.c_str());
		return true;
	}
	return false;
}

std::vector<std::string> PathSearcherBase::list_files(const std::string& suffix) const
{
	using boost::filesystem::directory_iterator;
	std::vector<std::string> out;
	BOOST_FOREACH(const std::string& s, get_search_path()) {
		boost::filesystem::path p(s);
		if (boost::filesystem::is_directory(s)) {
			for (directory_iterator i(p); i != directory_iterator(); ++i) {
				boost::filesystem::path in = i->path();
				if (in.extension() == suffix) {
#if !defined(BOOST_FILESYSTEM_VERSION) || BOOST_FILESYSTEM_VERSION == 2
					out.push_back(in.stem());
#else
					out.push_back(in.stem().string());
#endif
				}
			}
		}
	}
	return out;
}


ConfigPathSetter::ConfigPathSetter(PathSearcherBase& ps,
		const std::string &new_path)
	: ps_(ps), old_path_(ps.get_search_path())
{
	ps_.set_search_path(new_path);
}

ConfigPathSetter::~ConfigPathSetter()
{
	ps_.set_search_path(old_path_);
}

} /* end namespace PwrNlp */
