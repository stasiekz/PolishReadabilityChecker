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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_AND_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_AND_H

#include <libwccl/ops/functions/bool/predicates/logicalpredicate.h>

namespace Wccl {

/**
 * Operator that realises "and" logical predicate
 */
class And : public LogicalPredicate
{
public:
	And(const boost::shared_ptr<BoolFunctionPtrVector>& expressions)
		: LogicalPredicate(expressions)
	{
	}

	/**
	 * @returns Name of the function: "and"
	 */
	std::string raw_name() const {
		return "and";
	}

protected :
	/**
	 * "And" predicate evaluates each expression from left to right,
     * and returns False once an expression evaluating to False is found.
     * If all expressions were True, it returns True.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

};

} /* end ns Wccl */
#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_AND_H
