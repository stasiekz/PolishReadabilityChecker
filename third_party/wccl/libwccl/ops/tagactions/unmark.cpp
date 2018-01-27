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

#include <libwccl/ops/tagactions/unmark.h>
#include <boost/foreach.hpp>
#include <libcorpus2/ann/annotatedsentence.h>
#include <sstream>

namespace Wccl {

Bool Unmark::execute(const ActionExecContext& context) const
{
	SentenceContext& sc = context.sentence_context();

	const boost::shared_ptr<const Position>& position = pos_->apply(context);
	if (position->get_value() == Position::Nowhere) {
		return Bool(false);
	}

	int abs_pos = sc.get_abs_position(*position);
	if (!sc.is_inside(abs_pos)) {
		return Bool(false);
	}

	boost::shared_ptr<Corpus2::AnnotatedSentence> as;
	as = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(sc.get_sentence_ptr());
	if (!as) {
		throw WcclError("Operator needs an annotated sentence");
	}
	if (!as->has_channel(chan_name_)) {
		return Bool(false);
	}
	Corpus2::AnnotationChannel& channel = as->get_channel(chan_name_);

	int segment_idx = channel.get_segment_at(abs_pos);
	if (segment_idx == 0) {
		return Bool(false);
	}

	for (int i = 0; i < channel.size(); ++i) {
		if (channel.segments()[i] == segment_idx) {
			channel.set_segment_at(i, 0);
			channel.set_head_at(i, false);
		}
	}
	return Bool(true);
}

std::string Unmark::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << name() << "(" << pos_->to_string(tagset) << ", \""
			<< chan_name_ << "\")";
	return os.str();
}

std::ostream& Unmark::write_to(std::ostream& os) const
{
	os << name() << "(" << *pos_ << ", \"" << chan_name_ << "\")";
	return os;
}


} /* end ns Wccl */
