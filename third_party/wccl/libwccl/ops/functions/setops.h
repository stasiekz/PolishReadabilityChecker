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

#ifndef LIBWCCL_OPS_FUNCTIONS_SETOPS_H
#define LIBWCCL_OPS_FUNCTIONS_SETOPS_H

#include <libwccl/ops/functions/listoperator.h>
#include <libwccl/values/tset.h>
#include <libwccl/values/strset.h>

namespace Wccl {
	typedef boost::shared_ptr<StrSet> StrSetPtr;

template<class T>
class SetListOperator : public ListOperator<T>
{
	BOOST_MPL_ASSERT(( boost::mpl::count<boost::mpl::list<StrSet, TSet>, T> ));
public:
	SetListOperator(const boost::shared_ptr<typename SetListOperator<T>::TFunctionPtrVector>& expressions)
		: ListOperator<T>(expressions)
	{
	}
};

/**
 * Set operation: union
 */
template<class T>
class SetUnion : public SetListOperator<T>
{
public:
	SetUnion(const boost::shared_ptr<typename SetListOperator<T>::TFunctionPtrVector>& expressions)
		: SetListOperator<T>(expressions)
	{
	}

	/**
	 * @returns Name of the function: "union".
	 */
	std::string raw_name() const {
		return "union";
	}

protected:
	FunctionBase::BaseRetValPtr apply_internal(const FunExecContext& context) const;
};

/**
 * Set operation: intersection
 */
template<class T>
class SetIntersection : public SetListOperator<T>
{
public:
	SetIntersection(const boost::shared_ptr<typename SetListOperator<T>::TFunctionPtrVector>& expressions)
		: SetListOperator<T>(expressions)
	{
	}

	/**
	 * @returns Name of the function: "intersection".
	 */
	std::string raw_name() const {
		return "intersection";
	}

protected:
	FunctionBase::BaseRetValPtr apply_internal(const FunExecContext& context) const;
};

//
// ----- Implementation -----
//

template <> inline
FunctionBase::BaseRetValPtr SetUnion<TSet>::apply_internal(const FunExecContext& context) const {
	Corpus2::Tag out;
	BOOST_FOREACH (boost::shared_ptr< Function<TSet> > expression, *expressions_) {
		Corpus2::Tag s = expression->apply(context)->get_value();
		out.combine_with(s);
	}
	return boost::make_shared<TSet>(out);
}

template <> inline
FunctionBase::BaseRetValPtr SetUnion<StrSet>::apply_internal(const FunExecContext& context) const {
	if (expressions_->size() == 1) return expressions_->front()->apply(context);
	boost::shared_ptr<StrSet> out = boost::make_shared<StrSet>();
	if (expressions_->empty()) return out;
	const boost::shared_ptr<const StrSet>& set1 = (*expressions_)[0]->apply(context);
	out->contents() = set1->contents();

	for (size_t i = 1; i < expressions_->size(); ++i) {
		const boost::shared_ptr<const StrSet>& seti = (*expressions_)[i]->apply(context);
		BOOST_FOREACH (const UnicodeString& s, seti->contents()) {
			out->insert(s);
		}
	}
	return out;
}

template <> inline
FunctionBase::BaseRetValPtr SetIntersection<TSet>::apply_internal(const FunExecContext& context) const {
	Corpus2::Tag out;
	if (!expressions_->empty()) {
		out = (*expressions_)[0]->apply(context)->get_value();
		for (size_t i = 1; i < expressions_->size(); ++i) {
			Corpus2::Tag s = (*expressions_)[i]->apply(context)->get_value();
			out.mask_with(s);
		}
	}
	return boost::make_shared<TSet>(out);
}

template <> inline
FunctionBase::BaseRetValPtr SetIntersection<StrSet>::apply_internal(const FunExecContext& context) const {
	if (expressions_->size() == 1) return expressions_->front()->apply(context);
	boost::shared_ptr<StrSet> out = boost::make_shared<StrSet>();
	if (expressions_->empty()) return out;


	//find smallest set
	size_t smallest = 0;
	size_t smallest_size = (std::numeric_limits<size_t>::max)();
	for (size_t i = 0; i < expressions_->size(); ++i) {
		const boost::shared_ptr<const StrSet>& set1 = (*expressions_)[i]->apply(context);
		size_t ssize = set1->size();
		if(ssize < smallest_size){
			smallest = i;
			smallest_size = ssize;
		}
	}

	const boost::shared_ptr<const StrSet>& smallest_set = (*expressions_)[smallest]->apply(context);
	//for each element in smallest set
	BOOST_FOREACH (const UnicodeString& s, smallest_set->contents()) {
		bool everywhere = true;
		// find if every other set contains this element
		for (size_t i = 0; i < expressions_->size() && everywhere; ++i) {
			if (i == smallest)
				continue;
			const boost::shared_ptr<const StrSet>& set1 = (*expressions_)[i]->apply(context);

			if(set1->contents().find(s) == set1->contents().end())
				everywhere = false;
		}
		if(everywhere)
			out->insert(s);
	}




	return out;
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_SETOPS_H
