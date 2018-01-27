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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_AMBIGUOUS_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_AMBIGUOUS_H

#include <libwccl/ops/functions/bool/predicate.h>
#include <libwccl/values/position.h>
#include <libwccl/ops/formatters.h>


namespace Wccl {

class StrSet;
class TSet;
class Position;

/**
 * Predicate that checks for ambiguity of a set type (size > 1).
 * Can also check for ambiguity of a token at a position --- equivalent
 * to checking if there is more than one possible lexeme.
 */
template <class T>
class IsAmbiguous : public Predicate
{
	BOOST_MPL_ASSERT(( boost::mpl::count<boost::mpl::list<
		StrSet, TSet, Position>, T> ));
public:
	typedef boost::shared_ptr<Function<T> > ArgFunctionPtr;

	IsAmbiguous(const ArgFunctionPtr& arg_expr)
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
		return "ambiguous";
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
inline FunctionBase::BaseRetValPtr IsAmbiguous<T>::apply_internal(const FunExecContext& context) const {
	return Predicate::evaluate(this->arg_expr_->apply(context)->size() > 1, context);
}

template <>
inline FunctionBase::BaseRetValPtr IsAmbiguous<Position>::apply_internal(const FunExecContext& context) const {
	const SentenceContext& sc = context.sentence_context();
	const boost::shared_ptr<const Position>& pos = arg_expr_->apply(context);
	if (sc.is_outside(*pos)) {
		return Predicate::False(context);
	}
	const Corpus2::Token* tok = sc.at(*pos);
	return Predicate::evaluate(tok->lexemes().size() > 1, context);
}

template <class T>
inline std::string IsAmbiguous<T>::to_string(const Corpus2::Tagset &tagset) const {
	return UnaryFunctionFormatter::to_string(tagset, *this, *arg_expr_);
}

template <class T>
inline std::ostream& IsAmbiguous<T>::write_to(std::ostream &os) const {
	return os << this->raw_name() << "(" << *this->arg_expr_ << ")";
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_AMBIGUOUS_H
