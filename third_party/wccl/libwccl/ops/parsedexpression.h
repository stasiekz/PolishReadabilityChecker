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

#ifndef LIBWCCL_OPS_PARSEDEXPRESSION_H
#define LIBWCCL_OPS_PARSEDEXPRESSION_H

#include <libwccl/ops/expression.h>
#include <libwccl/variables.h>

namespace Wccl {

/**
 * Abstract base class for WCCL expressions coming from parser and thus
 * having a set of variables.
 * @attention Usage of the class is not thread safe, but you can
 * create a copy and the copy can run concurrently, beacuse it has its
 * own set of variables.
 * @note See below for various usage of the class.
 * \code
 * // op is a ParsedExpression and op_ptr a (shared) pointer to it.
 * std::string s;
 * s = op["Foo"].to_raw_string();
 * s = (*op_ptr)["Foo"].to_raw_string();
 * // versus
 * s = op.get<Wccl::Bool>("Foo").to_raw_string();
 * s = op_ptr->get<Wccl::Bool>("Foo").to_raw_string();
 * // but!
 * Wccl::Bool b;
 * op["Foo"] = b; // NOT OK! compile error!
 * op.get<Wccl::Bool>("Foo") = b; // OK
 * op.set("Foo", b); // alternative for setting the value
 * \endcode
 */
class ParsedExpression : public Expression
{
public:

	/**
	 * operator[] that gets value of a variable with given name.
	 * @param var_name Variable name.
	 * @returns Const reference to value of the variable.
	 * @throws InvalidVariableName if the expression doesn't have variable
	 * of given name.
	 * @note This allows only read-only (const) access to value, using the base
	 * type Value. There would be no way to guarantee proper type if assignment
	 * to returned reference had been allowed.
	 * @see \link get(const std::string& var_name) const get<T> const \endlink -
	 * a version that returns concrete type of Value.
	 * @see \link get(const std::string& var_name) get<T> \endlink - a version
	 * that returns concrete type of Value and also allows assignment of the value.
	 * @see set<T> - a convenience function if all you want is to assign
	 * value to a variable.
	 */
	const Value& operator[](const std::string& var_name) const;

	/**
	 * Gets value of a variable with given name and type.
	 * @returns Const reference to value of the variable of type T.
	 * @param var_name Variable name.
	 * @throws InvalidVariableName if the expression doesn't have variable
	 * of given name.
	 * @throws VariableTypeMismatch if the expression has variable of the given
	 * name, but its type is different than the supplied T.
	 * @see \link operator[]() operator[] \endlink - a version that returns
	 * the value using base type Value, which may be more convenient
	 * in some situations.
	 * @see \link get(const std::string& var_name) get<T> \endlink - non-const
	 * version that allows assignment of a value to the result
	 * @see set<T> - a convenience function if all you want is to assign
	 * value to a variable.
	 */
	template<class T>
	const T& get(const std::string& var_name) const;

	/**
	 * Gets value of a variable with given name and type, allowing for
	 * assignment.
	 * @returns Reference to Value of the variable of type T. You can assign
	 * values to the result and that changes value of the variable accordingly.
	 * @param var_name Variable name.
	 * @throws InvalidVariableName if the expression doesn't have variable
	 * of given name.
	 * @throws VariableTypeMismatch if the expression has variable of the given
	 * name, but its type is different than the supplied T.
	 * @see \link operator[]() operator[] \endlink - a version that returns
	 * the value using base type Value, which may be more convenient
	 * in some situations (but doesn't allow assignment to the result)
	 * @see \link get(const std::string& var_name) const get<T> const \endlink -
	 * the const version (which doesn't allow assignment to the result)
	 * @see set<T> - a convenience function if all you want is to assign
	 * a value to a variable.
	 */
	template<class T>
	T& get(const std::string& var_name);

	/**
	 * Assigns value to a variable with given name and type.
	 * @param var_name Variable name.
	 * @param value Value to assign.
	 * @throws InvalidVariableName if the expression doesn't have variable
	 * of given name.
	 * @throws VariableTypeMismatch if the expression has variable of the given
	 * name, but its type is different than the supplied T.
	 * @note This may be more convenient than assiging a value via get, because
	 * the type T is inferred from the parameter value:
	 * \code
	 * Bool b;
	 * op.set("Foo", b) //notice that set<Bool> isn't required
	 * //versus
	 * op.get<Bool>("Foo") = b; // specification of type is required
	 * \endcode
	 * @see get<T> - allows assignment of the value with assignment operator,
	 * but requires explicit specification of T.
	 */
	template<class T>
	void set(const std::string& var_name, const T& value);

	/**
	 * Checks if has a variable with given name and type.
	 * @returns if such variable exists AND is of type T.
	 * @param var_name Variable name.
	 */
	template<class T>
	bool has(const std::string& var_name);

	/**
	 * Sets all variables to their default values.
	 * @note Values of variables are preserved between applications
	 * of the expression. If the expression depends on some variables
	 * that it isn't setting by itself prior to the usage, you probably
	 * want to reset those variables yourself before each run, or you may
	 * call clean() depending on the situation.
	 * Also, derived classes that have clone() method copy the values
	 * as well, but a class may provide convenience method clone_clean()
	 * in case you want to get a clean state.
	 */
	void clean();

	/**
	 * @returns A copy of the expression, with values of the variables
	 * copied as well.
	 * @see clone_clean_ptr - convenience version that returns clean copy.
	 */
	boost::shared_ptr<ParsedExpression> clone_ptr() const;

	/**
	 * @returns A copy of the expression, with values of the variables
	 * set to their defaults.
	 * @see clone_ptr - a version that keeps values of the variables.
	 */
	boost::shared_ptr<ParsedExpression> clone_clean_ptr() const;
	
	/**
	 * @returns vector of strings corresponfing to valid variable names
	 * in this operator.
	 */
	std::vector<std::string> valid_variable_names() const;

	/**
	 * @returns String with representation of all held variables,
	 * in form of "variable = value" pairs, each pair on one line.
	 */
	std::string variables_string(const Corpus2::Tagset& tagset) const;

	/**
	 * Dump all variables to an output stream, in form of
	 * "variable = value" pairs, each pair on another line.
	 * @returns Stream written to.
	 */
	std::ostream& dump_variables(std::ostream& ostream, const Corpus2::Tagset& tagset) const;

protected:
	explicit ParsedExpression(const Variables& variables);

	boost::shared_ptr<Variables> variables_;

	virtual ParsedExpression* clone_internal() const = 0;
};



//--- implementation details ---

inline
ParsedExpression::ParsedExpression(const Variables &variables)
	: variables_(variables.clone())
{
}

inline
const Value& ParsedExpression::operator[](const std::string& var_name) const {
	boost::shared_ptr<Value> value = variables_->get<Value>(var_name);
	if (!value) {
		throw InvalidVariableName(var_name);
	}
	return *value;
}

template <class T> inline
const T& ParsedExpression::get(const std::string &var_name) const {
	BOOST_MPL_ASSERT_NOT(( boost::is_same<T, Value> ));
	boost::shared_ptr<T> value = variables_->get<T>(var_name);
	if (!value) {
		if (!variables_->get<Value>(var_name)) {
			throw InvalidVariableName(var_name);
		} else {
			throw VariableTypeMismatch(var_name);
		}
	}
	return *value;
}

template <class T> inline
T& ParsedExpression::get(const std::string &var_name) {
	BOOST_MPL_ASSERT_NOT(( boost::is_same<T, Value> ));
	boost::shared_ptr<T> value = variables_->get<T>(var_name);
	if (!value) {
		if (!variables_->get<Value>(var_name)) {
			throw InvalidVariableName(var_name);
		} else {
			throw VariableTypeMismatch(var_name);
		}
	}
	return *value;
}

template<class T> inline
void ParsedExpression::set(const std::string& var_name, const T& value) {
	get<T>(var_name) = value;
}

template<class T>
bool ParsedExpression::has(const std::string& var_name) {
	boost::shared_ptr<T> value = variables_->get<T>(var_name);
	return value;
}

inline
void ParsedExpression::clean()
{
	variables_->reset_values();
}

inline
boost::shared_ptr<ParsedExpression> ParsedExpression::clone_ptr() const {
	return boost::shared_ptr<ParsedExpression>(clone_internal());
}

inline
boost::shared_ptr<ParsedExpression> ParsedExpression::clone_clean_ptr() const {
	boost::shared_ptr<ParsedExpression> copy(clone_internal());
	BOOST_ASSERT(copy);
	copy->clean();
	return copy;
}

inline
std::vector<std::string> ParsedExpression::valid_variable_names() const {
	std::vector<std::string> valid;
	typedef std::pair<std::string, boost::shared_ptr<Wccl::Value> > v_t;
	// ja chcę c++1x i auto/decltype
	BOOST_FOREACH (const v_t& v, variables_->get_all<Wccl::Value>()) {
		valid.push_back(v.first);
	}
	return valid;
}

inline
std::ostream& ParsedExpression::dump_variables(
		std::ostream &ostream,
		const Corpus2::Tagset &tagset) const {
	typedef std::pair<std::string, boost::shared_ptr<Wccl::Value> > v_t;
	BOOST_FOREACH (const v_t& v, variables_->get_all<Wccl::Value>()) {
		ostream << v.second->make_var_repr(v.first) << "="
			<< v.second->to_string(tagset) << "\n";
	}
	return ostream;
}

inline
std::string ParsedExpression::variables_string(const Corpus2::Tagset &tagset) const {
	std::ostringstream oss;
	dump_variables(oss, tagset);
	return oss.str();
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_PARSEDEXPRESSION_H
