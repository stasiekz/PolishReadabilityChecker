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

#include <libwccl/exception.h>
#include <sstream>

namespace Wccl {

WcclError::WcclError(const std::string &what)
 : PwrNlp::PwrNlpError(what)
{
}

WcclError::~WcclError() throw()
{
}

std::string WcclError::scope() const
{
	return "Wccl";
}

FileNotFound::FileNotFound(const std::string& filename,
		const std::string& paths, const std::string& where)
	: WcclError("File not found: " + filename), filename(filename),
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

InvalidArgument::InvalidArgument(const std::string &arg_name, const std::string &reason)
	: WcclError(reason),
	  arg_name(arg_name),
	  reason(reason)
{
}

InvalidArgument::~InvalidArgument() throw()
{
}

std::string InvalidArgument::info() const
{
	std::ostringstream ss;
	ss << "Invalid argument " << arg_name << " : " << reason;
	return ss.str();
}

} /* end ns Wccl */
