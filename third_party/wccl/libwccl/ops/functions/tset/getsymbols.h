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

#ifndef LIBWCCL_OPS_FUNCTIONS_TSET_GETSYMBOLS_H
#define LIBWCCL_OPS_FUNCTIONS_TSET_GETSYMBOLS_H

#include <libwccl/values/tset.h>
#include <libwccl/values/position.h>
#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Operator that gets tagset symbols from a token at given position.
 */
class GetSymbols : public Function<TSet>
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;

	GetSymbols(const Corpus2::Tag& mask, const PosFunctionPtr& pos_expr)
		: mask_(mask), pos_expr_(pos_expr)
	{
		BOOST_ASSERT(pos_expr_);
	}

	/**
	 * @returns String representation of the function in the form of:
	 * "attribute_name[pos_expr_string]"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	std::string raw_name() const;

	std::string name(const Corpus2::Tagset& tagset) const;

protected:
	Corpus2::Tag mask_;

	const PosFunctionPtr pos_expr_;

	/**
	 * Gets a position from the argument expression, then gets the
	 * word at that position from the Sentence in the SentenceContext,
	 * then gets the tagset symbols matching the mask of the word.
	 *
	 * @returns A tagset symbol set of the word if position pointed to
	 * lies within boundaries of the Sentence. Empty Tset otherwise.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of the function in the form of:
	 * "raw_attribute_name[pos_expr_raw_string]"
	 * @note This version does not require tagset, but will be inclomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_TSET_GETSYMBOLS_H
