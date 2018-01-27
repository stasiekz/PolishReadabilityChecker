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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_INTERSECTS_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_INTERSECTS_H

#include <libwccl/ops/functions/bool/predicates/setpredicate.h>

namespace Wccl {

/**
 * Class that realises a predicate checking if one set intersects with another
 */
template <class T>
class Intersects : public SetPredicate<T>
{
public:
	typedef typename SetPredicate<T>::SetFunctionPtr SetFunctionPtr;

	Intersects(const SetFunctionPtr& set1_expr, const SetFunctionPtr& set2_expr)
		: SetPredicate<T>(set1_expr, set2_expr)
	{
	}

	/**
	 * @returns Name of the function: "inter"
	 */
	std::string raw_name() const {
		return "inter";
	}

protected:
	/**
	 * Take values for both sets and return True if they intersect,
	 * False otherwise.
	 */
	FunctionBase::BaseRetValPtr apply_internal(const FunExecContext& context) const;

};


//
// ----- Implementation -----
//

template <class T>
FunctionBase::BaseRetValPtr Intersects<T>::apply_internal(const FunExecContext& context) const {
	const boost::shared_ptr<const T>& set1 = this->set1_expr_->apply(context);
	const boost::shared_ptr<const T>& set2 = this->set2_expr_->apply(context);
	return Predicate::evaluate(set1->intersects(*set2), context);
}

} /* end ns Wccl */


#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_INTERSECTS_H
