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

#ifndef LIBWCCL_OPS_MATCH_CONDITIONS_REPEATEDMATCH_H
#define LIBWCCL_OPS_MATCH_CONDITIONS_REPEATEDMATCH_H

#include <libwccl/ops/match/conditions/conjconditions.h>

namespace Wccl {
namespace Matching {

/**
 * Class for "repeat" condition of match
 */
class RepeatedMatch : public MatchCondition
{
public:
	RepeatedMatch(const boost::shared_ptr<ConjConditions>& conditions);

	/**
	 * @returns Name of the condition.
	 */
	std::string name() const {
		return "repeat";
	}
	/**
	 * Applies the condition to the given execution context.
	 * Inner conditions are executed repeatedly as long as matching is successful.
	 * If there were any successful matches, the result consists of "true"
	 * and the MatchVector constructed out of individual match vectors coming
	 * from each successful repetition.
	 * If there was no match, "false" is returned along with a default Match.
	 * If match is found, the current sentence Position is increased
	 * as to point one token after all the matched tokens, otherwise
	 * it stays unchanged.
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
	const boost::shared_ptr<ConjConditions> _conditions;
};

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_CONDITIONS_REPEATEDMATCH_H
