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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_EQUALS_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_EQUALS_H

#include <libwccl/ops/functions/bool/predicate.h>
#include <libwccl/ops/formatters.h>

namespace Wccl {

/**
 * Helper template class to factor out equality comparison
 * (namely for special treatment of Position). By default,
 * T::equals(const T& other) is used.
 */
template <class T>
class EqualityComparer
{
public:
	static boost::shared_ptr<const Value> apply(
			const T& arg1,
			const T& arg2,
			const FunExecContext& context)
	{
		return Predicate::evaluate(arg1.equals(arg2), context);
	}
};

/**
 * Helper template class to factor out equality comparison.
 * This is specialization for Position that uses
 * Position::equals(const Position& other, const SentenceContext& context)
 */
template <>
class EqualityComparer<Position>
{
public:
	static boost::shared_ptr<const Value> apply(
			const Position& arg1,
			const Position& arg2,
			const FunExecContext& context)
	{
		return Predicate::evaluate(
				arg1.equals(arg2, context.sentence_context()),
				context);
	}
};

/**
 * Predicate that checks for equality of values
 */
template <class T>
class Equals : public Predicate
{
public:
	typedef boost::shared_ptr<Function<T> > ArgFunctionPtr;

	Equals(const ArgFunctionPtr& arg1_expr, const ArgFunctionPtr& arg2_expr)
		: arg1_expr_(arg1_expr), arg2_expr_(arg2_expr)
	{
		BOOST_ASSERT(arg1_expr_);
		BOOST_ASSERT(arg2_expr_);
	}

	/**
	 * @returns String representation of the function in form of:
	 * "equals(arg1_expr_string, arg2_expr_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function: "equals".
	 */
	std::string raw_name() const {
		return "equal";
	}

protected:
	const ArgFunctionPtr arg1_expr_;
	const ArgFunctionPtr arg2_expr_;

	/**
	 * Take values of arguments from expressions and return True if they are equal,
	 * False otherwise.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of the function in form of:
	 * "equals(arg1_expr_raw_string, arg2_expr_raw_string)".
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
FunctionBase::BaseRetValPtr Equals<T>::apply_internal(const FunExecContext& context) const {
	return EqualityComparer<T>::apply(
		*(this->arg1_expr_->apply(context)),
		*(this->arg2_expr_->apply(context)),
		context);
}

template <class T>
std::string Equals<T>::to_string(const Corpus2::Tagset &tagset) const {
	return BinaryFunctionFormatter::to_string(tagset, *this, *arg1_expr_, *arg2_expr_);
}

template <class T>
std::ostream& Equals<T>::write_to(std::ostream &os) const {
	return os << this->raw_name() << "("
			  << *this->arg1_expr_ << ", "
			  << *this->arg2_expr_ << ")";
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_EQUALS_H
