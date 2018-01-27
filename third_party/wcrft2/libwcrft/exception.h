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

#ifndef WCRFT_EXCEPTION_H
#define WCRFT_EXCEPTION_H

#include <libpwrutils/exception.h>

namespace Wcrft {

/**
 * Base class for all tagger errors. Derives from
 * @c PwrNlp::Error. Call member function @c what to get a
 * human-readable message associated with the error.
 */
class WcrftError : public PwrNlp::PwrNlpError
{
public:
	/**
	 * Instantiate an instance with the given message.
	 * @param what The message to associate with this error.
	 */
	WcrftError(const std::string &what);

	virtual ~WcrftError() throw();

	/// PwrNlpError override.
	virtual std::string scope() const
	{
		return "Wcrft";
	}
};

/**
 * To be thrown when a file is not found in any of the expected paths.
 */
class FileNotFound : public WcrftError
{
public:
	/**
	 * @brief Instantiates the error.
	 * @param filename path that was expected to exist.
	 * @param paths where the file was sought.
	 * @param where short description of what the file was supposed to be
	 *        (e.g. "config file") or empty string.
	 */
	FileNotFound(
			const std::string& filename,
			const std::string& paths,
			const std::string& where);

	virtual ~FileNotFound() throw();

	/// Get human-readable error message.
	virtual std::string info() const;

	std::string filename, paths, where;
};

}

#endif // WCRFT_EXCEPTION_H
