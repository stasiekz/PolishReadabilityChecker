/*
    Copyright (C) 2011 Adam Wardyński, Tomasz Śniatowski, Paweł Kędzia,
    Adam Radziszewski, Bartosz Broda
    Part of the WCCL project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBWCCL_EXCEPTION_H
#define LIBWCCL_EXCEPTION_H

#include <libpwrutils/exception.h>

namespace Wccl {

/**
 * Base class for all Wccl errorss. Derives from
 * @c std::runtime_error. Call member function @c what to get a
 * human-readable message associated with the error.
 */
class WcclError : public PwrNlp::PwrNlpError
{
public:
	/**
	 * Instantiate a WcclError instance with the given message.
	 * @param what The message to associate with this error.
	 */
	WcclError(const std::string &what);

	~WcclError() throw();

	/// PwrNlpError override
	std::string scope() const;
};

class FileNotFound : public WcclError
{
public:
	FileNotFound(const std::string& filename, const std::string& paths,
			const std::string& where);

	~FileNotFound() throw();

	std::string info() const;

	std::string filename, paths, where;
};

class InvalidArgument : public WcclError
{
public:
	InvalidArgument(const std::string& arg_name, const std::string& reason);

	~InvalidArgument() throw();

	std::string info() const;

	std::string arg_name;
	std::string reason;
};

} /* end ns Wccl */

#endif // LIBWCCL_EXCEPTION_H
