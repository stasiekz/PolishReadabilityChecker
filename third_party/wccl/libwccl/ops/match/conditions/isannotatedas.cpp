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

#include <libwccl/ops/match/conditions/isannotatedas.h>
#include <sstream>

namespace Wccl {
namespace Matching {

MatchResult IsAnnotatedAs::apply(const ActionExecContext& context) const
{
	SentenceContext& sc = context.sentence_context();
	boost::shared_ptr<Corpus2::AnnotatedSentence> as;
	as = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(sc.get_sentence_ptr());
	if (!as) {
		throw InvalidArgument("context", "Operator needs an annotated sentence.");
	}
	if (!as->has_channel(chan_name_)) {
		return MatchResult();
	}

	int orig_iter = sc.get_position();
	const Corpus2::AnnotationChannel& channel = as->get_channel(chan_name_);
	int segment_idx = channel.get_segment_at(orig_iter);

	// are we even within a segment annotaded with given annotation?
	if (segment_idx == 0) {
		return MatchResult();
	}
	// ok, so are we at the beginning of the segment?
	if (channel.get_segment_at(orig_iter - 1) == segment_idx) {
		return MatchResult();
	}
	// we are at the beginning of a segment with given annotation, so match it (continuous fragment at least).
	boost::shared_ptr<AnnotationMatch> ann_match(
		new AnnotationMatch(orig_iter, chan_name_));
	int segment_length = 1;
	for (
			int i = orig_iter + 1;
			(i < sc.size()) && (channel.get_segment_at(i) == segment_idx);
			++i
		) {
		++segment_length;
	}
	// increase current sentence position to point after the matched segment
	sc.set_position(orig_iter + segment_length);
	return MatchResult(ann_match);
}

std::string IsAnnotatedAs::to_string(const Corpus2::Tagset& /*tagset*/) const
{
	std::ostringstream os;
	os << name() << "(\"" << chan_name_ << "\")";
	return os.str();
}

std::ostream& IsAnnotatedAs::write_to(std::ostream& os) const
{
	return os << name() << "(\"" << chan_name_ << "\")";
}

} /* end ns Matching */
} /* end ns Wccl */
