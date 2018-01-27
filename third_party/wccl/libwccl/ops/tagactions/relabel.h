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

#ifndef LIBWCCL_OPS_TAGACTIONS_RELABEL_H
#define LIBWCCL_OPS_TAGACTIONS_RELABEL_H

#include <libwccl/ops/tagaction.h>
#include <libwccl/values/position.h>
#include <libwccl/values/bool.h>
#include <libwccl/values/tset.h>
#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Action to change part of speech for lexemes that meet a condition.
 */
class Relabel : public TagAction
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;
	typedef boost::shared_ptr<Function<Bool> > BoolFunctionPtr;
	typedef boost::shared_ptr<Function<TSet> > TSetFunctionPtr;

	Relabel(
		const TSetFunctionPtr& replace_with,
		const BoolFunctionPtr& condition,
		const PosFunctionPtr& pos = detail::CurrentPos())
		: pos_(pos),
		  condition_(condition),
		  replace_with_(replace_with)
	{
		BOOST_ASSERT(pos_);
		BOOST_ASSERT(condition_);
		BOOST_ASSERT(replace_with_);
	}

	/**
	 * @returns Name of the function.
	 */
	std::string name() const {
		return "relabel";
	}

	/**
	 * @returns String representation of the Action
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * Executes the Action on given context: for all lexemes
	 * that meet given condition, change their part of speech
	 * to the part of speech given by provided TSet function.
	 * No action is done if position points outside sentence.
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
	const TSetFunctionPtr replace_with_;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_TAGACTIONS_RELABEL_H
