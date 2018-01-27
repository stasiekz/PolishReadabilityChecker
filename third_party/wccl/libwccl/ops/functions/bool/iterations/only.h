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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATIONS_ONLY_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATIONS_ONLY_H

#include <libwccl/ops/functions/bool/iteration.h>

namespace Wccl {

/**
 * Iterative operator "only", which mandates that
 * evaluating expression should evaluate to true
 * on all positions in range OR the range should
 * be empty.
 */
class Only : public Iteration
{
public:
	Only(
		const PosFunctionPtr& left_pos_expr,
		const PosFunctionPtr& right_pos_expr,
		const VariableAccessor<Position>& iter_var_acc,
		const BoolFunctionPtr& evaluating_expr)
		: Iteration(left_pos_expr, right_pos_expr, iter_var_acc,
					evaluating_expr, true)
	{
	}

	/**
	 * @returns Name of the function: "only".
	 */
	std::string raw_name() const {
		return "only";
	}

protected:
	/**
	 * @returns True, if for all positions within range
	 * the evaluating expression returned true. False
	 * otherwise.
	 * @note Upon success, the iteration variable is
	 * set to the last position in range (that is the
	 * position that made the stop condition true in this case).
	 */ 
	bool iterate(
		int left,
		int right,
		Position &p,
		const FunExecContext &context) const;
};


} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATIONS_ONLY_H
