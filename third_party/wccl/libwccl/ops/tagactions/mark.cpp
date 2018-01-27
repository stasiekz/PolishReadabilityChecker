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

#include <libwccl/ops/tagactions/mark.h>
#include <boost/foreach.hpp>
#include <libcorpus2/ann/annotatedsentence.h>
#include <sstream>

namespace Wccl {

Bool Mark::execute(const ActionExecContext& context) const
{
	SentenceContext& sc = context.sentence_context();

	const boost::shared_ptr<const Position>& range_left = pos_begin_->apply(context);
	if (range_left->get_value() == Position::Nowhere) {
		return Bool(false);
	}
	const boost::shared_ptr<const Position>& range_right = pos_end_->apply(context);
	if (range_right->get_value() == Position::Nowhere) {
		return Bool(false);
	}
	int abs_left, abs_right;
	if (!sc.validate_range(*range_left, *range_right, abs_left, abs_right)) {
		return Bool(false);
	}

	int abs_head;
	if (pos_head_) {
		const boost::shared_ptr<const Position>& head = pos_head_->apply(context);
		if (head->get_value() == Position::Nowhere) {
			return Bool(false);
		}
		abs_head = sc.get_abs_position(*head);
		if (abs_head < abs_left || abs_head > abs_right) {
			return Bool(false);
		}
	} else {
		abs_head = abs_left;
	}

	boost::shared_ptr<Corpus2::AnnotatedSentence> as;
	as = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(sc.get_sentence_ptr());
	if (!as) {
		throw WcclError("Operator needs an annotated sentence");
	}
	if (!as->has_channel(chan_name_)) {
		as->create_channel(chan_name_);
	}
	Corpus2::AnnotationChannel& channel = as->get_channel(chan_name_);

	int segment_idx = channel.get_new_segment_index();
	//std::cerr << "Marking " << chan_name_ << " from " << abs_left << " to "
	//	<< abs_right << " as " << segment_idx << "\n";

	for (int i = abs_left; i <= abs_right; ++i) {
		if (channel.get_segment_at(i) > 0) {
			//std::cerr << "There already is an annotation at " << i
			//	<< " (" << channel.get_segment_at(i) << ")\n";
			//throw WcclError("Mark action would overwrite existing annotation");
			return Bool(false);
		}
	}

	for (int i = abs_left; i <= abs_right; ++i) {
		channel.set_segment_at(i, segment_idx);
		channel.set_head_at(i, false);
	}
	channel.set_head_at(abs_head, true);
	return Bool(true);
}

std::string Mark::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << name() << "(" << pos_begin_->to_string(tagset) << ", "
			<< pos_end_->to_string(tagset);
	if (pos_head_) {
		os << ", " << pos_head_->to_string(tagset);
	}
	os << ", \"" << chan_name_ << "\")";
	return os.str();
}

std::ostream& Mark::write_to(std::ostream& os) const
{
	os << name() << "(" << *pos_begin_ << ", " << *pos_end_;
	if (pos_head_) {
		os << ", " << *pos_head_ ;
	}
	os << ", \"" << chan_name_ << "\")";
	return os;
}


} /* end ns Wccl */
