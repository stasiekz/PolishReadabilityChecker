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

#ifndef LIBWCCL_OPS_TAGACTIONS_UNIFY_H
#define LIBWCCL_OPS_TAGACTIONS_UNIFY_H

#include <libwccl/ops/tagaction.h>
#include <libwccl/values/position.h>
#include <libwccl/values/bool.h>
#include <libwccl/values/tset.h>
#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Action to unify tokens on an agreement, removing lexemes
 * that violate the agreement.
 */
class Unify : public TagAction
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;
	typedef boost::shared_ptr<Function<TSet> > TSetFunctionPtr;

	Unify(
		const PosFunctionPtr& pos_begin,
		const PosFunctionPtr& pos_end,
		const TSetFunctionPtr& attribs_expr)
		: pos_begin_(pos_begin),
		  pos_end_(pos_end),
		  attribs_expr_(attribs_expr)
	{
		BOOST_ASSERT(pos_begin_);
		BOOST_ASSERT(pos_end_);
		BOOST_ASSERT(attribs_expr_);
	}

	/**
	 * @returns Name of the function.
	 */
	std::string name() const {
		return "unify";
	}

	/**
	 * @returns String representation of the Action
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * Executes the Action on given context: remove lexemes
	 * from tokens at given range that do not meet an agreement.
	 * Range is trimmed to sentence boundaries.
	 * No action is made in case of invalid/empty range.
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
	const TSetFunctionPtr attribs_expr_;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_TAGACTIONS_UNIFY_H
