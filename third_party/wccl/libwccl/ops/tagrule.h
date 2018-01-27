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

#ifndef LIBWCCL_OPS_TAGRULE_H
#define LIBWCCL_OPS_TAGRULE_H

#include <boost/scoped_ptr.hpp>

#include <libwccl/ops/parsedexpression.h>
#include <libwccl/ops/functions/constant.h>
#include <libwccl/ops/tagaction.h>

namespace Wccl {

/**
 * Represents a parsed WCCL Rule that executes some actions.
 * @note The class methods are not thread-safe, but you can use clone method
 * to acquire a separate copy of the Rule, and the copy can be used concurrently.
 */
class TagRule : public ParsedExpression
{
public:
	TagRule(
		const std::string& name,
		const Variables& variables,
		const boost::shared_ptr<const std::vector<boost::shared_ptr<TagAction> > >& actions,
		const boost::shared_ptr<const Function<Bool> >& condition = TrueCondition());

	/**
	 * Evaluates condition and if it is true, executes the actions sequentially. Does
	 * not execute any of the rules if condition is false.
	 * @returns True if any of the actions made a change, False otherwise.
	 * @param sentence_context SentenceContext of the Sentence to execute the Rule on.
	 * @see execute() - equivalent method; the \link operator()() operator() \endlink allows
	 * more convenient functional notation, however if you only have a pointer
	 * you might prefer the execute method as shown below. The choice is yours.
	 * \code
	 * Bool res;
	 * res = rule(sc);
	 * // versus
	 * res = rule.execute(sc);
	 * // or if you have a pointer...
	 * res = (*rule_ptr)(sc);
	 * // versus
	 * res = rule_ptr->execute(sc);
	 * \endcode
	 */
	Bool operator()(SentenceContext& sentence_context);

	/**
	 * Evaluates condition and if it is true, executes the actions sequentially. Does
	 * not execute any of the rules if condition is false.
	 * @returns True if any of the actions made a change, False otherwise.
	 * @param sentence_context SentenceContext of the Sentence to execute the Rule on.
	 * @see \link operator()() operator() \endlink - an equivalent of this method that allows
	 * functional notation, treating Rule directly as a function
	 */
	Bool execute(SentenceContext& sentence_context);

	/**
	 * @returns Name of the Rule.
	 */
	std::string name() const;

	/**
	 * Makes a copy of the Rule, having its own space for Variables.
	 * This allows the copy to be run concurrently (otherwise methods
	 * of this class are not thread-safe; concurrent executions
	 * on the same object would share Variables so don't do that).
	 * @returns A copy of the Rule, including copy of Variables' values.
	 * @note The values of the Variables are copied as they are, which is
	 * a usable feature. If that is not what you want, use clone_clean()
	 * or call clean() when you need it.
	 * @see clone_clean - a convenience version that returns a copy with
	 * all Variables set to their default values.
	 * @see clone_ptr - a version that returns a copy wrapped in a shared
	 * pointer
	 * @see clone_clean_ptr - a version that returns a copy wrapped
	 * in a shared pointer, and also having its Variables cleaned.
	 * @see \link Rule(const Rule&, bool) copy constructor \endlink -
	 * another way to create a copy
	 * @see \link operator=(const Rule&) operator= \endlink - assignment
	 * Rule, yet another way to create a copy.
	 */
	TagRule clone() const;

	/**
	 * Makes a copy of the Rule, having its own space for Variables.
	 * This allows the copy to be run concurrently (otherwise methods
	 * of this class are not thread-safe; concurrent executions
	 * on the same object would share Variables so don't do that).
	 * All the Variables in the copy will have default values.
	 * @returns A copy of the Rule with Variables that have default
	 * values.
	 * @see clone - a version that returns a copy without cleaning Variables.
	 * @see clone_ptr - a version that returns a copy wrapped in a shared
	 * pointer
	 * @see clone_clean_ptr - a version that returns a copy wrapped
	 * in a shared pointer, and also having its Variables cleaned.
	 */
	TagRule clone_clean() const;

	/**
	 * @returns A copy of the expression, with values of the variables
	 * copied as well.
	 * @see clone_clean_ptr - convenience version that returns clean copy.
	 */
	boost::shared_ptr<TagRule> clone_ptr() const;

	/**
	 * @returns A copy of the expression, with values of the variables
	 * set to their defaults.
	 * @see clone_ptr - a version that keeps values of the variables.
	 */
	boost::shared_ptr<TagRule> clone_clean_ptr() const;

	/**
	 * Copy constructor, creates a copy with new set of values for
	 * variables, allowing concurrent run of semantically same Rule
	 * (running same physical Rule object is inherently not thread safe).
	 * @param other - the Rule to copy.
	 * @param clean - true if copy needs to have its variables
	 * set to default values, false to keep the values; false is default
	 */
	TagRule(const TagRule& other, bool clean = false);

	/**
	 * Default constructor. Produces Rule that returns False.
	 */
	TagRule();
	/**
	 * Assignment operator to create a workable copy of other Rule
	 * (workable means it has a new set of values for variables, allowing
	 * concurrent run of semantically same Rule; running same physical
	 * Rule object is inherently not thread safe).
	 * @note The values for variables are kept as in the original Rule.
	 * @param other - Rule to copy
	 * @returns A reference to the current object, after assignment.
	 */
	TagRule& operator=(const TagRule& other);

	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	TagRule* clone_internal() const;
	std::ostream& write_to(std::ostream& ostream) const;

private:
	static const boost::shared_ptr<const Function<Bool> > TrueCondition();
	boost::shared_ptr<const std::vector<boost::shared_ptr<TagAction> > > actions_;
	boost::shared_ptr<const Function<Bool> > condition_;
	std::string name_;
};



//
//--- implementation details ---
//
inline
TagRule::TagRule(
		const std::string& name,
		const Variables& variables,
		const boost::shared_ptr<const std::vector<boost::shared_ptr<TagAction> > >& actions,
		const boost::shared_ptr<const Function<Bool> >& condition)
	: ParsedExpression(variables),
	  actions_(actions),
	  condition_(condition),
	  name_(name) {
	BOOST_ASSERT(actions_);
	BOOST_ASSERT(condition_);
}

inline
Bool TagRule::operator()(SentenceContext& sc) {
	return execute(sc);
}

inline
std::string TagRule::name() const {
	return name_;
}

inline
TagRule* TagRule::clone_internal() const {
	return new TagRule(name_, *variables_, actions_, condition_);
}

inline
TagRule::TagRule(const TagRule &other, bool clean)
	: ParsedExpression(*other.variables_),
	  actions_(other.actions_),
	  condition_(other.condition_),
	  name_(other.name_)
{
	BOOST_ASSERT(actions_);
	BOOST_ASSERT(condition_);
	if(clean) {
		this->clean();
	}
}

inline
TagRule TagRule::clone() const {
	return *this;
}

inline
TagRule TagRule::clone_clean() const {
	return TagRule(*this, true);
}

inline
TagRule& TagRule::operator=(const TagRule& other) {
	BOOST_ASSERT(other.actions_);
	BOOST_ASSERT(other.condition_);
	actions_ = other.actions_;
	condition_ = other.condition_;
	name_ = other.name_;
	variables_.reset(other.variables_->clone());
	return *this;
}

inline
TagRule::TagRule()
	: ParsedExpression((Variables())),
	  actions_(boost::make_shared<std::vector<boost::shared_ptr<TagAction> > >()),
	  condition_(detail::DefaultFunction<Bool>()),
	  name_()

{
	BOOST_ASSERT(actions_);
	BOOST_ASSERT(condition_);
}

inline
boost::shared_ptr<TagRule> TagRule::clone_ptr() const {
	return boost::shared_ptr<TagRule>(clone_internal());
}

inline
boost::shared_ptr<TagRule> TagRule::clone_clean_ptr() const {
	boost::shared_ptr<TagRule> copy(clone_internal());
	BOOST_ASSERT(copy);
	copy->clean();
	return copy;
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_TAGRULE_H
