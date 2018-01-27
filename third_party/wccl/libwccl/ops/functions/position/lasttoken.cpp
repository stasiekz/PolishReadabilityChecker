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

#include <libwccl/ops/functions/position/lasttoken.h>
#include <libwccl/ops/functions/constant.h>
#include <libwccl/exception.h>
#include <sstream>

namespace Wccl {

LastToken::BaseRetValPtr LastToken::apply_internal(
	const FunExecContext &context) const
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> s = 
		boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(
			context.sentence_context().get_sentence_ptr());
	if (!s) {
		throw InvalidArgument(
			"context",
			"Supplied context does not have a valid Corpus2::AnnotatedSentence.");
	}
	const Function<Match>::RetValPtr match = match_expr_->apply(context);
	if(match->empty()) {
		return detail::DefaultFunction<Position>()->apply(context);
	}
	int abs_pos = match->last_token(s);
	int rel_pos = abs_pos - context.sentence_context().get_position();
	return boost::make_shared<Position>(rel_pos);
}

std::string LastToken::to_string(const Corpus2::Tagset &tagset) const
{
	std::ostringstream ostream;
	ostream << name(tagset) << "(" << match_expr_->to_string(tagset) << ")";
	return ostream.str();
}

std::ostream& LastToken::write_to(std::ostream& ostream) const
{
	return ostream << raw_name() << "(" << *match_expr_ << ")";
}

} /* end ns Wccl */
