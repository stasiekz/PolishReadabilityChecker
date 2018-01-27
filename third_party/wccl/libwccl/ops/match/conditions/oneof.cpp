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

#include <libwccl/ops/match/conditions/oneof.h>
#include <libwccl/values/matchvector.h>
#include <sstream>
#include <boost/foreach.hpp>

namespace Wccl {
namespace Matching {

OneOf::OneOf(const boost::shared_ptr<std::vector<boost::shared_ptr<ConjConditions> > >& variants)
	: _variants(variants)
{
	BOOST_ASSERT(_variants);
	BOOST_ASSERT(!_variants->empty());
}

MatchResult OneOf::apply(const ActionExecContext& context) const
{
	int orig_pos = context.sentence_context().get_position();
	BOOST_FOREACH (const boost::shared_ptr<ConjConditions>& variant, *_variants) {
		MatchResult res = variant->apply(context);
		if (res.matched()) {
			return res;
		}
		context.sentence_context().set_position(orig_pos);
	}
	return MatchResult();
}

std::string OneOf::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream ostream;
	ostream << name() << "(";
	for (size_t i = 0; i < _variants->size(); ++i) {
		if (i != 0) {
			ostream << ", ";
		}
		ostream << "variant" << _variants->at(i)->to_string(tagset);
	}
	ostream << ")";
	return ostream.str();
}

std::ostream& OneOf::write_to(std::ostream& ostream) const
{
	ostream << name() << "(";
	for (size_t i = 0; i < _variants->size(); ++i) {
		if (i != 0) {
			ostream << ", ";
		}
		ostream << "variant" << *(_variants->at(i));
	}
	return ostream << ")";
}

} /* end ns Matching */
} /* end ns Wccl */
