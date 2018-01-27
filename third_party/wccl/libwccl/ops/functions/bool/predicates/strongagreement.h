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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATE_STRONGAGREEMENT_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATE_STRONGAGREEMENT_H

#include <libwccl/ops/functions/bool/predicate.h>
#include <libwccl/values/position.h>
#include <libwccl/values/tset.h>

namespace Wccl {

/**
 * Predicate checking strong agreement on a range of tokens.
 */
class StrongAgreement : public Predicate
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;
	typedef boost::shared_ptr<Function<TSet> > TSetFunctionPtr;

	StrongAgreement(
		const PosFunctionPtr& left_pos_expr,
		const PosFunctionPtr& right_pos_expr,
		const TSetFunctionPtr& attribs_expr,
		const boost::shared_ptr<const Corpus2::Tagset> tagset)
		: left_pos_expr_(left_pos_expr),
		  right_pos_expr_(right_pos_expr),
		  attribs_expr_(attribs_expr),
		  tagset_(tagset)
	{
		BOOST_ASSERT(left_pos_expr_);
		BOOST_ASSERT(right_pos_expr_);
		BOOST_ASSERT(attribs_expr);
	}

	/**
	 * @returns Name of the function: "agr"
	 */
	std::string raw_name() const {
		return "agr";
	}

	/**
	 * @returns String reperesentation of the predicate in form of
	 * "agr(pos1_expr, pos2_expr, attrib_expr)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	const PosFunctionPtr left_pos_expr_;
	const PosFunctionPtr right_pos_expr_;
	const TSetFunctionPtr attribs_expr_;
	const boost::shared_ptr<const Corpus2::Tagset> tagset_;

	/**
	 * Gets start and end positions for range from arguments.
	 * Range is trimmed to sentence boundaries.
	 * If either of the positions is Nowhere, or range is empty, False
	 * is returned.
	 * Otherwise, strong agreement for specified attributes is checked
	 * on the specified range of tokens and result of this test is returned.
	 * @returns True if the tokens in specified range are in strong
	 * agreement for given attributes. False otherwise, including cases
	 * of empty range or either of the positions being Nowhere.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string reperesentation of the predicate in form of
	 * "agr(pos1_raw_expr, pos2_raw_expr, attribs_raw_expr)"
	 * @note This version doesn't require a Tagset, but will
	 * be incomplete and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;

};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATE_STRONGAGREEMENT_H
