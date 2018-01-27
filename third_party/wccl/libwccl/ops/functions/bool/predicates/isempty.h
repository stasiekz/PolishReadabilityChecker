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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISEMPTY_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISEMPTY_H

#include <libwccl/ops/functions/bool/predicate.h>
#include <libwccl/ops/formatters.h>

namespace Wccl {

class StrSet;
class TSet;
class Match;

/**
 * Predicate that checks for emptiness of a Value type (works
 * for Match, TSet, StrSet).
 */
template <class T>
class IsEmpty : public Predicate
{
	BOOST_MPL_ASSERT(( boost::mpl::count<boost::mpl::list<StrSet, TSet, Match>, T> ));
public:
	typedef boost::shared_ptr<Function<T> > ArgFunctionPtr;

	IsEmpty(const ArgFunctionPtr& arg_expr)
		: arg_expr_(arg_expr)
	{
		BOOST_ASSERT(arg_expr_);
	}

	/**
	 * @returns String representation of the function
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function
	 */
	std::string raw_name() const {
		return "empty";
	}

protected:
	const ArgFunctionPtr arg_expr_;

	/**
	 * Take value of argument and return True if it is empty, False otherwise.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of the function
	 * @note This version does not require tagset but may be incomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};


//
// ----- Implementation -----
//

template <class T>
FunctionBase::BaseRetValPtr IsEmpty<T>::apply_internal(const FunExecContext& context) const {	
	return Predicate::evaluate(this->arg_expr_->apply(context)->empty(), context);
}

template <class T>
std::string IsEmpty<T>::to_string(const Corpus2::Tagset &tagset) const {
	return UnaryFunctionFormatter::to_string(tagset, *this, *arg_expr_);
}

template <class T>
std::ostream& IsEmpty<T>::write_to(std::ostream &os) const {
	return os << this->raw_name() << "(" << *this->arg_expr_ << ")";
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISEMPTY_H
