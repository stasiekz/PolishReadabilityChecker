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

#include "exception.h"

#include <sstream>

namespace Wcrft {

WcrftError::WcrftError(const std::string &what)
	: PwrNlp::PwrNlpError(what)
{
}

WcrftError::~WcrftError() throw()
{
}

FileNotFound::FileNotFound(
		const std::string &filename,
		const std::string &paths,
		const std::string &where)
	: WcrftError("File not found: " + filename), filename(filename), paths(paths), where(where)
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

} // end namespace Wcrft
