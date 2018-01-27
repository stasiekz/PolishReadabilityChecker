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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_VARSETTER_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_VARSETTER_H

#include <libwccl/ops/functions/bool/predicate.h>
#include <libwccl/ops/formatters.h>

namespace Wccl {

/**
 * Operator that sets value of a variable of given type T
 * and returns True.
 */
template<class T>
class VarSetter : public Function<Bool>
{
public:
	typedef typename boost::shared_ptr<Function<T> > ArgFunctionPtr;

	VarSetter(const VariableAccessor<T>& var_acc, const ArgFunctionPtr& arg_expr)
		: var_acc_(var_acc),
		  arg_expr_(arg_expr)
	{
		BOOST_ASSERT(arg_expr_);
	}
	
	/**
	 * @returns Function name for variable setter: "setvar"
	 */
	std::string raw_name() const {
		return "setvar";
	}

	/**
	 * @returns String representation of the variable setter which is
	 * setvar(var_repr, arg_raw_expr_str)
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	/**
	 * Evaluate argument expression and assign the result to underlying variable.
	 * @returns True.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of the variable setter which is
	 * setvar(var_repr, arg_expr_str)
	 * @note This version does not require a tagset, but may be incomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;

private:
	const VariableAccessor<T> var_acc_;
	const ArgFunctionPtr arg_expr_;
};


//
// ----- Implementation -----
//

template <class T>
FunctionBase::BaseRetValPtr VarSetter<T>::apply_internal(const Wccl::FunExecContext &context) const {
	context.variables()->get_fast(var_acc_)->set_value(arg_expr_->apply(context)->get_value());
	return Predicate::True(context);
}

template <class T>
std::string VarSetter<T>::to_string(const Corpus2::Tagset &tagset) const {
	return BinaryFunctionFormatter::to_string(
		tagset,
		*this,
		T::var_repr(var_acc_.get_name()),
		*arg_expr_);
}

template <class T>
std::ostream& VarSetter<T>::write_to(std::ostream &os) const {
	return os << this->raw_name() << "("
			  << T::var_repr(var_acc_.get_name()) << ", "
			  << *this->arg_expr_ << ")";
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_VARSETTER_H
