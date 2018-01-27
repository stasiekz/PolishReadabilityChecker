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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_NOR_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_NOR_H

#include <libwccl/ops/functions/bool/predicates/logicalpredicate.h>

namespace Wccl {

/**
 * Operator that realises logical predicate "nor",
 * (note: the operator is called "not" in CCL)
 */
class Nor : public LogicalPredicate
{
public:
	Nor(const boost::shared_ptr<BoolFunctionPtrVector>& expressions)
		: LogicalPredicate(expressions)
	{
	}

	/**
	 * @returns Name of the function: "not"
	 */
	std::string raw_name() const {
		return "not";
	}

protected :
	/**
	 * "Nor" (aka "not") predicate evaluates expressions one by one in order
     * from left to right, and False is returned once an expression evaluating
     * to True is found.
	 * If all of the expressions were False, True is returned.
	 */
	BaseRetValPtr apply_internal(const FunExecContext&) const;

};

} /* end ns Wccl */
#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_NOR_H
