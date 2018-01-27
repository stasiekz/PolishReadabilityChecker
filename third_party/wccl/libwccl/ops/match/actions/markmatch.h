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

#ifndef LIBWCCL_OPS_MATCH_ACTIONS_MARKMATCH_H
#define LIBWCCL_OPS_MATCH_ACTIONS_MARKMATCH_H

#include <libwccl/ops/match/actions/markbase.h>

namespace Wccl {
namespace Matching {

class MarkMatch : public MarkBase
{
public:
	MarkMatch(
			const boost::shared_ptr<Function<Match> >& match_from,
			const boost::shared_ptr<Function<Match> >& match_to,
			const boost::shared_ptr<Function<Match> >& head_match,
			const std::string& annotation_name)
		: MarkBase(match_from, match_to, head_match, annotation_name)
	{
	}

	MarkMatch(
			const boost::shared_ptr<Function<Match> >& match_from,
			const boost::shared_ptr<Function<Match> >& match_to,
			const std::string& annotation_name)
		: MarkBase(match_from, match_to, match_from, annotation_name)
	{
	}

	MarkMatch(
			const boost::shared_ptr<Function<Match> >& match_from_to,
			const std::string& annotation_name)
		: MarkBase(match_from_to, match_from_to, match_from_to, annotation_name)
	{
	}

	/**
	 * @returns Name of the action.
	 */
	std::string name() const {
		return "mark";
	}

	/**
	 * Executes the action for the given execution context.
	 */
	void execute(const ActionExecContext& context) const;
};

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_ACTIONS_MARKMATCH_H
