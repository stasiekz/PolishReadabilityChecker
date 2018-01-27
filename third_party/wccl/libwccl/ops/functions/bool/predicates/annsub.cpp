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

#include <libwccl/ops/functions/bool/predicates/annsub.h>
#include <libwccl/values/match.h>
#include <libcorpus2/ann/annotatedsentence.h>

namespace Wccl {

AnnSub::BaseRetValPtr AnnSub::apply_internal(const FunExecContext& context) const
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> as
		= boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(
			context.sentence_context().get_sentence_ptr());
	if (!as) {
		throw InvalidArgument("context", "Operator needs an annotated sentence.");
	}

	boost::shared_ptr<const Match> check_from = check_from_->apply(context);
	boost::shared_ptr<const Match> check_to =
		(check_from_ == check_to_) ? check_from : check_to_->apply(context);
	int abs_left = check_from->first_token(as);
	if (abs_left < 0) {
		throw WcclError("Received starting match that points outside sentence.");
	}
	int abs_right = check_to->last_token(as);
	if (abs_right >= context.sentence_context().size()) {
		throw WcclError("Received ending match that points outside sentence.");
	}
	if (abs_left > abs_right) {
		throw WcclError("Received starting match points after the received ending match.");
	}
	if (!as->has_channel(chan_name_)) {
		as->create_channel(chan_name_);
	}
	Corpus2::AnnotationChannel& channel = as->get_channel(chan_name_);

	int segment_idx = channel.get_segment_at(abs_left);
	if (segment_idx == 0) {
		return Predicate::False(context);
	} else {
		for (int i = abs_left + 1; i <= abs_right; ++i) {
			if (segment_idx != channel.get_segment_at(i)) {
				return Predicate::False(context);
			}
		}
	}
	return Predicate::True(context);
}

std::string AnnSub::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream ostream;
	ostream << raw_name() << "(" << check_from_->to_string(tagset);
	if (check_from_ != check_to_) {
		ostream << ", " << check_to_->to_string(tagset);
	}
	ostream << ", \"" << chan_name_ << "\")";
	return ostream.str();
}

std::ostream& AnnSub::write_to(std::ostream& ostream) const
{
	ostream << raw_name() << "(" << *check_from_;
	if (check_from_ != check_to_) {
		ostream << ", " << *check_to_;
	}
	ostream << ", \"" << chan_name_ << "\")";
	return ostream;
}

} /* end ns Wccl */
