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

#ifndef LIBWCCL_OPS_MATCH_MATCHCONDITION_H
#define LIBWCCL_OPS_MATCH_MATCHCONDITION_H

#include <libwccl/ops/actionexeccontext.h>
#include <libwccl/ops/expression.h>
#include <libwccl/ops/match/matchresult.h>

namespace Wccl {
namespace Matching {

/**
 * Abstract base class for conditions of MatchOperator
 */
class MatchCondition : public Expression
{
public:
	/**
	 * @returns Name of the condition.
	 */
	virtual std::string name() const = 0;
	/**
	 * Applies the condition to the given execution context.
	 * If match is found, the current sentence position is
	 * set to position right after the match.
	 */
	virtual MatchResult apply(const ActionExecContext& context) const = 0;
};

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_MATCHCONDITION_H
