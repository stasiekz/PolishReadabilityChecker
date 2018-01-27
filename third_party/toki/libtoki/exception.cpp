/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <libtoki/exception.h>
#include <sstream>

namespace Toki {

TokiError::TokiError(const std::string &what)
	: PwrNlp::PwrNlpError(what)
{
}

TokiError::~TokiError() throw()
{
}

std::string TokiError::scope() const
{
	return "Toki";
}

FileNotFound::FileNotFound(const std::string& filename,
		const std::string& paths, const std::string& where)
	: TokiError("File not found: " + filename), filename(filename),
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

} /* end ns Toki */
