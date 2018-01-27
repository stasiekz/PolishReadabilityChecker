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

#ifndef LIBWCCL_OPS_OPERATOR_H
#define LIBWCCL_OPS_OPERATOR_H

#include <boost/scoped_ptr.hpp>

#include <libwccl/ops/parsedexpression.h>
#include <libwccl/ops/function.h>
#include <libwccl/ops/functions/constant.h>

namespace Wccl {

/**
 * Abstract base class for WCCL expressions that are functional
 * operators
 */
class FunctionalOperator : public ParsedExpression
{
public:
	/**
	 * Applies the functional operator to given sentence context.
	 * @returns Result of the application, in terms of base type of Value.
	 */
	virtual boost::shared_ptr<const Value> base_apply(const SentenceContext& sc) = 0;

	/**
	 * @returns A copy of the expression, with values of the variables
	 * copied as well.
	 * @see clone_clean_ptr - convenience version that returns clean copy.
	 */
	boost::shared_ptr<FunctionalOperator> clone_ptr() const;

	/**
	 * @returns A copy of the expression, with values of the variables
	 * set to their defaults.
	 * @see clone_ptr - a version that keeps values of the variables.
	 */
	boost::shared_ptr<FunctionalOperator> clone_clean_ptr() const;

protected:
	explicit FunctionalOperator(const Variables& variables);

	FunctionalOperator* clone_internal() const = 0;
};


/**
 * Represents a parsed WCCL functional operator that returns a value of given type.
 * @note The class methods are not thread-safe, but you can use clone method
 * to acquire a separate copy of the Operator, and the copy can be used concurrently.
 */
template <class T>
class Operator : public FunctionalOperator
{
public:
	Operator(const boost::shared_ptr<const Function<T> >& body, const Variables& variables);

	/**
	 * Applies the functional operator to given sentence context.
	 * @returns Result of the application of this operator.
	 * @param sentence_context SentenceContext of the Sentence to apply the operator to.
	 * @note The result is conciously marked as const, so a copy of operator data
	 * is not created unless necessary.
	 * @see apply() - equivalent method; the \link operator()() operator() \endlink allows
	 * more convenient functional notation, however if you only have a pointer
	 * you might prefer the apply method as shown below. The choice is yours.
	 * \code
	 * shared_ptr<const Bool> b;
	 * b = op(sc);
	 * // versus
	 * b = op.apply(sc);
	 * // or if you have a pointer...
	 * b = (*op_ptr)(sc);
	 * // versus
	 * b = op_ptr->apply(sc);
	 * \endcode
	 * @see copy_apply() - method that creates and returns a copy of the result
	 * for you to modify as you may see fit if you need it;
	 * this is convenience method over having to create a copy yourself.
	 */
	boost::shared_ptr<const T> operator()(const SentenceContext& sentence_context);

	/**
	 * Applies the functional operator to given sentence context.
	 * @returns Result of the application of this operator.
	 * @param sentence_context SentenceContext of the Sentence to apply the operator to.
	 * @note The result is conciously marked as const, so a copy of operator data
	 * is not created unless necessary.
	 * @see \link operator()() operator() \endlink - an equivalent of this method that allows for functional
	 * notation, treating Operator directly as a function
	 * @see copy_apply - method that creates and returns a copy of the result
	 * for you to modify as you may see fit if you need it;
	 * this is convenience method over having to create a copy yourself.
	 */
	boost::shared_ptr<const T> apply(const SentenceContext& sentence_context);

	/**
	 * Applies the functional operator to given sentence context, returning pointer
	 * to a mutable Value.
	 * @returns Result of the application of this operator.
	 * @param sentence_context SentenceContext of the Sentence to apply the operator to.
	 * @see \link operator()() operator() \endlink, apply - you may still be
	 * better off using them if you expect to work on a raw value rather
	 * than a pointer, as in e.g.
	 * \code
	 * StrSet s = *op(sc);
	 * StrSet s2 = *op_ptr->apply(sc);
	 * StrSet s3 = *(*op_ptr)(sc); // included for completeness
	 * //versus
	 * shared_ptr<StrSet> s_ptr = op.copy_apply(sc);
	 * shared_ptr<StrSet> s_ptr2 = op_ptr->copy_apply(sc);
	 * \endcode
	 */
	boost::shared_ptr<T> copy_apply(const SentenceContext& sentence_context);

	/**
	 * Applies the functional operator to given sentence context.
	 * @returns Result of the application, in terms of base type of Value.
	 * @see \link operator()() operator() \endlink, apply - since you have
	 * a concrete type at hand, you probably want to use one of these
	 * two instead of this method that is inherited from FunctionalOperator
	 */
	boost::shared_ptr<const Value> base_apply(const SentenceContext& sc);

	/**
	 * Makes a copy of the Operator, having its own space for Variables.
	 * This allows the copy to be run concurrently (otherwise methods
	 * of this class are not thread-safe; concurrent executions
	 * on the same object would share Variables so don't do that).
	 * @returns A copy of the Operator, including copy of Variables' values.
	 * @note The values of the Variables are copied as they are, which is
	 * a usable feature. If that is not what you want, use clone_clean()
	 * or call clean() when you need it.
	 * @see clone_clean - a convenience version that returns a copy with
	 * all Variables set to their default values.
	 * @see clone_ptr - a version that returns a copy wrapped in a shared
	 * pointer
	 * @see clone_clean_ptr - a version that returns a copy wrapped
	 * in a shared pointer, and also having its Variables cleaned.
	 * @see \link Operator(const Operator&, bool) copy constructor \endlink -
	 * another way to create a copy
	 * @see \link operator=(const Operator&) operator= \endlink - assignment
	 * operator, yet another way to create a copy.
	 */
	Operator clone() const;

	/**
	 * Makes a copy of the Operator, having its own space for Variables.
	 * This allows the copy to be run concurrently (otherwise methods
	 * of this class are not thread-safe; concurrent executions
	 * on the same object would share Variables so don't do that).
	 * All the Variables in the copy will have default values.
	 * @returns A copy of the Operator with Variables that have default
	 * values.
	 * @see clone - a version that returns a copy without cleaning Variables.
	 * @see clone_ptr - a version that returns a copy wrapped in a shared
	 * pointer
	 * @see clone_clean_ptr - a version that returns a copy wrapped
	 * in a shared pointer, and also having its Variables cleaned.
	 */
	Operator clone_clean() const;

	/**
	 * @returns A copy of the expression, with values of the variables
	 * copied as well.
	 * @see clone_clean_ptr - convenience version that returns clean copy.
	 */
	boost::shared_ptr<Operator<T> > clone_ptr() const;

	/**
	 * @returns A copy of the expression, with values of the variables
	 * set to their defaults.
	 * @see clone_ptr - a version that keeps values of the variables.
	 */
	boost::shared_ptr<Operator<T> > clone_clean_ptr() const;

	/**
	 * Copy constructor, creates a copy with new set of values for
	 * variables, allowing concurrent run of semantically same Operator;
	 * running same physical Operator object is inherently not thread safe).
	 * @param other - the Operator to copy.
	 * @param clean - true if copy needs to have its variables
	 * set to default values, false to keep the values; false is default
	 */
	Operator(const Operator& other, bool clean = false);

	/**
	 * Default constructor. Produces operator that returns default
	 * value for given type.
	 */
	Operator();

	/**
	 * Assignment operator to create a workable copy of other Operator
	 * (workable means it has a new set of values for variables, allowing
	 * concurrent run of semantically same Operator; running same physical
	 * Operator object is inherently not thread safe).
	 * @note The values for variables are kept as in the original Operator.
	 * @param other - Operator to copy
	 * @returns A reference to the current object, after assignment.
	 */
	Operator& operator=(const Operator& other);

	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	Operator* clone_internal() const;
	std::ostream& write_to(std::ostream& ostream) const;

private:
	boost::shared_ptr<const Function<T> > function_body_;
};



//
//--- implementation details ---
//


inline
FunctionalOperator::FunctionalOperator(
		const Variables &variables)
	: ParsedExpression(variables)
{
}

inline
boost::shared_ptr<FunctionalOperator> FunctionalOperator::clone_ptr() const {
	return boost::shared_ptr<FunctionalOperator>(clone_internal());
}

inline
boost::shared_ptr<FunctionalOperator> FunctionalOperator::clone_clean_ptr() const {
	boost::shared_ptr<FunctionalOperator> copy(clone_internal());
	BOOST_ASSERT(copy);
	copy->clean();
	return copy;
}

template <class T> inline
Operator<T>::Operator(
		const boost::shared_ptr<const Function<T> >& body,
		const Variables &variables)
	: FunctionalOperator(variables),
	  function_body_(body)
{
	BOOST_ASSERT(body);
}

template <class T> inline
boost::shared_ptr<const T> Operator<T>::apply(const SentenceContext &sentence_context) {
	if(sentence_context.size() == 0) {
		throw InvalidArgument(
				"sentence_context",
				"Received an empty sentence.");
	}
	if(!sentence_context.is_current_inside()) {
		throw InvalidArgument(
				"sentence_context",
				"Current position is outside boundaries of the sentence.");
	}
	return function_body_->apply(FunExecContext(sentence_context, variables_));
}

template <class T> inline
boost::shared_ptr<const T> Operator<T>::operator()(const SentenceContext &sc) {
	return apply(sc);
}

template <class T> inline
boost::shared_ptr<T> Operator<T>::copy_apply(const SentenceContext &sc) {
	return boost::make_shared<T>(*apply(sc));
}

template <class T> inline
boost::shared_ptr<const Value> Operator<T>::base_apply(const SentenceContext &sc) {
	return apply(sc);
}

template <class T> inline
Operator<T>* Operator<T>::clone_internal() const {
	return new Operator<T>(function_body_, *variables_);
}

template <class T> inline
Operator<T>::Operator(const Operator &other, bool clean)
	: FunctionalOperator(*other.variables_),
	  function_body_(other.function_body_)
{
	BOOST_ASSERT(function_body_);
	if(clean) {
		this->clean();
	}
}

template <class T> inline
Operator<T> Operator<T>::clone() const {
	return *this;
}

template <class T> inline
Operator<T> Operator<T>::clone_clean() const {
	return Operator(*this, true);
}

template <class T> inline
Operator<T>& Operator<T>::operator=(const Operator& other) {
	BOOST_ASSERT(other.function_body_);
	BOOST_ASSERT(other.variables_);
	function_body_ = other.function_body_;
	variables_.reset(other.variables_->clone());
	return *this;
}

template <class T> inline
Operator<T>::Operator()
	: FunctionalOperator((Variables())),
	  function_body_(detail::DefaultFunction<T>())
{
	BOOST_ASSERT(function_body_);
}

template <class T> inline
boost::shared_ptr<Operator<T> > Operator<T>::clone_ptr() const {
	return boost::shared_ptr<Operator<T> >(clone_internal());
}

template <class T> inline
boost::shared_ptr<Operator<T> > Operator<T>::clone_clean_ptr() const {
	boost::shared_ptr<Operator<T> > copy(clone_internal());
	BOOST_ASSERT(copy);
	copy->clean();
	return copy;
}

template <class T> inline
std::string Operator<T>::to_string(const Corpus2::Tagset &tagset) const {
	return function_body_->to_string(tagset);
}

template<class T>
std::ostream& Operator<T>::write_to(std::ostream& os) const {
	return os << *function_body_;
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_OPERATOR_H
