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

#ifndef LIBWCCL_OPS_FUNCTIONS_POSITION_RELATIVEPOSITION_H
#define LIBWCCL_OPS_FUNCTIONS_POSITION_RELATIVEPOSITION_H

#include <libwccl/ops/function.h>
#include <libwccl/values/position.h>

namespace Wccl {

/**
 * Operator that takes a Position and an offset and returns relative
 * Position, shifted by the offset from the original one.
 */
class RelativePosition : public Function<Position> {
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;

	RelativePosition(const PosFunctionPtr& pos_expr, int offset)
		: pos_expr_(pos_expr),
		  offset_(offset)
	{
		BOOST_ASSERT(pos_expr_);
	}

	/**
	 * @returns String representation of the function in the form of:
	 * "arg_expr_string [+-] offset"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function: "relpos"
	 */
	std::string raw_name() const {
		return "relpos";
	}

protected:
	const PosFunctionPtr pos_expr_;
	const int offset_;

	/**
	 * Writes string representation of the function in the form of:
	 * "arg_expr_raw_string [+-] offset"
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;

	/**
	 * Takes the value of a Position from argument expression, and returns
	 * a Position relative to it, shifted by the offset that this
	 * RelativePosition object is representing.
	 * The result is not being trimmed to boundaries of the current
	 * sentence (a Position pointing outside of a sentence is still
	 * a valid Position).
	 * If "nowhere" is given, "nowhere" is returned (shifted "nowhere"
	 * still points to "nowhere").
	 * If "begin" or "end" are given, they are first represented as a normal
	 * Position Value (i.e. a Position relative to the current Position
	 * in the SentenceContext acted upon) which is then shifted normally.
	 * @returns Position that is shifted by the represented offset relative
	 * to the Position being passed as argument to Operator.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_POSITION_RELATIVEPOSITION_H
