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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_SETPREDICATE_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_SETPREDICATE_H

#include <boost/mpl/list.hpp>
#include <boost/mpl/count.hpp>
#include <libwccl/ops/functions/bool/predicate.h>
#include <libwccl/values/strset.h>
#include <libwccl/values/tset.h>

#include <libwccl/ops/formatters.h>

namespace Wccl {

/**
 * Abstract base class for predicates which operate on two sets
 */
template<class T>
class SetPredicate : public Predicate {
	BOOST_MPL_ASSERT(( boost::mpl::count<boost::mpl::list<StrSet, TSet>, T> ));
public:
	typedef boost::shared_ptr<Function<T> > SetFunctionPtr;

	SetPredicate(const SetFunctionPtr& set1_expr, const SetFunctionPtr& set2_expr)
		: set1_expr_(set1_expr), set2_expr_(set2_expr)
	{
		BOOST_ASSERT(set1_expr_);
		BOOST_ASSERT(set2_expr_);
	}

	/**
	 * @returns String representation of the function in form of:
	 * "name_string(arg1_expr_string, arg2_expr_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	const SetFunctionPtr set1_expr_;
	const SetFunctionPtr set2_expr_;

	/**
	 * Writes raw string representation of the function in form of:
	 * "raw_name_string(arg1_expr_raw_string, arg2_expr_raw_string)"
	 * @note This version does not require a tagset, but may be inclomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

//
// ----- Implementation -----
//

template <class T>
std::string SetPredicate<T>::to_string(const Corpus2::Tagset &tagset) const {
	return BinaryFunctionFormatter::to_string(tagset, *this, *set1_expr_, *set2_expr_);
}

template <class T>
std::ostream& SetPredicate<T>::write_to(std::ostream &os) const {
	return os << this->raw_name() << "("
			  << *this->set1_expr_ << ", "
			  << *this->set2_expr_ << ")";
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_SETPREDICATE_H
