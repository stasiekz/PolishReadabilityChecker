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

#ifndef LIBWCCL_OPS_FUNCTIONS_TSET_AGRFILTER_H
#define LIBWCCL_OPS_FUNCTIONS_TSET_AGRFILTER_H

#include <libwccl/values/tset.h>
#include <libwccl/values/position.h>
#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Operator that returs a filtered set of tagset symbols that
 * describe weak agreement of tokens within specified range,
 * provided that the tokens actually are in weak agreement.
 */
class AgrFilter : public Function<TSet>
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;
	typedef boost::shared_ptr<Function<TSet> > TSetFunctionPtr;

	AgrFilter(
			const PosFunctionPtr& left_pos_expr,
			const PosFunctionPtr& right_pos_expr,
			const TSetFunctionPtr& attribs_expr,
			const TSetFunctionPtr& mask_expr,
			const boost::shared_ptr<const Corpus2::Tagset> tagset)
		: left_pos_expr_(left_pos_expr),
		  right_pos_expr_(right_pos_expr),
		  attribs_expr_(attribs_expr),
		  mask_expr_(mask_expr),
		  tagset_(tagset)
	{
		BOOST_ASSERT(left_pos_expr_);
		BOOST_ASSERT(right_pos_expr_);
		BOOST_ASSERT(attribs_expr_);
		BOOST_ASSERT(mask_expr_);
	}

	/**
	 * @returns String representation of the function in the form of:
	 * "agrflt(pos_expr, selector_expr, mask_expr)
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the operator, "agrflt".
	 */
	std::string raw_name() const {
		return "agrflt";
	}

protected:
	const PosFunctionPtr left_pos_expr_;
	const PosFunctionPtr right_pos_expr_;

	const TSetFunctionPtr attribs_expr_;
	const TSetFunctionPtr mask_expr_;

	const boost::shared_ptr<const Corpus2::Tagset> tagset_;

	/**
	 * Gets range specified via Position expressions. Trims the range
	 * to sentence boundaries if needed. Then computes
	 * the set that describes weak agreement of tokens within the range,
	 * over specified attributes, then filters that set and returns the result.
	 * @returns Tagset symbol set for tokens at specified range,
	 * describing weak agreement of tokens at that range taking into account
	 * specified attributes, after application of given filter. Empty TSet
	 * if range is invalid (either of the positions pointing Nowhere
	 * or the range is empty). Also empty TSet if tokens within the range
	 * are not in weak agreement over specified attributes.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of the function in the form of:
	 * "agrflt(pos_expr_raw_str, selector_expr_raw_str, mask_expr_raw_str)
	 * @note This version does not require tagset, but will be inclomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_TSET_AGRFILTER_H
