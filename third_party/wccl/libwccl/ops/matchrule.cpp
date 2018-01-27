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

#include <libwccl/sentencecontext.h>
#include <libwccl/ops/match/applyoperator.h>
#include <libwccl/ops/matchrule.h>

#include <sstream>

namespace Wccl {
namespace Matching {

void MatchRule::apply(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s)
{
	if(!s) {
		throw InvalidArgument("s", "Received a null sentence.");
	}
	if(s->size() == 0) {
		throw InvalidArgument("s", "Received an empty sentence.");
	}
	if (!apply_) {
		return; // no-op (default) version
	}
	SentenceContext sc(s);
	ActionExecContext aec(sc, variables_);
	apply_->execute(aec);
}

std::string MatchRule::to_string(const Corpus2::Tagset &tagset) const
{
	std::ostringstream os;
	os << apply_->to_string(tagset);
	return os.str();
}

std::ostream& MatchRule::write_to(std::ostream& os) const
{
	return os << *apply_;
}

} /* end ns Matching */
} /* end ns Wccl */
