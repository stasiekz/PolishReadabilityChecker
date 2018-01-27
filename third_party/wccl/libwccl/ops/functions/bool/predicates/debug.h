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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_DEBUG_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_DEBUG_H

#include <libwccl/ops/functions/bool/predicate.h>
#include <libwccl/values/value.h>

namespace Wccl {

/**
 * Diagnostic predicate that prints the value
 */
class DebugPrint : public Predicate
{
public:
	typedef boost::shared_ptr<FunctionBase> FunctionPtr;

	DebugPrint(const FunctionPtr& expr)
		: expr_(expr)
	{
		BOOST_ASSERT(expr_);
	}

	/**
	 * @returns String representation of the function in the form of:
	 * "inside(arg_expr_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function: "debug"
	 */
	std::string raw_name() const {
		return "debug";
	}

protected:
	const FunctionPtr expr_;

	/**
	 * Outputs the string value of the returned value
	 * @returns True
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of the function in the form of:
	 * "debug(expr)"
	 * @note This version doesn't require a tagset, but may be incomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& os) const;
};


} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_DEBUG_H
