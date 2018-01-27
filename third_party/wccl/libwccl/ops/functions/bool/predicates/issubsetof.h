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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISSUBSETOF_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISSUBSETOF_H

#include <libwccl/ops/functions/bool/predicates/setpredicate.h>

namespace Wccl {

/**
 * Class that realises a predicate checking if one set is in another.
 * Almost like being a subset of, but empty set is considered not to be "in".
 */
template <class T>
class IsSubsetOf : public SetPredicate<T>
{
public:
	typedef typename SetPredicate<T>::SetFunctionPtr SetFunctionPtr;

	IsSubsetOf(const SetFunctionPtr& subset_expr, const SetFunctionPtr& set_expr)
		: SetPredicate<T>(subset_expr, set_expr)
	{
	}

	/**
	 * @returns Name of the function: "in"
	 */
	std::string raw_name() const {
		return "in";
	}

protected:
	/**
	 * Take value of possible subset in question. If it is an empty set, return False.
	 * Otherwise, take value of the set that is being compared to.
	 * @returns True if the possible subset is inside set compared to.
	 * @note Take note this is not true "subset" function, because empty
	 * set is considered not to be "in" any other set so False is returned.
	 */
	FunctionBase::BaseRetValPtr apply_internal(const FunExecContext& context) const;

};

//
// ----- Implementation -----
//

template <class T>
FunctionBase::BaseRetValPtr IsSubsetOf<T>::apply_internal(const FunExecContext& context) const {
	const boost::shared_ptr<const T>& possible_subset = this->set1_expr_->apply(context);
	if(!possible_subset->empty())
	{
		const boost::shared_ptr<const T>& set_compared_to = this->set2_expr_->apply(context);
		return Predicate::evaluate(possible_subset->is_subset_of(*set_compared_to), context);
	}
	return Predicate::False(context);
}

} /* end ns Wccl */


#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISSUBSETOF_H
