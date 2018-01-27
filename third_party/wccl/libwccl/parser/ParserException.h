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

#ifndef LIBWCCL_PARSER_PARSEREXCEPTION_H
#define LIBWCCL_PARSER_PARSEREXCEPTION_H

#include <libwccl/exception.h>

#include <string>

class ParserException : public Wccl::WcclError
{
public:
	ParserException(const std::string);

	~ParserException() throw() { 
	}

	//
	std::string info() const { return this->msg; }

private:
	std::string msg;
};

#endif // LIBWCCL_PARSER_PARSEREXCEPTION_H
