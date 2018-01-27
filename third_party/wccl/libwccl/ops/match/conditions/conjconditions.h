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

#ifndef LIBWCCL_OPS_MATCH_CONDITIONS_CONJCONDITIONS_H
#define LIBWCCL_OPS_MATCH_CONDITIONS_CONJCONDITIONS_H

#include <libwccl/ops/match/matchcondition.h>

namespace Wccl {
namespace Matching {

/**
 * Class that wraps a vector of match conditions to act as
 * single condition that matches if all inner conditions match.
 */
class ConjConditions : public MatchCondition
{
public:
	ConjConditions(const std::vector< boost::shared_ptr<const MatchCondition> >& conditions);

	/**
	 * @returns Name of the condition.
	 */
	std::string name() const {
		return "and"; // string repr actually doesn't include this
	}
	/**
	 * Applies the condition to the given execution context.
	 * If match is found, the current position in sentence is increased
	 * by one (the size of token match, which is always one).
	 */
	MatchResult apply(const ActionExecContext& context) const;

	/**
	 * @returns String representation of the MatchCondition
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	/**
	 * Writes string representation of the MatchCondition to
	 * an output stream.
	 * @returns Stream written to.
	 * @note May be incomplete and/or containt internal info.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
private:
	const std::vector<boost::shared_ptr<const MatchCondition> > _conditions;
};

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_CONDITIONS_CONJCONDITIONS_H
