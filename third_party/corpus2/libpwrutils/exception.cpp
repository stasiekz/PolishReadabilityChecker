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

#include <libpwrutils/exception.h>
#include <sstream>

namespace PwrNlp {

PwrNlpError::PwrNlpError(const std::string &what)
	: std::runtime_error(what)
{
}

PwrNlpError::~PwrNlpError() throw()
{
}

std::string PwrNlpError::info() const
{
	return what();
}

std::string PwrNlpError::scope() const
{
	return "general";
}

} /* end ns PwrNlp */
