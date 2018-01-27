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

#include <libwccl/ops/match/conditions/optionalmatch.h>
#include <libwccl/values/matchvector.h>
#include <sstream>

namespace Wccl {
namespace Matching {

OptionalMatch::OptionalMatch(const boost::shared_ptr<ConjConditions>& conditions)
	: _conditions(conditions)
{
	BOOST_ASSERT(_conditions);
}

MatchResult OptionalMatch::apply(const ActionExecContext& context) const
{
	int orig_pos = context.sentence_context().get_position();
	MatchResult res = _conditions->apply(context);
	if (res.matched()) {
		return res;
	}
	context.sentence_context().set_position(orig_pos);
	return MatchResult(boost::make_shared<Match>());
}

std::string OptionalMatch::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream ostream;
	ostream << name() << _conditions->to_string(tagset);
	return ostream.str();
}

std::ostream& OptionalMatch::write_to(std::ostream& ostream) const
{
	return ostream << name() << *_conditions;
}

} /* end ns Matching */
} /* end ns Wccl */
