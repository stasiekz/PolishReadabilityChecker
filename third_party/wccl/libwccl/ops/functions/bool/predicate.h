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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATE_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATE_H

#include <boost/scoped_ptr.hpp>

#include <libwccl/values/bool.h>
#include <libwccl/ops/functions/constant.h>

namespace Wccl {

/**
 * Abstract base class for Predicates (operators returning Bool)
 */
class Predicate : public Function<Bool> {
public:
	/**
	 * Helper function returing True, to use by predicates when returning value
	 */
	static RetValPtr True(const FunExecContext& context);
	/**
	 * Helper function returing False, to use by predicates when returning value
	 */
	static RetValPtr False(const FunExecContext& context);
	/**
	 * Helper function returing True or False depending on condition
	 */
	static RetValPtr evaluate(bool condition, const FunExecContext& context);
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATE_H
