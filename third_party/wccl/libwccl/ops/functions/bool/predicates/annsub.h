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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ANNSUB_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ANNSUB_H

#include <libwccl/ops/functions/bool/predicate.h>

namespace Wccl {

/**
 * An annotation-checking match condition
 */
class AnnSub : public Predicate
{
public:
	AnnSub(
		const boost::shared_ptr<Function<Match> >& check_from,
		const boost::shared_ptr<Function<Match> >& check_to,
		const std::string& annotation_name)
		: check_from_(check_from),
		  check_to_(check_to),
		  chan_name_(annotation_name)
	{
		BOOST_ASSERT(check_from);
		BOOST_ASSERT(check_to);
		BOOST_ASSERT(!annotation_name.empty());
	}

	AnnSub(
		const boost::shared_ptr<Function<Match> >& check_from_to,
		const std::string& annotation_name)
		: check_from_(check_from_to),
		  check_to_(check_from_to),
		  chan_name_(annotation_name)
	{
		BOOST_ASSERT(check_from_);
		BOOST_ASSERT(check_to_);
		BOOST_ASSERT(!annotation_name.empty());
	}


	/**
	 * @returns Name of the function
	 */
	std::string raw_name() const {
		return "annsub";
	}

	/**
	 * @returns String representation of the predicate
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	/**
	 * Outputs the string value of the returned value
	 * @returns True
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes string representation of the AnnSub to
	 * an output stream.
	 * @returns Stream written to.
	 * @note May be incomplete and/or containt internal info.
	 */
	std::ostream& write_to(std::ostream& ostream) const;

private:
	const boost::shared_ptr< const Function<Match> > check_from_;
	const boost::shared_ptr< const Function<Match> > check_to_;
	const std::string chan_name_;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_ANNSUB_H
