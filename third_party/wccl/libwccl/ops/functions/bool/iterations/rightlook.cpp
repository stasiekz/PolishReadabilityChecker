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

#include <libwccl/ops/functions/bool/iterations/rightlook.h>

namespace Wccl {

bool RightLook::iterate(
	int left,
	int right,
	Wccl::Position &p,
	const Wccl::FunExecContext &context) const
{
	for(int i = left; i <= right; ++i) {
		p.set_value(i);
		if(evaluating_expr_->apply(context)->get_value()) {
			return true;
		}
	}
	return false;
}

} /* end ns Wccl */
