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

#include <libwccl/ops/functions/strset/anninter.h>
#include <libcorpus2/ann/annotatedsentence.h>

#include <libpwrutils/util.h>

namespace Wccl {

AnnInter::BaseRetValPtr AnnInter::apply_internal(const FunExecContext& context) const
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> as
		= boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(
			context.sentence_context().get_sentence_ptr());
	if (!as) {
		throw InvalidArgument("context", "Operator needs an annotated sentence.");
	}

	const boost::shared_ptr<const Position>& pos = pos_expr_->apply(context);
	const SentenceContext& sc = context.sentence_context();
	// prepare the output set
	boost::shared_ptr<StrSet > out_set = boost::make_shared<StrSet>();
	// return empty set if out of bounds
	int abs_idx = sc.get_abs_position(*pos);

	if (sc.is_outside(*pos)) {
		return out_set;
	}

	// get the value of the set of channel names to consider
	const boost::shared_ptr<const StrSet >& in_set = strset_expr_->apply(context);

	BOOST_FOREACH (const UnicodeString& u_chan_name, in_set->contents()) {
		// check if there is any annot at pos
		const std::string chan_name(PwrNlp::to_utf8(u_chan_name));
		if (as->has_channel(chan_name)) {
			const Corpus2::AnnotationChannel& chan = as->get_channel(chan_name);
			if (chan.get_segment_at(abs_idx) != 0) {
				// there is an annotation, so we take this channel's name
				out_set->insert(u_chan_name);
			}
		}
	}

	return out_set;
}

std::string AnnInter::to_string(const Corpus2::Tagset&) const
{
	std::ostringstream ostream;
	ostream << raw_name() << "(" << *pos_expr_
			<< ", " << *strset_expr_ << ")";
	return ostream.str();
}

std::ostream& AnnInter::write_to(std::ostream& ostream) const
{
	ostream << raw_name() << "(" << *pos_expr_ << ", "
			<< ", " << *strset_expr_ << ")";
	return ostream;
}

} /* end ns Wccl */
