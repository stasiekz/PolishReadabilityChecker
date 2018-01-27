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

#ifndef PWRNLP_PATHSEARCH_H
#define PWRNLP_PATHSEARCH_H

#include <unicode/unistr.h>

#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace PwrNlp {

/**
 * A class to find files in a given search path.
 *
 */
class PathSearcherBase
{
public:
	/**
	 * Create a new PathSearcher with a separator and no paths.
	 * Note: the separator MUST be a single character.
	 */
	PathSearcherBase(const std::string& separator);

	/// Destructor
	virtual ~PathSearcherBase();

	bool get_verbose() const {
		return verbose_loading_;
	}

	void set_verbose(bool v) {
		verbose_loading_ = v;
	}

	/// Search path vector accessor
	const std::vector<std::string>& get_search_path() const;

	/// Search path string representation accessor
	std::string get_search_path_string() const;

	/// Seacrh path setter, vector of already-split paths
	void set_search_path(const std::vector<std::string> &paths);

	/// Search path setter, string with the paths separated by the
	/// separator char
	void set_search_path(const std::string &);

	/// Separator accessor
	const std::string& get_path_separator() const;

	/**
	 * Look for a filename under the search path and return a path to a
	 * file that exists, or an empty string in case of failure
	 * @param filename the filename to look for
	 * @param info info about the file to be displayed if verbose loading
	 *             is on. Empty info string suppreses loading info.
	 */
	std::string find_file(const std::string& filename,
			const std::string& info = "") const;

	/**
	 * Open a file stream for a file in the library search path
	 * @param filename the filename to look for
	 * @param ifs the stream to use
	 * @param info info about the file to be displayed if verbose loading
	 *             is on. Empty info string suppreses loading info.
	 */
	bool open_stream(const std::string& filename, std::ifstream& ifs,
			const std::string& info = "") const;

	/**
	 * Convenience wrapper around find_file to throw an exception
	 * when the file is not found.
	 * Virtual, as it throws the exception defined by the child class.
	 */
	virtual std::string find_file_or_throw(const std::string& filename,
			const std::string& where) const = 0;

	/**
	 * Wrapper around open_stream to throw an exception when the file is not
	 * found. Virtual, as it throws the exception defined by the child class.
	 */
	virtual void open_stream_or_throw(const std::string& filename,
			std::ifstream& ifs, const std::string& where) const = 0;

	/**
	 * Look for files matching a condition.
	 */
	std::vector<std::string> list_files(const std::string& suffix) const;

private:
	/// The search paths
	std::vector<std::string> paths_;

	/// The path separator
	std::string separator_;

	/// Flag to control outputting actual loaded files
	bool verbose_loading_;
};

/**
 * The templated bit of the path search utility.
 *
 * The passed exception class' constructor should take three parameters:
 * the filename, the current search path string and the circumstance
 * string (where).
 */
template<typename E>
class PathSearcher : public PathSearcherBase
{
public:
	PathSearcher(const std::string& separator)
		: PathSearcherBase(separator)
	{
	}

	/**
	 * Convenience wrapper around find_file to throw an exception
	 * when the file is not found.
	 */
	std::string find_file_or_throw(const std::string& filename,
			const std::string& where) const;

	/**
	 * Convenience template wrapper around open_stream to throw an
	 * exception when the file is not found.
	 */
	void open_stream_or_throw(const std::string& filename,
			std::ifstream& ifs, const std::string& where) const;

};

/**
 * Convenience class to set the library config path and have it
 * automatically reset to the original value upon destruction
 */
class ConfigPathSetter
{
public:
	/// Constructor
	ConfigPathSetter(PathSearcherBase& ps, const std::string& new_path);

	/// Destructor
	~ConfigPathSetter();
private:
	/// The affected PathSearcher
	PathSearcherBase& ps_;

	/// Stored old path
	std::vector<std::string> old_path_;
};


/* Implementation */

template<class E>
std::string PathSearcher<E>::find_file_or_throw(
		const std::string& filename, const std::string& info) const
{
	std::string fn = find_file(filename, info);
	if (fn.empty()) {
		throw E(filename, get_search_path_string(), info);
	}
	return fn;
}

template<class E>
void PathSearcher<E>::open_stream_or_throw(const std::string& filename,
		std::ifstream& ifs, const std::string& info) const
{
	if (!open_stream(filename, ifs, info)) {
		throw E(filename, get_search_path_string(), info);
	}
}

} /* end ns PwrNlp */

#endif // PWRNLP_PATHSEARCH_H
