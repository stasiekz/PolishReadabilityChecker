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

#ifndef LIBWCCL_OPS_FUNCTIONS_STRSET_PROPVAL_H
#define LIBWCCL_OPS_FUNCTIONS_STRSET_PROPVAL_H

#include <libwccl/values/position.h>
#include <libwccl/values/strset.h>
#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Operator that takes a position and a set of strings denoting property keys
 * and return union of property values obtained via given keys from the
 * metadata assigned to a token occupying the given position.
 * If position out of sentence boundaries, will return an empty set. Also,
 * if some of the given keys are not present in the metadata (or no metadata
 * present at all at the given position), no error is raised but the keys
 * are silently ignored. This may result in empty value set return.
 */
class PropVal : public Function<StrSet>
{
public:
	typedef boost::shared_ptr<Function<Position> > PosFunctionPtr;
	typedef boost::shared_ptr<Function<StrSet> > StrSetFunctionPtr;
	
	PropVal(const PosFunctionPtr& pos_expr,
			const StrSetFunctionPtr& strset_expr)
		: pos_expr_(pos_expr),
		  strset_expr_(strset_expr)
	{
		BOOST_ASSERT(pos_expr_);
		BOOST_ASSERT(strset_expr_);
	}

	/**
	 * String representation of the operator in form of:
	 * "prop(pos_expr_string, strset_expr_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function: "affix"
	 */
	std::string raw_name() const {
		return "prop";
	}

protected:
	const PosFunctionPtr pos_expr_;
	const StrSetFunctionPtr strset_expr_;

	/**
	 * Get a string set that aggregates all property values retrieved from
	 * the given position using the given key set.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;
	
	/**
	 * Writes raw string representation of the operator in form of:
	 * "prop(pos_expr_string, strset_expr_raw_string)"
	 * @note This version doesn't require tagset, but may be incomplete and/or
	 * contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_STRSET_PROPVAL_H
