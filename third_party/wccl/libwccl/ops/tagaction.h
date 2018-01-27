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

#ifndef LIBWCCL_OPS_TAGACTION_H
#define LIBWCCL_OPS_TAGACTION_H

#include <libwccl/ops/actionexeccontext.h>
#include <libwccl/ops/function.h>


namespace Wccl {

/**
 * Abstract base class for actions in WCCL tagging rules
 */
class TagAction : public Expression
{
public:
	/**
	 * @returns Name of the action.
	 */
	virtual std::string name() const = 0;
	/**
	 * Executes the action for the given execution context.
	 */
	virtual Bool execute(const ActionExecContext& context) const = 0;
};

namespace detail {
	boost::shared_ptr<Function<Position> > CurrentPos();
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_TAGACTION_H
