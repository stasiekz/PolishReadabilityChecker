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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATIONS_SKIP_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATIONS_SKIP_H

#include <libwccl/ops/function.h>
#include <libwccl/values/bool.h>
#include <libwccl/values/position.h>

namespace Wccl {

/**
 * Iterative operator "skip", setting the position variable to given
 * value plus offset, having skipped positions where given predicate
 * is satisfied.
 */
class Skip : public Function<Bool>
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;
	typedef boost::shared_ptr<Function<Bool> > BoolFunctionPtr;
	Skip(
		const PosFunctionPtr& central_pos_expr,
		const VariableAccessor<Position>& iter_var_acc,
		const BoolFunctionPtr& evaluating_expr,
		int offset)
		: central_pos_expr_(central_pos_expr),
		  iter_var_acc_(iter_var_acc),
		  evaluating_expr_(evaluating_expr),
		  offset_(offset)
	{
		BOOST_ASSERT(central_pos_expr_);
		BOOST_ASSERT(evaluating_expr_);
	}

	/**
	 * @returns Name of the function: "skip".
	 */
	std::string raw_name() const {
		return "skip";
	}

	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	std::ostream& write_to(std::ostream& ostream) const;
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

private:
	const PosFunctionPtr central_pos_expr_;
	const VariableAccessor<Position> iter_var_acc_;
	const BoolFunctionPtr evaluating_expr_;
	const int offset_;

};
} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_ITERATIONS_SKIP_H
