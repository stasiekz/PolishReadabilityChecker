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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATION_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATION_H

#include <libwccl/ops/function.h>
#include <libwccl/values/bool.h>
#include <libwccl/values/position.h>

namespace Wccl {

/**
 * Abstract base class for a class of operators that iterate over a range.
 */
class Iteration : public Function<Bool>
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;
	typedef boost::shared_ptr<Function<Bool> > BoolFunctionPtr;

	/**
	 * @returns String reperesentation of Iteration Operator. This is
	 * default representation of
	 * name(left_pos_expr, right_pos_expr, variable, eval_expr)
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	const PosFunctionPtr left_pos_expr_;
	const PosFunctionPtr right_pos_expr_;
	const VariableAccessor<Position> iter_var_acc_;
	const BoolFunctionPtr evaluating_expr_;
	const bool may_cross;
	
	Iteration(
		const PosFunctionPtr& left_pos_expr,
		const PosFunctionPtr& right_pos_expr,
		const VariableAccessor<Position>& iter_var_acc,
		const BoolFunctionPtr& evaluating_expr,
		bool may_cross)
		: left_pos_expr_(left_pos_expr),
		  right_pos_expr_(right_pos_expr),
		  iter_var_acc_(iter_var_acc),
		  evaluating_expr_(evaluating_expr),
		  may_cross(may_cross)
	{
		BOOST_ASSERT(left_pos_expr_);
		BOOST_ASSERT(right_pos_expr_);
		BOOST_ASSERT(evaluating_expr_);
	}

	/**
	 * Gets start and end positions from arguments to create
	 * a range of positions to iterate over using a supplied
	 * iteration variable, evaluating positions within the
	 * range using supplied evaluation function.
	 * Range is trimmed to sentence boundaries.
	 * A range is invalid when begin or end point Nowhere. Depending on the
	 * value of may_cross, a range with begin following end is treated either
	 * as invalid (may_cross == False) or valid. In case of an invalid range,
	 * False is returned and iteration variable set to Nowhere.
	 * If range is correct, return value depends on stopping condition
	 * that describes how many positions within the range have to evaluate
	 * to true. Exact details depend on type of iteration, and are
	 * specified by the subclasses via \link iterate \endlink method.
	 * If the stopping condition has been met positively,
	 * the iteration variable is set to the first position that made
	 * the condition true, and True is returned.
	 * If whole range has been iterated over without meeting the stopping
	 * condition, False is returned and iteration variable is set to
	 * Nowhere.
	 * @returns False in case of invalid range or when whole range has
	 * been iterated without meeting stop condition specified by a subclass.
	 * True is returned if the stop condition has been met.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string reperesentation of Iteration Operator. This is
	 * default representation of
	 * raw_name(raw_left_pos_expr, raw_right_pos_expr, variable, raw_eval_expr)
	 * @note This version doesn't require a Tagset, but may
	 * be incomplete and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;

	virtual bool iterate(
		int left,
		int right,
		Position &p,
		const FunExecContext &context) const = 0;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATION_H
