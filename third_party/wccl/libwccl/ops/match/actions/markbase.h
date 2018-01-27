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

#ifndef LIBWCCL_OPS_MATCH_ACTIONS_MARKBASE_H
#define LIBWCCL_OPS_MATCH_ACTIONS_MARKBASE_H


#include <libwccl/ops/match/matchaction.h>
#include <libwccl/ops/function.h>

namespace Wccl {
namespace Matching {

class MarkBase : public MatchAction
{
public:
	MarkBase(
			const boost::shared_ptr<Function<Match> >& match_from,
			const boost::shared_ptr<Function<Match> >& match_to,
			const boost::shared_ptr<Function<Match> >& head_match,
			const std::string& annotation_name)
		: match_from_(match_from),
		  match_to_(match_to),
		  head_match_(head_match),
		  chan_name_(annotation_name)
	{
		BOOST_ASSERT(match_from_);
		BOOST_ASSERT(match_to_);
		BOOST_ASSERT(head_match_);
	}

	virtual std::string name() const = 0;

	/**
	 * Executes the action for the given execution context.
	 */
	virtual void execute(const ActionExecContext& context) const = 0;

	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	const boost::shared_ptr<Function<Match> > match_from_;
	const boost::shared_ptr<Function<Match> > match_to_;
	const boost::shared_ptr<Function<Match> > head_match_;
	const std::string chan_name_;

	/**
	 * Writes string representation of the MatchAction to
	 * an output stream.
	 * @returns Stream written to.
	 * @note May be incomplete and/or containt internal info.
	 */
	virtual std::ostream& write_to(std::ostream& ostream) const;

	/**
	  * The actual implementation. Depending on may_overwrite, will either
	  * remove any previous annotations in the range given or will raise
	  * an exception if any annotations present. */
	void execute_mark(const ActionExecContext& context, bool may_overwrite)
			const;
};

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_ACTIONS_MARKBASE_H
