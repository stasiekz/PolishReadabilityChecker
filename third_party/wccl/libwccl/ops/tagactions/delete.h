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

#ifndef LIBWCCL_OPS_TAGACTIONS_DELETE_H
#define LIBWCCL_OPS_TAGACTIONS_DELETE_H

#include <libwccl/ops/tagaction.h>
#include <libwccl/values/position.h>
#include <libwccl/values/bool.h>
#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Action to delete lexemes that meet a condition
 * (unless no lexemes would be left, in which case
 * token is left alone with no changes)
 */
class Delete : public TagAction
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;
	typedef boost::shared_ptr<Function<Bool> > BoolFunctionPtr;

	Delete(const BoolFunctionPtr& condition, const PosFunctionPtr& pos = detail::CurrentPos())
		: pos_(pos),
		  condition_(condition)
	{
		BOOST_ASSERT(pos_);
		BOOST_ASSERT(condition_);
	}

	/**
	 * @returns Name of the function.
	 */
	std::string name() const {
		return "delete";
	}

	/**
	 * @returns String representation of the Action
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * Executes the Action on given context: it deletes
	 * all lexemes of the token at given position that meet
	 * the condition, unless all of them meet the condition.
	 * In such case no action is commited to avoid a token
	 * that would otherwise have no lexemes remaining.
	 * Also, no action is done if position points outside sentence.
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
	const PosFunctionPtr pos_;
	const BoolFunctionPtr condition_;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_TAGACTIONS_DELETE_H
