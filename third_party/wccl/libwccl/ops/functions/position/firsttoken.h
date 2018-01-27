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

#ifndef LIBWCCL_OPS_FUNCTIONS_POSITION_FIRSTTOKEN_H
#define LIBWCCL_OPS_FUNCTIONS_POSITION_FIRSTTOKEN_H

#include <libwccl/ops/function.h>
#include <libwccl/values/match.h>

namespace Wccl {

/**
 * Operator that takes a Match and returns Position of the first token
 * within the given Match.
 */
class FirstToken : public Function<Position>
{
public:
	typedef boost::shared_ptr<Function<Match> > MatchFunctionPtr;

	FirstToken(const MatchFunctionPtr& match_expr)
		: match_expr_(match_expr)
	{
		BOOST_ASSERT(match_expr_);
	}

	/**
	 * @returns String representation of the function
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function
	 */
	std::string raw_name() const {
		return "first";
	}

protected:
	const MatchFunctionPtr match_expr_;

	/**
	 * Writes string representation of the function
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;

	/**
	 * Takes the value of a Match from argument expression, and returns
	 * a Position that corresponds to first token of the given Match.
	 * @returns Position corresponding to first token of a Match.
	 *			Nowhere in case of an empty Match.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_POSITION_FIRSTTOKEN_H
