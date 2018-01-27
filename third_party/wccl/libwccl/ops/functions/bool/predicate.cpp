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

#include <libwccl/ops/functions/bool/predicate.h>

namespace Wccl {

Predicate::RetValPtr Predicate::True(const FunExecContext& context)
{
	static Constant<Bool> true_constant(Bool(true));
	return true_constant.apply(context);
}

Predicate::RetValPtr Predicate::False(const FunExecContext& context)
{
	static Constant<Bool> false_constant(Bool(false));
	return false_constant.apply(context);
}

Predicate::RetValPtr Predicate::evaluate(bool condition, const FunExecContext &context)
{
	return condition ? True(context) : False(context);
}

} /* end ns Wccl */
