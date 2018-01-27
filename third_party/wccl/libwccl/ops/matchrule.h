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

#ifndef LIBWCCL_OPS_MATCHRULE_H
#define LIBWCCL_OPS_MATCHRULE_H

#include <libwccl/ops/parsedexpression.h>
#include <libcorpus2/ann/annotatedsentence.h>

namespace Wccl {
class SentenceContext;

namespace Matching {
class ApplyOperator;

class MatchRule : public ParsedExpression
{
public:
	MatchRule(
		const Variables& variables,
		const boost::shared_ptr<ApplyOperator>& apply);
	/**
	 * Executes the underlying apply() operator.
	 * @param sentence_context SentenceContext of the Sentence to execute the Rule on.
	 * @see apply() - equivalent method; the \link operator()() operator() \endlink allows
	 * more convenient functional notation, however if you only have a pointer
	 * you might prefer the execute method as shown below. The choice is yours.
	 * \code
	 * match_rule(s);
	 * // versus
	 * match_rule.apply(s);
	 * // or if you have a pointer...
	 * (*match_rule_ptr)(s);
	 * // versus
	 * match_rule_ptr->execute(s);
	 * \endcode
	 */
	void operator()(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s);

	/**
	 * Executes the underlying apply() operator.
	 * @param sentence_context SentenceContext of the Sentence to execute the Rule on.
	 * @see \link operator()() operator() \endlink - an equivalent of this method that allows
	 * functional notation, treating Rule directly as a function object
	 */
	void apply(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s);

	/**
	 * Makes a copy of the MatchRule, which has its own space for Variables.
	 * This allows the copy to be run concurrently (otherwise methods
	 * of this class are not thread-safe; concurrent executions
	 * on the same object would share Variables so don't do that).
	 * @returns A copy of the MatchRule, including copy of Variables' values.
	 * @note The values of the Variables are copied as they are, which is
	 * a usable feature. If that is not what you want, use clone_clean()
	 * or call clean() when you need it.
	 * @see clone_clean - a convenience version that returns a copy with
	 * all Variables set to their default values.
	 * @see clone_ptr - a version that returns a copy wrapped in a shared
	 * pointer
	 * @see clone_clean_ptr - a version that returns a copy wrapped
	 * in a shared pointer, and also having its Variables cleaned.
	 * @see \link MatchRule(const MatchRule&, bool) copy constructor \endlink -
	 * another way to create a copy
	 * @see \link operator=(const MatchRule&) operator= \endlink - assignment
	 * operator, yet another way to create a copy.
	 */
	MatchRule clone() const;

	/**
	 * Makes a copy of the MatchRule, which has its own space for Variables.
	 * This allows the copy to be run concurrently (otherwise methods
	 * of this class are not thread-safe; concurrent executions
	 * on the same object would share Variables so don't do that).
	 * All the Variables in the copy will have default values.
	 * @returns A copy of the MatchRule with Variables that have default
	 * values.
	 * @see clone - a version that returns a copy without cleaning Variables.
	 * @see clone_ptr - a version that returns a copy wrapped in a shared
	 * pointer
	 * @see clone_clean_ptr - a version that returns a copy wrapped
	 * in a shared pointer, and also having its Variables cleaned.
	 */
	MatchRule clone_clean() const;

	/**
	 * @returns A copy of the expression, with values of the variables
	 * copied as well.
	 * @see clone_clean_ptr - convenience version that returns clean copy.
	 */
	boost::shared_ptr<MatchRule> clone_ptr() const;

	/**
	 * @returns A copy of the expression, with values of the variables
	 * set to their defaults.
	 * @see clone_ptr - a version that keeps values of the variables.
	 */
	boost::shared_ptr<MatchRule> clone_clean_ptr() const;

	/**
	 * Copy constructor, creates a copy with new set of values for
	 * variables, allowing concurrent run of semantically same Rule
	 * (running same physical MatchRule object is inherently not thread safe).
	 * @param other - the MatchRule to copy.
	 * @param clean - true if copy needs to have its variables
	 * set to default values, false to keep the values; false is default
	 */
	MatchRule(const MatchRule& other, bool clean = false);

	/**
	 * Default constructor. Produces MatchRule that is a no-op.
	 */
	MatchRule();

	/**
	 * Assignment operator to create a workable copy of other MatchRule
	 * (workable means it has a new set of values for variables, allowing
	 * concurrent run of semantically same MatchRule; running same physical
	 * MatchRule object is inherently not thread safe).
	 * @note The values for variables are kept as in the original Rule.
	 * @param other - MatchRule to copy
	 * @returns A reference to the current object, after assignment.
	 */
	MatchRule& operator=(const MatchRule& other);

	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	MatchRule* clone_internal() const;
	std::ostream& write_to(std::ostream& ostream) const;

private:
	boost::shared_ptr<ApplyOperator> apply_;
};


//
//--- implementation details ---
//
inline
MatchRule::MatchRule(
		const Variables& variables,
		const boost::shared_ptr<ApplyOperator>& apply) 
	: ParsedExpression(variables),
	  apply_(apply)
{
	BOOST_ASSERT(apply_);
}

inline
void MatchRule::operator()(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) {
	return apply(s);
}

inline
MatchRule* MatchRule::clone_internal() const {
	return new MatchRule(*variables_, apply_);
}

inline
MatchRule::MatchRule(const MatchRule &other, bool clean)
	: ParsedExpression(*other.variables_),
	  apply_(other.apply_)
{
	BOOST_ASSERT(apply_);
	if(clean) {
		this->clean();
	}
}

inline
MatchRule MatchRule::clone() const {
	return *this;
}

inline
MatchRule MatchRule::clone_clean() const {
	return MatchRule(*this, true);
}

inline
MatchRule& MatchRule::operator=(const MatchRule& other) {
	apply_ = other.apply_;
	variables_.reset(other.variables_->clone());
	return *this;
}

inline
MatchRule::MatchRule()
	: ParsedExpression((Variables())),
	  apply_()
{
}

inline
boost::shared_ptr<MatchRule> MatchRule::clone_ptr() const {
	return boost::shared_ptr<MatchRule>(clone_internal());
}

inline
boost::shared_ptr<MatchRule> MatchRule::clone_clean_ptr() const {
	boost::shared_ptr<MatchRule> copy(clone_internal());
	BOOST_ASSERT(copy);
	copy->clean();
	return copy;
}

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCHRULE_H
