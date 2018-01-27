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

#ifndef LIBWCCL_OPS_FUNCTIONS_H
#define LIBWCCL_OPS_FUNCTIONS_H

#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include <libwccl/ops/expression.h>
#include <libwccl/values/value.h>
#include <libwccl/ops/funexeccontext.h>

namespace Wccl {

/**
 * Abstract base class for WCCL operators that are functions returning a Value
 */
class FunctionBase : public Expression
{
public:
	/**
	 * @returns Name of the function. By default it is same as raw name.
	 */
	virtual std::string name(const Corpus2::Tagset&) const {
		return raw_name();
	}
	/**
	 * @returns A representation of function's name that does
	 * not require a tagset.
	 * @note May be incomplete and/or contain internal info.
	 */
	virtual std::string raw_name() const = 0;
//protected:
	/**
	 * Base type returned after application of function
	 * (shared pointer to a const Value)
	 */
	typedef boost::shared_ptr<const Value> BaseRetValPtr;
	/**
	 * Applies the function for the given execution context.
	 */
	virtual BaseRetValPtr apply_internal(const FunExecContext& context) const = 0;
};

/**
 * Abstract base class template for functional WCCL operators that are functions
 * returning a value of given type
 */
template<class T>
class Function : public FunctionBase {
	BOOST_MPL_ASSERT( (boost::is_base_of<Value, T>) );
	BOOST_MPL_ASSERT_NOT( (boost::is_same<Value, T>) );
public:
	/**
	 * Type returned after application of function (shared pointer to
	 * a variable of the specified return type)
	 */
	typedef boost::shared_ptr<const T> RetValPtr;

	/**
	 * Applies the function, given the sentence context, returning specific
	 * type of Value (as shared pointer). Uses apply_internal which has to
	 * be specified in derived classes.
	 */
	RetValPtr apply(const FunExecContext& context) const {
		RetValPtr v = boost::dynamic_pointer_cast<const T>(apply_internal(context));
		BOOST_ASSERT(v);
		return v;
	}
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_H
