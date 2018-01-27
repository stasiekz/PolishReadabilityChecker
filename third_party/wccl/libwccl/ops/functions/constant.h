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

#ifndef LIBWCCL_OPS_FUNCTIONS_CONSTANT_H
#define LIBWCCL_OPS_FUNCTIONS_CONSTANT_H

#include <boost/concept_check.hpp>

#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Functional realisation of constant value of a given type
 */
template<class T>
class Constant : public Function<T>
{
	BOOST_CONCEPT_ASSERT((boost::CopyConstructible<T>));
public:
	/*
	 * Constant function holds specific value to return when applying it
	 */
	Constant(const T& value)
		: value_(new T(value))
	{
		BOOST_ASSERT(value_);
	}

	/**
	 * @returns Name of the function: "const".
	 */
	std::string raw_name() const {
		return "const";
	}

	/**
	 * @returns String representation of the Constant, which is the
	 * the string representation of the underlying Value.
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	/**
	 * @returns String representation of the Constant that does
	 * not require a tagset. It is same as raw string representation
	 * of underlying Value.
	 * @note May be incomplete and/or containt internal info.
	 */
	std::ostream& write_to(std::ostream& ostream) const;

	/**
	 * @returns Underlying constant Value.
	 */
	FunctionBase::BaseRetValPtr apply_internal(const FunExecContext&) const;

private:
	const boost::shared_ptr<const T> value_;
};

namespace detail {

/**
 * Helper function creating a default Function<T> object - a Constant<T>
 * that is returning default Value of given type T.
 */
template<class T>
static const boost::shared_ptr<const Function<T> > DefaultFunction() {
	static boost::shared_ptr<Function<T> > default_fun(new Constant<T>((T())));
	return default_fun;
}

}

//
// ----- Implementation -----
//

template <class T>
std::string Constant<T>::to_string(const Corpus2::Tagset& tagset) const {
	return value_->to_string(tagset);
}

template <class T>
FunctionBase::BaseRetValPtr Constant<T>::apply_internal(const FunExecContext&) const {
	return value_;
}

template <class T>
std::ostream& Constant<T>::write_to(std::ostream& ostream) const {
	return ostream << value_->to_raw_string();
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_CONSTANT_H
