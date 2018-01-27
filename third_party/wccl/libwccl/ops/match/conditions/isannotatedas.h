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

#ifndef LIBWCCL_OPS_MATCH_CONDITIONS_ISANNOTATEDAS_H
#define LIBWCCL_OPS_MATCH_CONDITIONS_ISANNOTATEDAS_H

#include <libwccl/ops/match/matchcondition.h>
#include <libwccl/ops/function.h>

namespace Wccl {
namespace Matching {

/**
 * is() match condition - matches an annotation
 */
class IsAnnotatedAs : public MatchCondition
{
public:
	IsAnnotatedAs(const std::string& annotation_name)
		: chan_name_(annotation_name) {
			BOOST_ASSERT(!chan_name_.empty());
	}
	/**
	 * @returns Name of the Condition.
	 */
	std::string name() const {
		return "is";
	}
	/**
	 * Applies the condition to the given execution context.
	 * If a match is found, the current sentence Position is increased
	 * by the lenght of matched annotation segment.
	 */
	MatchResult apply(const ActionExecContext& context) const;

	/**
	 * @returns String representation of the Condition
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	/**
	 * Writes the string representation of the Condition to
	 * an output stream.
	 * @returns Stream written to.
	 * @note May be incomplete and/or containt internal info.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
private:
	const std::string chan_name_;
};

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_CONDITIONS_ISANNOTATEDAS_H
