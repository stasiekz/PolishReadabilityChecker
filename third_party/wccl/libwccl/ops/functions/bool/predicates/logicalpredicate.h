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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_LOGICALPREDICATE_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_LOGICALPREDICATE_H

#include <vector>

#include <libwccl/ops/functions/bool/predicate.h>

namespace Wccl {
/**
 * Abstract base class for predicates which are logical functions
 * (over any number of arguments)
 */
class LogicalPredicate : public Predicate
{
public:
	typedef boost::shared_ptr<Function<Bool> > BoolFunctionPtr;
	typedef std::vector<BoolFunctionPtr> BoolFunctionPtrVector;

	LogicalPredicate(const boost::shared_ptr<BoolFunctionPtrVector>& expressions)
		: expressions_(expressions)
	{
		BOOST_ASSERT(expressions_);
		BOOST_ASSERT(expressions_->size() > 0);
	}

	/**
	 * @returns String representation of the logical predicate, realised by default
	 * as "operator_name(expr1_string, ..., exprn_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	const boost::shared_ptr<BoolFunctionPtrVector> expressions_;
	
	/**
	 * Writes raw string representation of the logical predicate, realised by default
	 * as "raw_operator_name(raw_expr1_string, ..., raw_exprn_string)"
	 * @note This version doesn't require tagset, but may be incomplete and/or
	 * contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_LOGICALPREDICATE_H
