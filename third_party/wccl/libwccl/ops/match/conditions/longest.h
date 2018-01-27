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

#ifndef LIBWCCL_OPS_MATCH_CONDITIONS_LONGEST_H
#define LIBWCCL_OPS_MATCH_CONDITIONS_LONGEST_H

#include <libwccl/ops/match/conditions/conjconditions.h>

namespace Wccl {
namespace Matching {

/**
 * Class for "longest" condition of match
 */
class Longest : public MatchCondition
{
public:
	Longest(const boost::shared_ptr<std::vector<boost::shared_ptr<ConjConditions> > >& variants);

	/**
	 * @returns Name of the condition.
	 */
	std::string name() const {
		return "longest";
	}
	/**
	 * Applies the condition to the given execution context.
	 * Inner match variants are executed one by one to check which
	 * one is the longest one. If any match variants were found to be "true",
	 * the one that produced the longest match is returned with "true".
	 * If there was no "true" match variant, "false" is returned instead.
	 * If a match is found, the current sentence Position is increased
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
	const boost::shared_ptr<std::vector<boost::shared_ptr<ConjConditions> > > _variants;
};
} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_CONDITIONS_LONGEST_H
