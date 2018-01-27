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

#ifndef LIBMACA_EXCEPTION_H
#define LIBMACA_EXCEPTION_H

#include <libpwrutils/exception.h>

namespace Maca {

/**
 * Base class for all Maca errorss. Derives from
 * @c std::runtime_error. Call member function @c what to get a
 * human-readable message associated with the error.
 */
class MacaError : public PwrNlp::PwrNlpError
{
public:
	/**
	 * Instantiate a MacaError instance with the given message.
	 * @param what The message to associate with this error.
	 */
	MacaError(const std::string &what);

	~MacaError() throw();

	/// PwrNlpError override
	std::string scope() const;
};

class FileNotFound : public MacaError
{
public:
	FileNotFound(const std::string& filename, const std::string& paths,
			const std::string& where);

	~FileNotFound() throw();

	std::string info() const;

	std::string filename, paths, where;
};

} /* end ns Maca */

#endif // LIBMACA_EXCEPTION_H
