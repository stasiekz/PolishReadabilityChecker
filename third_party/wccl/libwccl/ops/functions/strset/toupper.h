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

#ifndef LIBWCCL_OPS_FUNCTIONS_STRSET_TOUPPER_H
#define LIBWCCL_OPS_FUNCTIONS_STRSET_TOUPPER_H

#include <libwccl/values/strset.h>
#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Operator that takes a set of strings and returns a new
 * set with corresponding values in upper case form
 */
class ToUpper : public Function<StrSet> {
public:
	typedef boost::shared_ptr<Function<StrSet> > StrSetFunctionPtr;
	
	ToUpper(const StrSetFunctionPtr& strset_expr)
		: strset_expr_(strset_expr)
	{
		BOOST_ASSERT(strset_expr_);
	}

	/**
	 * @returns String representation of the function in the form of:
	 * "upper(strset_expr_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function: "upper"
	 */
	std::string raw_name() const {
		return "upper";
	}
protected:
	const StrSetFunctionPtr strset_expr_;

	/**
	 * Get a string set from the argument expression and return copy of the set
	 * with all strings in upper case form
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of the function in the form of:
	 * "upper(strset_expr_raw_string)"
	 * @note This version does not require tagset, but may be inclomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_STRSET_TOUPPER_H
