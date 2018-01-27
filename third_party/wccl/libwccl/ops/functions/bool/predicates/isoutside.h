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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISOUTSIDE_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISOUTSIDE_H

#include <libwccl/ops/functions/bool/predicate.h>
#include <libwccl/values/position.h>

namespace Wccl {

/**
 * Predicate that checks if a position is outside of the sentence boundaries
 */
class IsOutside : public Predicate
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;

	IsOutside(const PosFunctionPtr& pos_expr)
		: pos_expr_(pos_expr)
	{
		BOOST_ASSERT(pos_expr_);
	}

	/**
	 * @returns String representation of the function in the form of:
	 * "outside(arg_expr_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function: "outside"
	 */
	std::string raw_name() const {
		return "outside";
	}

protected:
	const PosFunctionPtr pos_expr_;

	/**
	 * Takes values of position from argument, and checks if it is outside of the
	 * sentence boundaries, in the given context (i.e. relative to current position)
	 * @returns True value if position is outside of the sentence boundaries, False otherwise.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of the function in the form of:
	 * "outside(arg_expr_raw_string)"
	 * @note This version doesn't require a tagset, but may be incomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& os) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISOUTSIDE_H
