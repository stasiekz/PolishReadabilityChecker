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

#include <libwccl/values/annotationmatch.h>
#include <libwccl/values/position.h>
#include <boost/lexical_cast.hpp>

namespace Wccl {

std::string AnnotationMatch::to_raw_string() const
{
	return "ANN[" + boost::lexical_cast<std::string>(abs_pos_) + "," + channel_ + "]";
}

int AnnotationMatch::first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const
{
	const Corpus2::AnnotationChannel& chan = s->get_channel(channel_);
	int seg = chan.get_segment_at(abs_pos_);
	if (seg > 0) {
		for (int i = 0; i < abs_pos_; ++i) {
			if (chan.get_segment_at(i) == seg) {
				return i;
			}
		}
		return abs_pos_;
	} else {
		return Position::Nowhere;
	}
}

int AnnotationMatch::last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const
{
	const Corpus2::AnnotationChannel& chan = s->get_channel(channel_);
	int seg = chan.get_segment_at(abs_pos_);
	if (seg > 0) {
		for (int i = s->size() - 1; i > abs_pos_; --i) {
			if (chan.get_segment_at(i) == seg) {
				return i;
			}
		}
		return abs_pos_;
	} else {
		return Position::Nowhere;
	}
}


} /* end ns Wccl */
