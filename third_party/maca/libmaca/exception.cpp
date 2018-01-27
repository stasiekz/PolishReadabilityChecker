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

#include <libmaca/exception.h>
#include <sstream>
#include <libmaca/util/settings.h>

namespace Maca {

MacaError::MacaError(const std::string &what)
 : PwrNlp::PwrNlpError(what)
{
}

MacaError::~MacaError() throw()
{
}

std::string MacaError::scope() const
{
	return "Maca";
}

FileNotFound::FileNotFound(const std::string& filename,
		const std::string& paths, const std::string& where)
	: MacaError("File not found: " + filename), filename(filename),
	paths(paths), where(where)
{
}

FileNotFound::~FileNotFound() throw()
{
}

std::string FileNotFound::info() const
{
	std::ostringstream ss;
	if (where.empty()) {
		ss << "File ";
	} else {
		ss << where << " file ";
	}
	ss << "'" << filename << "' not found in search path " << paths;
	return ss.str();
}

} /* end ns Maca */
