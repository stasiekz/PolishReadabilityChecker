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

#ifndef LIBWCCL_OPS_MATCH_ACTIONS_UNMARKMATCH_H
#define LIBWCCL_OPS_MATCH_ACTIONS_UNMARKMATCH_H

#include <libwccl/ops/match/matchaction.h>
#include <libwccl/ops/function.h>

namespace Wccl {
namespace Matching {

/**
 * Action to unmark (delete) an annotation passing through a first
 * token of a Match.
 */
class UnmarkMatch : public MatchAction
{
public:
	UnmarkMatch(
			const boost::shared_ptr<Function<Match> >& match,
			const std::string& annotation_name)
		: match_(match),
		  chan_name_(annotation_name)
	{
		BOOST_ASSERT(match_);
		BOOST_ASSERT(!chan_name_.empty());
	}

	/**
	 * @returns Name of the action.
	 */
	std::string name() const {
		return "unmark";
	}

	/**
	 * Executes the action for the given execution context.
	 */
	void execute(const ActionExecContext& context) const;

	/**
	 * @returns String representation of the expression.
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	/**
	 * Writes string representation of the MatchAction to
	 * an output stream.
	 * @returns Stream written to.
	 * @note May be incomplete and/or containt internal info.
	 */
	virtual std::ostream& write_to(std::ostream& ostream) const;
private:
	const boost::shared_ptr<Function<Match> > match_;
	const std::string chan_name_;
};

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_ACTIONS_UNMARKMATCH_H
