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

#include <libwccl/ops/match/conditions/tokencondition.h>
#include <libwccl/values/tokenmatch.h>

namespace Wccl {
namespace Matching {

TokenCondition::TokenCondition(const boost::shared_ptr<Function<Bool> >& predicate)
	: _predicate(predicate)
{
	BOOST_ASSERT(_predicate);
}

std::string TokenCondition::name() const
{
	return _predicate->raw_name();
}

std::string TokenCondition::to_string(const Corpus2::Tagset& tagset) const
{
	return _predicate->to_string(tagset);
}

MatchResult TokenCondition::apply(const ActionExecContext& context) const
{
	int orig_iter = context.sentence_context().get_position();
	if (context.sentence_context().is_current_inside()) {
		if (_predicate->apply(context)->get_value()) {
			boost::shared_ptr<TokenMatch> match(new TokenMatch(context.sentence_context().get_position()));
			// increase current sentence position by one after successful token match
			context.sentence_context().set_position(orig_iter + 1);
			return MatchResult(match);
		}
	}

	// else
	context.sentence_context().set_position(orig_iter);
	return MatchResult();
}

std::ostream& TokenCondition::write_to(std::ostream& os) const
{
	return os << *_predicate;
}

} /* end ns Matching */
} /* end ns Wccl */
