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

#include <libwccl/values/match.h>
#include <libwccl/ops/match/actions/markmatch.h>

#include <sstream>


namespace Wccl {
namespace Matching {

void MarkBase::execute_mark(const ActionExecContext& context,
							bool may_overwrite) const
{
	// TODO may overwrite, use it in mark_mark and perhaps move fields back to private
	SentenceContext& sc = context.sentence_context();
	boost::shared_ptr<Corpus2::AnnotatedSentence> as;
	as = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(sc.get_sentence_ptr());
	if (!as) {
		throw InvalidArgument("context", "Operator needs an annotated sentence.");
	}

	boost::shared_ptr<const Match> match_from = match_from_->apply(context);
	boost::shared_ptr<const Match> match_to =
		(match_from_ == match_to_) ? match_from : match_to_->apply(context);
	boost::shared_ptr<const Match> head_match =
		(match_from_ == head_match_) ? match_from : head_match_->apply(context);

	int abs_left = match_from->first_token(as);
	if (abs_left < 0) {
		throw WcclError("Received starting match that points outside sentence.");
	}

	int abs_right = match_to->last_token(as);
	if (abs_right >= sc.size()) {
		throw WcclError("Received ending match that points outside sentence.");
	}
	if (abs_left > abs_right) {
		throw WcclError("Received starting match points after the received ending match.");
	}

	int abs_head = head_match->first_token(as);
	if (abs_head < abs_left || abs_head > abs_right) {
		throw WcclError("Received head match points outside range defined by start and end matches.");
	}

	if (!as->has_channel(chan_name_)) {
		as->create_channel(chan_name_);
	}
	Corpus2::AnnotationChannel& channel = as->get_channel(chan_name_);

	int segment_idx = channel.get_new_segment_index();

	if (may_overwrite) {
		std::set<int> segments_to_unmark;
		for (int i = abs_left; i <= abs_right; ++i) {
			if (channel.get_segment_at(i) > 0) {
				segments_to_unmark.insert(channel.get_segment_at(i));
			}
		}
		if (!segments_to_unmark.empty()) {
			// unmark all the segments that appeared within the range
			for (int i = 0; i < channel.size(); ++i) {
				if (segments_to_unmark.find(channel.segments()[i]) !=
						segments_to_unmark.end()) {
					channel.set_segment_at(i, 0);
					channel.set_head_at(i, false);
				}
			}
		}
	}
	else {
		for (int i = abs_left; i <= abs_right; ++i) {
			if (channel.get_segment_at(i) > 0) {
				throw WcclError("Mark action would overwrite existing annotation");
			}
		}
	}
	// mark new one
	for (int i = abs_left; i <= abs_right; ++i) {
		channel.set_segment_at(i, segment_idx);
		channel.set_head_at(i, false);
	}
	channel.set_head_at(abs_head, true);
}

std::string MarkBase::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << name() << "("
			<< match_from_->to_string(tagset) << ", ";
	if (match_from_.get() != match_to_.get()) {
		os << match_to_->to_string(tagset) << ", ";
	}
	os << "\"" << chan_name_ << "\")";
	return os.str();
}

std::ostream& MarkBase::write_to(std::ostream& os) const
{
	os << name() << "("
			<< *match_from_ << ", ";
	if (match_from_.get() != match_to_.get()) {
		os << *match_to_ << ", ";
	}
	os << "\"" << chan_name_ << "\")";
	return os;
}

} /* end ns Matching */
} /* end ns Wccl */
