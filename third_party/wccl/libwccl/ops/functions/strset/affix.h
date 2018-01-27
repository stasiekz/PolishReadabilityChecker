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

#ifndef LIBWCCL_OPS_FUNCTIONS_STRSET_AFFIX_H
#define LIBWCCL_OPS_FUNCTIONS_STRSET_AFFIX_H

#include <libwccl/values/strset.h>
#include <libwccl/ops/function.h>

namespace Wccl {

/**
 * Operator that takes a set of strings and returns a new
 * set with corresponding values that are prefixes or
 * suffixes of given length
 */
class Affix : public Function<StrSet>
{
public:
	typedef boost::shared_ptr<Function<StrSet> > StrSetFunctionPtr;
	
	Affix(const StrSetFunctionPtr& strset_expr, int affix_length)
		: strset_expr_(strset_expr),
		  affix_length_(affix_length)
	{
		BOOST_ASSERT(strset_expr_);
	}

	/**
	 * String representation of the operator in form of:
	 * "affix(strset_expr_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function: "affix"
	 */
	std::string raw_name() const {
		return "affix";
	}

protected:
	const StrSetFunctionPtr strset_expr_;
	const int affix_length_;

	/**
	 * Get a string set from the argument expression and return copy of the set
	 * with all strings converted into prefixes or suffixes of given length
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;
	
	/**
	 * Writes raw string representation of the operator in form of:
	 * "affix(strset_expr_raw_string)"
	 * @note This version doesn't require tagset, but may be incomplete and/or
	 * contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_STRSET_AFFIX_H
