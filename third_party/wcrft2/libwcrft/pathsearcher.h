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

#ifndef WCRFT_PATH_SEARCHER_H
#define WCRFT_PATH_SEARCHER_H

#include <loki/Singleton.h>

#include <libpwrutils/pathsearch.h>

#include "exception.h"

namespace Wcrft {

/**
 * Path search utility to be used as singleton instance. Able to find
 * path to a particular file or directory according to path search order
 * that has been embedded in definitions by the build process.
 * This class abstracts from WCRFT-specific subdirs and may be shared
 * among tagger instances.
 */
class WcrftPathSearcher : public PwrNlp::PathSearcher<FileNotFound> {
public:
	WcrftPathSearcher();

	/**
	 * Look for a directory under the search path and return a path to a
	 * directory that exists, or an empty string in case of failure
	 * @param filename the directory to look for
	 * @param info info about the directory to be displayed if verbose loading
	 *             is on. Empty info string suppreses loading info.
	 */
	std::string find_dir(const std::string& filename, const std::string& info = "") const;

	/**
	 * Convenience wrapper around find_dir to throw an exception
	 * when the directory is not found.
	 */
	std::string find_dir_or_throw(const std::string& filename, const std::string& info = "") const;
};

typedef Loki::SingletonHolder<WcrftPathSearcher> WcrftPathSearcherSingleton;

}

#endif // WCRFT_PATH_SEARCHER_H
