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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATIONS_LEFTLOOK_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATIONS_LEFTLOOK_H

#include <libwccl/ops/functions/bool/iteration.h>

namespace Wccl {

/**
 * Iterative operator "llook", which looks for the rightmost position
 * in a range that makes the evaluating expression return true, i.e.
 * it "looks to the left".
 */
class LeftLook : public Iteration
{
public:
	LeftLook(
		const PosFunctionPtr& left_pos_expr,
		const PosFunctionPtr& right_pos_expr,
		const VariableAccessor<Position>& iter_var_acc,
		const BoolFunctionPtr& evaluating_expr)
		: Iteration(left_pos_expr, right_pos_expr, iter_var_acc,
					evaluating_expr, false) // false==no crossing ranges
	{
	}

	/**
	 * @returns Name of the function: "llook".
	 */
	std::string raw_name() const {
		return "llook";
	}

	/**
	 * @returns String reperesentation of LeftLook Operator:
	 * llook(right_pos_expr, left_pos_expr, variable, eval_expr, min_matches)
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	/**
	 * @returns True if, when scanning right-to-left,
	 * a position within the range is found that makes
	 * the evaluating function return true. False
	 * otherwise.
	 * @note Upon success, the iteration variable is
	 * set to the rightmost position in range
	 * that has made evaluation function return true
	 * (that is the position that made the stop condition
	 * true in this case).
	 */ 
	bool iterate(
		int left,
		int right,
		Position &p,
		const FunExecContext &context) const;

	/**
	 * Writes raw string reperesentation of LeftLook operator, in form of
	 * llook(raw_right_p_expr, raw_left_p_expr, var, raw_eval_expr, min_matches)
	 * @note This version doesn't require a Tagset, but may
	 * be incomplete and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};


} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATIONS_LEFTLOOK_H
