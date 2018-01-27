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

#ifndef LIBWCCL_OPS_FUNCTIONS_STRSET_GETBASE_H
#define LIBWCCL_OPS_FUNCTIONS_STRSET_GETBASE_H

#include <libwccl/values/strset.h>
#include <libwccl/values/position.h>
#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Operator that takes a position, gets word pointed by the
 * position and returns the lemmas of the word.
 * Returns empty string set if position pointed outside of
 * the sentence boundaries.
 */
class GetLemmas : public Function<StrSet>
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;
	
	GetLemmas(const PosFunctionPtr& pos_expr)
		: pos_expr_(pos_expr)
	{
		BOOST_ASSERT(pos_expr_);
	}

	/**
	 * @returns String representation of the function in the form of:
	 * "base(pos_expr_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function: "base"
	 */
	std::string raw_name() const {
		return "base";
	}
protected:
	const PosFunctionPtr pos_expr_;

	/**
	 * Gets a position from the argument expression, then gets
	 * word at that position from Sentence in the SentenceContext,
	 * then gets the lemmas of the word and returns them.
	 * @returns Lemmas of the word poitned to, if position
	 * lies within boundaries of the Sentence. Empty string set otherwise.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;
	
	/**
	 * Writes raw string representation of the function in the form of:
	 * "base(pos_expr_raw_string)"
	 * @note This version does not require tagset, but may be inclomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_STRSET_GETBASE_H
