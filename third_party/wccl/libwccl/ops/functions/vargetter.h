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

#ifndef LIBWCCL_OPS_FUNCTIONS_VARGETTER_H
#define LIBWCCL_OPS_FUNCTIONS_VARGETTER_H

#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Operator that returns value of a variable of given type T
 */
template<class T>
class VarGetter : public Function<T>
{
public:
	VarGetter(const VariableAccessor<T>& var_acc)
		: var_acc_(var_acc)
	{
	}
	
	/**
	 * @returns Function name for variable getter: "getvar"
	 */
	std::string raw_name() const {
		return "getvar";
	}

	/**
	 * @returns String representation of the variable getter which is
	 * equal to string representation of the underlying variable.
	 */
	std::string to_string(const Corpus2::Tagset&) const;

protected:
	/**
	 * Writes string representation of the variable getter which is
	 * equal to string representation of the underlying variable.
	 * @returns Stream written to
	 */
	std::ostream& write_to(std::ostream& ostream) const;

	/**
	 * Return value held by the underlying variable.
	 */
	FunctionBase::BaseRetValPtr apply_internal(const FunExecContext& context) const;

private:
	const VariableAccessor<T> var_acc_;
};

//
// ----- Implementation -----
//

template <class T>
FunctionBase::BaseRetValPtr VarGetter<T>::apply_internal(const FunExecContext& context) const {
	return context.variables()->get_fast(var_acc_);
}

template <class T>
std::string VarGetter<T>::to_string(const Corpus2::Tagset &) const {
	return T::var_repr(var_acc_.get_name());
}

template <class T>
std::ostream& VarGetter<T>::write_to(std::ostream& ostream) const {
	return ostream << T::var_repr(var_acc_.get_name());
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_VARGETTER_H
