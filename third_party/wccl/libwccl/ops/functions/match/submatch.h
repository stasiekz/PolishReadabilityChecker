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

#ifndef LIBWCCL_OPS_FUNCTIONS_MATCH_SUBMATCH_H
#define LIBWCCL_OPS_FUNCTIONS_MATCH_SUBMATCH_H

#include <libwccl/ops/function.h>
#include <libwccl/values/match.h>


namespace Wccl {

/**
 * Operator that takes a Match and an index and returns 
 * a submatch at given index (indices start at 1; operator
 * works only on Matches that have MatchVector).
 */
class Submatch : public Function<Match>
{
public:
	typedef boost::shared_ptr<Function<Match> > MatchFunctionPtr;

	Submatch(const MatchFunctionPtr& match_expr, size_t index)
		: match_expr_(match_expr),
		  index_(index)
	{
		BOOST_ASSERT(match_expr_);
		if (index < 1) {
			throw InvalidArgument("index", "Submatch indices start from 1.");
		}
	}

	/**
	 * @returns String representation of the function
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function
	 */
	std::string raw_name() const {
		return "->";
	}

protected:
	const MatchFunctionPtr match_expr_;
	const size_t index_;

	/**
	 * Writes string representation of the function.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;

	/**
	 * Takes the value of a Match from argument expression, and returns
	 * a submatch at given index. Works only if Match has a MatchVector.
	 * @throws WcclError if given Match does not hold a MatchVector, or
	 *         if the index is outside boundaries of MatchVector.
	 * @returns Match that is in underlying MatchVector at specified index
	 *          (note - indexing starts from 1 not from 0).
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_MATCH_SUBMATCH_H
