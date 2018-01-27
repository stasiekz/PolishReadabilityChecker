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

#ifndef PWRNLP_EXCEPTION_H
#define PWRNLP_EXCEPTION_H

#include <stdexcept>

namespace PwrNlp {

/**
 * Base class for all errors. Derives from @c std::runtime_error.
 *
 * Call member function @c what to get a short human-readable message
 * associated with the error. Call member function @c info to get verbose
 * information about the error in possibly multi-line form.
 */
class PwrNlpError : public std::runtime_error
{
public:
	/**
	 * Instantiate an Error instance with the given message.
	 * @param what The message to associate with this error.
	 */
	PwrNlpError(const std::string &what);

	/// Destructor
	~PwrNlpError() throw();

	/// verbose-info function
	virtual std::string info() const;

	/// scope information (subproject-like)
	virtual std::string scope() const;
};

} /* end ns PwrNlp */

#endif // PWRNLP_EXCEPTION_H
