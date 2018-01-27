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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISSINGULAR_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISSINGULAR_H

#include <libwccl/ops/functions/bool/predicate.h>
#include <libwccl/values/tset.h>

namespace Wccl {

/**
 * Predicate that checks for tag singularity: at most one value
 * chosen for each attribute.
 */
class IsSingular : public Predicate
{
public:
	typedef boost::shared_ptr<Function<TSet> > ArgFunctionPtr;

	IsSingular(const ArgFunctionPtr& arg_expr, const boost::shared_ptr<const Corpus2::Tagset> tagset)
		: arg_expr_(arg_expr), tagset_(tagset)
	{
		BOOST_ASSERT(arg_expr_);
	}

	/**
	 * @returns String representation of the function
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function
	 */
	std::string raw_name() const {
		return "singular";
	}

protected:
	const ArgFunctionPtr arg_expr_;
	const boost::shared_ptr<const Corpus2::Tagset> tagset_;

	/**
	 * Take value of argument and return True if it is empty, False otherwise.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of the function
	 * @note This version does not require tagset but may be incomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ISSINGULAR_H
