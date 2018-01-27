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

#ifndef LIBWCCL_OPS_TAGACTIONS_MARK_H
#define LIBWCCL_OPS_TAGACTIONS_MARK_H

#include <libwccl/ops/tagaction.h>
#include <libwccl/values/position.h>
#include <libwccl/values/bool.h>
#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Action to mark an annotation fragment on a channel.
 */
class Mark : public TagAction
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;

	Mark(
		const PosFunctionPtr& pos_begin,
		const PosFunctionPtr& pos_end,
		const PosFunctionPtr& pos_head,
		const std::string& chan_name)
		: pos_begin_(pos_begin),
		  pos_end_(pos_end),
		  pos_head_(pos_head),
		  chan_name_(chan_name)
	{
		BOOST_ASSERT(pos_begin_);
		BOOST_ASSERT(pos_end_);
		BOOST_ASSERT(!chan_name.empty());
	}

	Mark(
		const PosFunctionPtr& pos_begin,
		const PosFunctionPtr& pos_end,
		const std::string& chan_name)
		: pos_begin_(pos_begin),
		  pos_end_(pos_end),
		  pos_head_(),
		  chan_name_(chan_name)
	{
		BOOST_ASSERT(pos_begin_);
		BOOST_ASSERT(pos_end_);
		BOOST_ASSERT(!chan_name.empty());
	}

	/**
	 * @returns Name of the function.
	 */
	std::string name() const {
		return "mark";
	}

	/**
	 * @returns String representation of the Action
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * Executes the Action on given context: Marks an annotation within
	 * the given channel on the range supplied. It is an error if there
	 * already is an annotation anywhere in the range in the channel.
	 * The Range is trimmed to sentence boundaries.
	 * No action is made in case of an invalid/empty range.
	 * @returns True if there were any changes made; False otherwise
	 */
	Bool execute(const ActionExecContext &context) const;

protected:
	/**
	 * Writes string representation of the Action to
	 * an output stream.
	 * @returns Stream written to.
	 * @note May be incomplete and/or containt internal info.
	 */
	std::ostream& write_to(std::ostream& ostream) const;

private:
	const PosFunctionPtr pos_begin_;
	const PosFunctionPtr pos_end_;
	const PosFunctionPtr pos_head_;
	const std::string chan_name_;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_TAGACTIONS_MARK_H
