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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ANNTOKEN_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ANNTOKEN_H

#include <libwccl/ops/functions/bool/predicate.h>

namespace Wccl {

/**
 * An annotation-checking predicate: checks whether the given position belongs
 * to any annotation in the given channel. If ann constraint provided, will
 * return True only if the additional constraint holds: token must be
* annotation's head, must be annotation-first or annotation-last.
 */
class AnnToken : public Predicate
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;

	enum TokenConstraint
	{
		O_ANY = 0,   // no constraints
		O_HEAD = 1,  // token must be ann head
		O_FIRST = 2, // must be ann-first
		O_LAST = 3,  // must be ann-last
	};

	AnnToken(
		const PosFunctionPtr& pos_expr,
		const std::string& annotation_name,
		const TokenConstraint tok_constr = O_ANY)
		: pos_expr_(pos_expr),
		  chan_name_(annotation_name),
		  tok_constr_(tok_constr)
	{
		BOOST_ASSERT(pos_expr_);
		BOOST_ASSERT(!annotation_name.empty());
	}

	/**
	 * @returns Name of the function
	 */
	std::string raw_name() const {
		return "isannpart";
	}

	/**
	 * @returns String representation of the predicate
	 */
	std::string to_string(const Corpus2::Tagset& /* tagset */) const;

protected:
	const PosFunctionPtr pos_expr_;
	const std::string chan_name_;
	const TokenConstraint tok_constr_;

	/**
	 * Returns whether the given position belongs to an annotation in
	 * a channel of the given name.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes string representation of the predicate to
	 * an output stream.
	 * @returns Stream written to.
	 * @note May be incomplete and/or containt internal info.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ANN_H
