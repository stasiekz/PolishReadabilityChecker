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

#ifndef LIBWCCL_OPS_FUNCTIONS_TSET_GETSYMBOLSINRANGE_H
#define LIBWCCL_OPS_FUNCTIONS_TSET_GETSYMBOLSINRANGE_H

#include <libwccl/values/tset.h>
#include <libwccl/values/position.h>
#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Operator that gets tagset symbols from tokens in given range.
 */
class GetSymbolsInRange : public Function<TSet>
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;

	GetSymbolsInRange(
			const Corpus2::Tag& mask,
			const PosFunctionPtr& range_begin_expr,
			const PosFunctionPtr& range_end_expr)
		: mask_(mask),
		  rbegin_expr_(range_begin_expr),
		  rend_expr_(range_end_expr)
	{
		BOOST_ASSERT(rbegin_expr_);
		BOOST_ASSERT(rend_expr_);
	}

	/**
	 * @returns String representation of the function in the form of:
	 * "range(tagset_symbol, range_begin_expr, range_end_expr)
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns The operator name, "range"
	 */
	std::string raw_name() const {
		return "range";
	}

protected:
	Corpus2::Tag mask_;

	const PosFunctionPtr rbegin_expr_;
	const PosFunctionPtr rend_expr_;

	/**
	 * Gets positions for beginning and end of range we are
	 * interested in (from corresponding argument expressions).
	 * The range is trimmed to boundaries of the sentence we are working on.
	 * An empty set is returned if any of the positions points
	 * to nowhere, or when the range doesn't overlap with the sentence,
	 * or when supplied begin is actually after the supplied end.
	 * Otherwise we have a valid range, and a symbol set is returned,
	 * which is the sum of tagset symbols for words within the range
	 * (inclusive). The selection of symbols is based on the mask.
	 * The main intention is to supply masks that correspond
	 * directly to a single selected attribute, but the code accepts any
	 * valid mask (i.e. any combination of attributes and their values,
	 * and even includes the part-of-speech part).
	 * @returns A tagset symbol set for the words within range, given
	 * the mask, if the range is valid. An empty Tset otherwise.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of the function in the form of:
	 * "range(raw_tagset_symbol, range_begin_expr_raw, range_end_expr_raw)
	 * @note This version does not require tagset, but will be inclomplete
	 * and/or contain internal info.
	 * @returns The stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_TSET_GETSYMBOLSINRANGE_H
