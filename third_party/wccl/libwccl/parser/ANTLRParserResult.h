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

#ifndef LIBWCCL_ANTLRPARSERRESULT_H
#define LIBWCCL_ANTLRPARSERRESULT_H

#include <boost/shared_ptr.hpp>

#include <libwccl/variables.h>
#include <libwccl/ops/function.h>


template<class T>
class ANTLRParserResult;

class ANTLRParserResultBase
{
public:
	ANTLRParserResultBase()
		: variables(new Wccl::Variables())
	{
	}

	boost::shared_ptr<Wccl::Variables> variables;

	virtual boost::shared_ptr<Wccl::FunctionBase> get_op_base() const = 0;
};

template<class T>
class ANTLRParserResult : public ANTLRParserResultBase
{
public:
	ANTLRParserResult()
	{
	}

	boost::shared_ptr<Wccl::Function<T> > op;

	boost::shared_ptr<Wccl::FunctionBase> get_op_base() const
	{
		return op;
	}
};

#endif // ANTLRPARSERRESULT_H
