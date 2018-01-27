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

#ifndef LIBWCCL_OPS_FUNCTIONS_LISTOPERATOR_H
#define LIBWCCL_OPS_FUNCTIONS_LISTOPERATOR_H

#include <libwccl/ops/function.h>
#include <libwccl/ops/formatters.h>

namespace Wccl {

/**
 * Abstract base class for operators over any number of arguments
 * (of the same type)
 */
template<class T>
class ListOperator : public Function<T>
{
public:
	typedef boost::shared_ptr<Function<T> > TFunctionPtr;
	typedef std::vector<TFunctionPtr> TFunctionPtrVector;

	ListOperator(const boost::shared_ptr<TFunctionPtrVector>& expressions)
		: expressions_(expressions)
	{
		BOOST_ASSERT(expressions_);
		BOOST_ASSERT(expressions_->size() > 0);
	}

	/**
	 * @returns String representation of the logical predicate, realised by default
	 * as "operator_name(expr1_string, ..., exprn_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	const boost::shared_ptr<TFunctionPtrVector> expressions_;

	/**
	 * Writes raw string representation of the operator, realised by default
	 * as "raw_operator_name(raw_expr1_string, ..., raw_exprn_string)"
	 * @note This version doesn't require tagset, but may be incomplete and/or
	 * contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
};

//
// ----- Implementation -----
//

template<class T> inline
std::string ListOperator<T>::to_string(const Corpus2::Tagset& tagset) const
{
	std::stringstream ss;
	ss << this->name(tagset) << "(";
	typename TFunctionPtrVector::const_iterator it = expressions_->begin();
	while(it != expressions_->end()) {
		ss << (*it)->to_string(tagset);
		if(++it != expressions_->end()) {
			ss << ", ";
		}
	}
	ss << ")";
	return ss.str();
}

template<class T> inline
std::ostream& ListOperator<T>::write_to(std::ostream& ostream) const
{
	ostream << this->raw_name() << "(";
	typename TFunctionPtrVector::const_iterator it = expressions_->begin();
	while(it != expressions_->end()) {
		ostream << (*it)->to_raw_string();
		if(++it != expressions_->end()) {
			ostream << ", ";
		}
	}
	ostream << ")";
	return ostream;
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_LISTOPERATOR_H
