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

#ifndef LIBWCCL_OPS_FUNCTIONS_CONDITIONAL_H
#define LIBWCCL_OPS_FUNCTIONS_CONDITIONAL_H

#include <boost/format.hpp>
#include <libwccl/ops/functions/constant.h>

namespace Wccl {

/**
 * Template class for conditional operators, returning value
 * depending on evaluation of some predicate.
 * This class is targeted towards if..then..else expression
 */
template<class T>
class Conditional : public Function<T>
{
public:
	typedef boost::shared_ptr<const Function<T> > ArgFunctionPtr;
	typedef boost::shared_ptr<const Function<Bool> > BoolFunctionPtr;
	
	Conditional(
		const BoolFunctionPtr& cond_expr,
		const ArgFunctionPtr& iftrue_expr,
		const ArgFunctionPtr& iffalse_expr = Default())
		: cond_expr_(cond_expr), iftrue_expr_(iftrue_expr), iffalse_expr_(iffalse_expr)
	{
		BOOST_ASSERT(cond_expr_);
		BOOST_ASSERT(iftrue_expr_);
		BOOST_ASSERT(iffalse_expr_);
	}

	/**
	 * String representation of conditional operator in form of:
	 * "if(cond_expr_string, iftrue_expr_string, iffalse_expr_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function: "if"
	 */
	std::string raw_name() const {
		return "if";
	}

protected:
	const BoolFunctionPtr cond_expr_;
	const ArgFunctionPtr iftrue_expr_;
	const ArgFunctionPtr iffalse_expr_;

	/**
	 * Evaluate the predicate. If it is true, evaluate and return value of
	 * iftrue_expression. If predicate is false, evalute and return value
	 * of iffalse_expression.
	 */
	FunctionBase::BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of conditional operator in form of:
	 * "if(cond_expr_raw_s, iftrue_expr_raw_s, iffalse_expr_raw_s)"
	 * This version does not require tagset, but may be inclomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	virtual std::ostream& write_to(std::ostream& ostream) const;

private:
	static const ArgFunctionPtr Default() {
		return detail::DefaultFunction<T>();
	}
};

/**
 * Template class for conditional operator targeted
 * towards the operator "? if_true_value ? predicate"
 * Difference between base Conditional<T> is that
 * the if_false_value is always default,
 * and string representation is different.
 */
template<class T>
class ConditionalOp : public Conditional<T>
{
public:
	typedef typename Conditional<T>::ArgFunctionPtr ArgFunctionPtr;
	typedef typename Conditional<T>::BoolFunctionPtr BoolFunctionPtr;

	ConditionalOp(
		const BoolFunctionPtr& cond_expr,
		const ArgFunctionPtr& iftrue_expr)
		: Conditional<T>(cond_expr, iftrue_expr)
	{
	}

	/**
	 * @returns String representation of conditional operator in form of:
	 * "? if_true_expr_string ? cond_expr_string"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function: "?".
	 */
	std::string raw_name() const {
		return "?";
	}

protected:
	/**
	 * Writes String representation of conditional operator in form of:
	 * "? if_true_expr_raw_string ? cond_expr_raw_string"
	 * @note This version does not require tagset, but may be inclomplete
	 * and/or contain internal info.
	 * @returns Stream written to
	 */
	virtual std::ostream& write_to(std::ostream& ostream) const;
};


//
// ----- Implementation -----
//

template<class T>
FunctionBase::BaseRetValPtr Conditional<T>::apply_internal(const FunExecContext& context) const {
	if(this->cond_expr_->apply(context)->get_value()) {
		return this->iftrue_expr_->apply(context);
	}
	return this->iffalse_expr_->apply(context);
}

template<class T>
std::ostream& Conditional<T>::write_to(std::ostream& os) const {
	return os << raw_name() << "("
			  << *this->cond_expr_ << ", "
			  << *this->iftrue_expr_ << ", "
			  << *this->iffalse_expr_ << ")";
}

template<class T>
std::string Conditional<T>::to_string(const Corpus2::Tagset &tagset) const {
	std::stringstream ss;
	ss << boost::format("%1%(%2%, %3%, %4%)")
			% this->name(tagset)
			% this->cond_expr_->to_string(tagset)
			% this->iftrue_expr_->to_string(tagset)
			% this->iffalse_expr_->to_string(tagset);
	return ss.str();
}

template<class T>
std::ostream& ConditionalOp<T>::write_to(std::ostream& os) const {
	return os << this->raw_name() << " " << *this->iftrue_expr_ << " ? " << *this->cond_expr_;
}

template<class T>
std::string ConditionalOp<T>::to_string(const Corpus2::Tagset &tagset) const
{
	std::stringstream ss;
	ss << boost::format("%1% %2% ? %3%")
			% this->name(tagset)			
			% this->iftrue_expr_->to_string(tagset)
			% this->cond_expr_->to_string(tagset);
	return ss.str();
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_CONDITIONAL_H
