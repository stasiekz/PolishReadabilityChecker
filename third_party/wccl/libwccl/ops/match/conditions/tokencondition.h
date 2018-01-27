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

#ifndef LIBWCCL_OPS_MATCH_CONDITIONS_TOKENCODITION_H
#define LIBWCCL_OPS_MATCH_CONDITIONS_TOKENCODITION_H

#include <libwccl/ops/match/matchcondition.h>
#include <libwccl/ops/function.h>

namespace Wccl {
namespace Matching {

/**
 * Class for conditions of MatchOperator that operate on tokens,
 * wrapping WCCL Predicates
 */
class TokenCondition : public MatchCondition
{
public:
	TokenCondition(const boost::shared_ptr<Function<Bool> >& predicate);
	/**
	 * @returns Name of the condition.
	 */
	std::string name() const;
	/**
	 * Applies the condition to the given execution context.
	 * If match is found, the current sentence Position is increased
	 * by one (the size of token match, which is always one).
	 */
	MatchResult apply(const ActionExecContext& context) const;

	/**
	 * @returns String representation of the Action
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	/**
	 * Writes string representation of the TokenCondition to
	 * an output stream.
	 * @returns Stream written to.
	 * @note May be incomplete and/or containt internal info.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
private:
	const boost::shared_ptr< const Function<Bool> > _predicate;
};

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_CONDITIONS_TOKENCODITION_H
