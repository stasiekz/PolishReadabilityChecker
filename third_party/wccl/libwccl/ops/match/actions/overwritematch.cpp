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

#include <libwccl/ops/match/actions/overwritematch.h>

#include <sstream>


namespace Wccl {
namespace Matching {

void OverwriteMatch::execute(const ActionExecContext& context) const
{
	// fire inner execute in silent-overwrite mode
	execute_mark(context, true);
}

} /* end ns Matching */
} /* end ns Wccl */