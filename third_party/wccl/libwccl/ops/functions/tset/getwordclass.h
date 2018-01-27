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

#ifndef LIBWCCL_OPS_FUNCTIONS_TSET_GETWORDCLASS_H
#define LIBWCCL_OPS_FUNCTIONS_TSET_GETWORDCLASS_H

#include <libwccl/ops/functions/tset/getsymbols.h>

namespace Wccl {

/**
 * Operator that gets word class/part of speech symbols from
 * a token at given position.
 */
class GetWordClass : public GetSymbols {
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;

	GetWordClass(const PosFunctionPtr& pos_expr)
		: GetSymbols(Corpus2::Tag::all_pos_mask, pos_expr)
	{
	}

	/**
	 * @returns Name of the operator, "class"
	 */
	std::string name(const Corpus2::Tagset&) const {
		return "class";
	}
	/**
	 * @returns Name of the operator, "class"
	 */
	std::string raw_name() const {
		return "class";
	}
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_TSET_GETWORDCLASS_H
