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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATE_POINTAGREEMENT_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATE_POINTAGREEMENT_H

#include <libwccl/ops/functions/bool/predicate.h>
#include <libwccl/values/position.h>
#include <libwccl/values/tset.h>

namespace Wccl {

/**
 * Predicate checking point-to-point strong agreement
 * between tokens on two positions.
 */
class PointAgreement : public Predicate
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;
	typedef boost::shared_ptr<Function<TSet> > TSetFunctionPtr;

	PointAgreement(
		const PosFunctionPtr& pos1_expr,
		const PosFunctionPtr& pos2_expr,
		const TSetFunctionPtr& attribs_expr,
		const boost::shared_ptr<const Corpus2::Tagset> tagset)
		: pos1_expr_(pos1_expr),
		  pos2_expr_(pos2_expr),
		  attribs_expr_(attribs_expr),
		  tagset_(tagset)
	{
		BOOST_ASSERT(pos1_expr_);
		BOOST_ASSERT(pos2_expr_);
		BOOST_ASSERT(attribs_expr);
	}
	/**
	 * @returns Name of the function: "agrpp"
	 */
	std::string raw_name() const {
		return "agrpp";
	}

	/**
	 * @returns String reperesentation of the predicate in form of
	 * "agrpp(pos1_expr, pos2_expr, attrib_expr)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	const PosFunctionPtr pos1_expr_;
	const PosFunctionPtr pos2_expr_;
	const TSetFunctionPtr attribs_expr_;
	const boost::shared_ptr<const Corpus2::Tagset> tagset_;

	/**
	 * Gets two positions from arguments. If either of them
	 * is Nowhere, or points outside the sentence, False is returned.
	 * Otherwise, when both positions point inside the sentence,
	 * strong agreement on specified attriutes is checked between
	 * tokens on the positions and result of this test is returned.
	 * @returns True if the tokens at specified positions are in strong
	 * agreement for specified attributes. False otherwise, including
	 * cases when either of the positions points outside the sentence.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string reperesentation of the predicate in form of
	 * "agrpp(pos1_raw_expr, pos2_raw_expr, attribs_raw_expr)"
	 * @note This version doesn't require a Tagset, but will
	 * be incomplete and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;

};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATE_POINTAGREEMENT_H
