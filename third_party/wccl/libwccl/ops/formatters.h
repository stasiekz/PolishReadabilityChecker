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

#ifndef LIBWCCL_OPS_FORMATTERS_H
#define LIBWCCL_OPS_FORMATTERS_H

#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Class that formats unary functions as string
 */
struct UnaryFunctionFormatter
{
	/** 
	 * @returns String representation of an unary function.
	 * It is in form of	
	 * operator_name(arg_str)	
	 * although the open and close brackets can be changed
	 * (some operators use [])
	 */
	static std::string to_string(
		const Corpus2::Tagset& tagset,
		const FunctionBase& f,
		const std::string& arg_str,
		const char* open_bracket = "(",
		const char* close_bracket = ")");

	/** 
	 * @returns String representation of an unary function.
	 * It is in form of
	 * operator_name(argument_expression_string)
	 * although the open and close brackets can be changed
	 * (some operators use [])
	 */
	static std::string to_string(
		const Corpus2::Tagset& tagset,
		const FunctionBase& f,
		const FunctionBase& arg_expr,
		const char* open_bracket = "(",
		const char* close_bracket = ")")
	{
		return to_string(
			tagset,
			f,
			arg_expr.to_string(tagset),
			open_bracket, close_bracket);
	}
};

/**
 * Class that formats binary functions as string
 */
struct BinaryFunctionFormatter
{
	/** 
	 * @returns String representation of a binary function.
	 * It is in form of
	 * operator_name(arg1_str, arg2_str)
	 */
	static std::string to_string(
		const Corpus2::Tagset& tagset,
		const FunctionBase& f,
		const std::string& arg1_str,
		const std::string& arg2_str);

	/** 
	 * @returns String representation of a binary function.
	 * It is in form of
	 * operator_name(arg1_str, arg2_expr_string)
	 */
	static std::string to_string(
		const Corpus2::Tagset& tagset,
		const FunctionBase& f,
		const std::string& arg1_str,
		const FunctionBase& arg2_expr)
	{
		return to_string(
			tagset,
			f,
			arg1_str,
			arg2_expr.to_string(tagset));
	}
	/** 
	 * @returns String representation of a binary function.
	 * It is in form of
	 * operator_name(arg1_expr_string, arg2_str)
	 */
	static std::string to_string(
		const Corpus2::Tagset& tagset,
		const FunctionBase& f,
		const FunctionBase& arg1_expr,
		const std::string& arg2_str)
	{
		return to_string(
			tagset,
			f,
			arg1_expr.to_string(tagset),
			arg2_str);
	}

	/** 
	 * @returns String representation of a binary function.
	 * It is in form of
	 * operator_name(arg1_expr_string, arg2_expr_string)
	 */
	static std::string to_string(
		const Corpus2::Tagset& tagset,
		const FunctionBase& f,
		const FunctionBase& arg1_expr,
		const FunctionBase& arg2_expr)
	{
		return to_string(
			tagset,
			f,
			arg1_expr.to_string(tagset),
			arg2_expr.to_string(tagset));
	}
};

} /* end ns Wccl */


#endif // LIBWCCL_OPS_FORMATTERS_H
