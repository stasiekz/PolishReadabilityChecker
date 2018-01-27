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

#include <libwccl/ops/match/conditions/matchtext.h>
#include <sstream>
#include <libpwrutils/util.h>


namespace Wccl {
namespace Matching {

MatchResult MatchText::apply(const ActionExecContext& context) const
{
	SentenceContext& sc = context.sentence_context();

	int orig_iter = sc.get_position();
	
	UnicodeString sent_frag(sc.at(orig_iter)->orth());
	int iter_pos = orig_iter + 1;
	while(sent_frag.length() < text_.length() && iter_pos < sc.size()) {
		if (sc.at(iter_pos)->wa() != PwrNlp::Whitespace::None) {
			sent_frag += " ";
		}
		sent_frag += sc.at(iter_pos)->orth();
		++iter_pos;
	}

	if (sent_frag != text_) {
		return MatchResult();
	}

	boost::shared_ptr<MatchVector> v(new MatchVector());
	for(int i = orig_iter; i < iter_pos; ++i) {
		v->append(boost::shared_ptr<TokenMatch>(new TokenMatch(i)));
	}
	// increase current sentence position to the point after the matched tokens.
	sc.set_position(orig_iter + v->size());
	return MatchResult(v);
}

std::string MatchText::to_string(const Corpus2::Tagset&) const
{
	std::ostringstream os;
	os << name() << "(\"" << PwrNlp::to_utf8(text_) << "\")";
	return os.str();
}

std::ostream& MatchText::write_to(std::ostream& os) const
{
	return os << name() << "(\"" << PwrNlp::to_utf8(text_) << "\")";
}

} /* end ns Matching */
} /* end ns Wccl */
