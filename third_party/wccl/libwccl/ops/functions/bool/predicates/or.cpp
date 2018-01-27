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

#include <libwccl/ops/functions/bool/predicates/or.h>
#include <boost/foreach.hpp>

namespace Wccl {

Or::BaseRetValPtr Or::apply_internal(const FunExecContext& context) const
{
	BOOST_FOREACH (BoolFunctionPtr expression, *expressions_) {
		if(expression->apply(context)->get_value()) {
			return Predicate::True(context);
		}
	}
	return Predicate::False(context);
}

} /* end ns Wccl */
