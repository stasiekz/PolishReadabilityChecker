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

#ifndef LIBWCCL_VARIABLES_H
#define LIBWCCL_VARIABLES_H

#include <libwccl/exception.h>
#include <libwccl/values/bool.h>
#include <iostream>
#include <map>
#include <string>
#include <cassert>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/count.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/list.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp> 
#include <boost/type_traits/is_same.hpp>

namespace Wccl {

class Match;
class Position;
class StrSet;
class TSet;

namespace detail {

/**
 * Helper class template for the Variables class to facilitate
 * per-type maps.
 */
template<typename T>
class Vmap
{
public:
	typedef std::map< std::string, boost::shared_ptr<T> > map_t;
protected:
	Vmap() : map_() {}
	boost::shared_ptr<T> get(const std::string& s) const {
		typename map_t::const_iterator i = map_.find(s);
		if (i != map_.end()) {
			return i->second;
		} else {
			return boost::shared_ptr<T>();
		}
	}
	map_t map_;
};

} /* end ns detail */

/**
 * Exception class for variable type mismatches
 */
class VariableTypeMismatch : public WcclError
{
public:
	VariableTypeMismatch(const std::string& varname)
	 : WcclError("Variable type mismatch"), varname(varname)
	{}

	~VariableTypeMismatch() throw() {}

	std::string varname;
};

/**
 * Exception class for invalid variable name errors in situations where
 * it is not possible to return a NULL-equivalent
 */
class InvalidVariableName : public WcclError
{
public:
	InvalidVariableName(const std::string& varname)
	 : WcclError("Invalid variable name"), varname(varname)
	{}

	~InvalidVariableName() throw() {}

	std::string varname;
};

/**
 * Base class for the "fast" variable accessors.
 */
class BaseAccessor
{
public:
	/// Variable name getter
	const std::string get_name() const {
		return varname_;
	}
protected:
	/// Protected ctor, only constructed by derived classes
	BaseAccessor(const std::string& varname)
		: varname_(varname)
	{
	}

	std::string varname_;
};

/**
 * A "fast" accessor class for getting a variable off a Variables object.
 *
 * It should be faster to create an accessor object from the variables name and
 * then use it multiple times than to use the name itself often.
 *
 * @todo the above is not the case yet.
 *
 * Objects of the accessor class can only be created by the Variables object,
 * are valid only for that Variables object and its copies, and might stop
 * working if the Variables object is touched in the wrong way in between the
 * creation and use of a VariableAccessor. UB is meant by "stop working".
 * Bad touch is not yet fully defined, removing a variable certainly is one.
 */
template<typename T>
class VariableAccessor : public BaseAccessor
{
public:

protected:
	/// Protected ctor, only created by Variables::create_accessor
	VariableAccessor(const std::string& varname)
		: BaseAccessor(varname)
	{
	}

	friend class Variables;
};

/**
 * A class for holding variables.
 *
 * Variables are values with a name. They can any of the allowed types,
 * but only one variable, of one type, can exist under one name, so it is
 * not allowed to have an ambiguous variable name.
 * 
 * Values are kept as shared pointers and returned as such.
 *
 * Variables are manipulated by name using specific type overloads, and also
 * by the Value overload that, if possible, works across all valid types.
 * Some functions can only work for a concrete Value derived class, they are
 * marked as such and guarded by compile-time checks.
 *
 * Values are subtypes of the Value class, valid types are listed as arguments
 * of instantiations of the detail::Vmap base class template, and in the type
 * list below. Adding a new subtype should only take adding it to these two
 * places: inherit from another Vmap, add to typelist.
 */
class Variables : detail::Vmap<Value>
	, detail::Vmap<Bool>
	, detail::Vmap<Position>
	, detail::Vmap<StrSet>
	, detail::Vmap<TSet>
	, detail::Vmap<Match>
	, boost::noncopyable
{
public:
	/// Valid value types, should match the inheritance.
	/// the type Value must be first, order of other items is not important
	typedef boost::mpl::list<Value,
		Bool, Position, StrSet, TSet, Match> types;

	/// Constructor, creates an empty instance.
	Variables();

	/** Per-type size checker.
	 *
	 * Use the Value type to count all variables. In C++0x, Value would be
	 * the default template argument.
	 */
	template<typename T>
	int size() const {
		BOOST_MPL_ASSERT(( boost::mpl::count<types, T> ));
		return detail::Vmap<T>::map_.size();
	}

	/** Set all values to their default value.
	  *
	  * Effectively iterates through all variables and assigns them their
	  * respective type's default-constructed value.
	  */
	void reset_values();

	/** Per-type all variables accesor.
	 *
	 * Allows iterating through all variables of a given type (or all variables
	 * if the type is Value). Values may be modified, the variable names or
	 * shared pointers themselves cannot, use put etc. for that.
	 */
	template<typename T>
	const typename detail::Vmap<T>::map_t& get_all() const {
		BOOST_MPL_ASSERT(( boost::mpl::count<types, T> ));
		return detail::Vmap<T>::map_;
	}

	/** Variables cloning.
	 *
	 * A clone has the same variable names, with distinct underlying Value
	 * objects, and the same values in these objects.
	 */
	Variables* clone() const;

	/** Get a variable.
	 *
	 * Returns the variable with the given name, or NULL if it does not exist.
	 * Use a specialied version with the requested type, or the general Value 
	 * type if looking for any variable.
	 * 
	 * If T is not Value and the variable exists but has a different type,
	 * returns NULL as if the variable did not exist.
	 */
	template<typename T>
	boost::shared_ptr<T> get(const std::string& s) const {
		BOOST_MPL_ASSERT(( boost::mpl::count<types, T> ));
		return detail::Vmap<T>::get(s);
	}

	/** Get a variable, throwing version.
	 *
	 * Returns a valid pointer to the variable with the given name, as get(),
	 * or throws if it is not found. Never returns NULL.
	 *
	 * May throw either VariableTypeMismatch or InvalidVariableName.
	 */
	template<typename T>
	boost::shared_ptr<T> get_or_throw(const std::string& s) const {
		BOOST_MPL_ASSERT(( boost::mpl::count<types, T> ));
		boost::shared_ptr<T> r = detail::Vmap<T>::get(s);
		if (r) {
			return r;
		} else {
			if (detail::Vmap<Value>::get(s)) {
				throw VariableTypeMismatch(s);
			} else {
				throw InvalidVariableName(s);
			}
		}
	}

	/** Convenience function to get the actual underlying Value of a variable.
	 *
	 * Will throw on errors like get_or_throw would. Returns whatever the
	 * Value referenced returns in its get_value.
	 */
	template<typename T>
	const typename T::value_type& get_value(const std::string& s) const {
		return get_or_throw<T>(s)->get_value();
	}

	/** Create a "fast" accessor for a variable by name.
	 *
	 * Returns a special object which is valid for use in get_fast, which when
	 * used should be faster than calling get<T>(s) repeatedly.
	 *
	 * If the variable name is invalid or teh type doesn't match, an exception
	 * will be thrown, either VariableTypeMismatch or InvalidVariableName.
	 */
	template<typename T>
	VariableAccessor<T> create_accessor(const std::string& s) {
		BOOST_MPL_ASSERT(( boost::mpl::count<types, T> ));
		if (!get<T>(s)) {
			if (get<Value>(s)) {
				throw VariableTypeMismatch(s);
			} else {
				throw InvalidVariableName(s);
			}
		}
		return VariableAccessor<T>(s);
	}

	/** Get a variable using a "fast" accessor.
	 *
	 * The accessor is required to have been created by this particular object,
	 * or a copy (either way), and the Variables object must have not had
	 * variables removed (adding new variables should not invalidate accessors).
	 *
	 * If the VariableAccessor comes form a different Variables object or there
	 * have been invalidating changes to this Variables object, behavior is not
	 * well defined. You might get a NULL, a different variable than you
	 * expected, or a segfault.
	 */
	template<typename T>
	boost::shared_ptr<T> get_fast(const VariableAccessor<T>& a) const {
		BOOST_MPL_ASSERT(( boost::mpl::count<types, T> ));
		return get<T>(a.get_name());
	}

	/** Get a variable, possibly default-creatng it.
	 *
	 * Returns the variable with the given name, or, if it does not exist,
	 * creates a new empty variable of the type given as the template
	 * parameter and returns that.
	 *
	 * The variable must be of the requested type, otherwise
	 * VariableTypeMismatch is thrown.
	 * 
	 * The function cannot be called if T is Value since T must be
	 * constructible.
	 */
	template<typename T>
	boost::shared_ptr<T> get_put(const std::string& s); 

	/** Add a variable with the given name and the given value.
	 *
	 * The type should be normally deduced by the compiler, and should be 
	 * a subtype of Value. Value itself can not be used, but @see put_any.
	 *
	 * If the variable does not exist, it is created.
	 *
	 * If it already exists it is replaced provided that the current type
	 * matches (if it does not, VariableTypeMismatch is thrown). This
	 * replacement is different from a set(s, v) since the original
	 * value is not modified.
	 */
	template<typename T>
	void put(const std::string& s, const boost::shared_ptr<T>& v);

	/** Add a variable with the given name and the given value, dynamically
	 * deducing the actual type.
	 *
	 * Prefer put<T> with a concrete T since it will be faster.
	 *
	 * @see the put version with a concrete type for semantics since
	 * it is called eventually.
	 *
	 * The first matching type (where a dynamic_cast succeeds) is used.
	 * If the passed Value cannot be converted to a valid subtype,
	 * VariableTypeMismatch is thrown.
	 */
	void put_any(const std::string& s, const boost::shared_ptr<Value>& v);

	/** Remove a variable assuming it has the given type.
	 *
	 * When T is Value, equivalent to del_any.
	 *
	 * @returns true if the variable was removed, false otherwise
	 */
	template<typename T>
	bool del_variable(const std::string& s);

	/** Remove a variable regardless of it's type.
	 *
	 * Prefer del_variable<T> with a concrete T as it will be faster.
	 *
	 * @returns true if the variable was removed, false otherwise
	 */
	bool del_any(const std::string& s);

	/** Put<T> convenience overload for raw pointers that wraps them in a
	 * shared_ptr (thus taking ownership).
	 */
	template<typename T>
	void put(const std::string& s, T* v) {
		put(s, boost::shared_ptr<T>(v));
	}

	/** Convenience function for putting raw values (not pointers)
	 * that will allocate a copy as a shared pointer.
	 */
	template<typename T>
	void put(const std::string& s, const T& v) {
		put(s, boost::make_shared<T>(v));
	}

	/** Set an existing variable to the given value, or create a new variable.
	 *
	 * If the variable with the given name does not exist, it is created.
	 * If it exists, it is set to the passed value (modified in-place).
	 * This is in contrast to put(s, v) which would leave the original value
	 * unchanged.
	 */
	template<typename T>
	void set(const std::string& s, const T& v);

	struct AccessHelper;
	friend struct Variables::AccessHelper;
	struct AccessHelper
	{
		template<typename T>
		typename detail::Vmap<T>::map_t& access() {
			return vars.get_all_nonconst<T>();
		}
		Variables& vars;
	private:
		AccessHelper(Variables& v) : vars(v) {}
		friend class Variables;
	};
private:
	template<typename T>
	typename detail::Vmap<T>::map_t& get_all_nonconst() {
		return detail::Vmap<T>::map_;
	}
};

/* implementation */

template<typename T> inline
boost::shared_ptr<T> Variables::get_put(const std::string& s)
{
	BOOST_MPL_ASSERT(( boost::mpl::count<types, T> ));
	boost::shared_ptr<T> v = get<T>(s);
	if (!v) {
		if (!get<Value>(s)) {
			v.reset(new T());
			put(s, v);
		} else {
			throw VariableTypeMismatch(s);
		}
	}
	return v;
}


template<typename T> inline
void Variables::put(const std::string& s, const boost::shared_ptr<T>& v) {
	BOOST_MPL_ASSERT(( boost::mpl::count<types, T> ));
	// disable put<Value>
	BOOST_MPL_ASSERT_NOT(( boost::is_same<T, Value> ));
	using detail::Vmap;
	typename Vmap<T>::map_t::iterator i = Vmap<T>::map_.find(s);
	typename Vmap<Value>::map_t::iterator vi = Vmap<Value>::map_.find(s);
	if (i != Vmap<T>::map_.end()) {
		assert(vi != Vmap<Value>::map_.end()); //invariant
		i->second = v;
		vi->second = v;
	} else if (vi == Vmap<Value>::map_.end()) {
		Vmap<T>::map_[s] = v;
		Vmap<Value>::map_[s] = v;
	} else {
		throw VariableTypeMismatch(s);
	}
}

template<typename T> inline
bool Variables::del_variable(const std::string &s)
{
	BOOST_MPL_ASSERT(( boost::mpl::count<types, T> ));
	if (detail::Vmap<T>::map_.erase(s)) {
		bool was_in_values = detail::Vmap<Value>::map_.erase(s);
		assert(was_in_values);
		return true;
	} else {
		return false;
	}
}

template<> inline
bool Variables::del_variable<Value>(const std::string &s)
{
	return del_any(s);
}

template<typename T> inline
void Variables::set(const std::string& s, const T& v)
{
	BOOST_MPL_ASSERT(( boost::mpl::count<types, T> ));
	boost::shared_ptr<T> p = get<T>(s);
	if (p) {
		*p = v;
	} else {
		put(s, v);
	}
}

} /* end ns Wccl */

#endif // LIBWCCL_VARIABLES_H
