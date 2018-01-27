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

#include <libwccl/ops/match/conditions/conjconditions.h>
#include <libwccl/values/matchvector.h>
#include <boost/foreach.hpp>
#include <sstream>

namespace Wccl {
namespace Matching {

ConjConditions::ConjConditions(const std::vector< boost::shared_ptr<const MatchCondition> >& conditions)
	: _conditions(conditions)
{
	BOOST_ASSERT(_conditions.size() > 0);
}

MatchResult ConjConditions::apply(const ActionExecContext& context) const
{
	boost::shared_ptr<MatchVector> matches(new MatchVector());
	int orig_pos = context.sentence_context().get_position();

	BOOST_FOREACH (const boost::shared_ptr<const MatchCondition>& cond, _conditions) {
		MatchResult res = cond->apply(context);
		if(res.matched()) {
			matches->append(res.get_match());
		} else {
			context.sentence_context().set_position(orig_pos);
			return MatchResult();
		}
	}
	return MatchResult(matches);
}

std::string ConjConditions::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream ostream;
	ostream << "(";
	for(size_t i = 0; i < _conditions.size(); ++i) {
		if (i != 0) {
			ostream << ", ";
		}
		ostream << _conditions[i]->to_string(tagset);
	}
	ostream << ")";
	return ostream.str();
}

std::ostream& ConjConditions::write_to(std::ostream &ostream) const
{
	ostream << "(";
	for(size_t i = 0; i < _conditions.size(); ++i) {
		if (i != 0) {
			ostream << ", ";
		}
		ostream << *_conditions[i];
	}
	ostream << ")";
	return ostream;
}

} /* end ns Matching */
} /* end ns Wccl */
