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

#include <libwccl/ops/functions/bool/predicates/anntoken.h>
#include <libcorpus2/ann/annotatedsentence.h>

namespace Wccl {

AnnToken::BaseRetValPtr AnnToken::apply_internal(const FunExecContext& context) const
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> as
		= boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(
			context.sentence_context().get_sentence_ptr());
	if (!as) {
		throw InvalidArgument("context", "Operator needs an annotated sentence.");
	}

	const boost::shared_ptr<const Position>& pos = pos_expr_->apply(context);
	const SentenceContext& sc = context.sentence_context();
	// return False if out of bounds
	if (sc.is_outside(*pos)) {
		return Predicate::False(context);
	}
	// return False if no such channel in sent
	if (!as->has_channel(chan_name_)) {
		return Predicate::False(context);
	}
	// channel exists, position in range, check if any annot at pos
	int abs_idx = sc.get_abs_position(*pos);

	const Corpus2::AnnotationChannel& chan = as->get_channel(chan_name_);

	if (chan.get_segment_at(abs_idx) == 0) {
		return Predicate::False(context);
	}
	else {
		if (tok_constr_ == O_ANY) {
			return Predicate::True(context);
		}
		if (tok_constr_ == O_HEAD) {
			return chan.is_head_at(abs_idx) ?
					Predicate::True(context) : Predicate::False(context);
		}
		if (tok_constr_ == O_FIRST) {
			return (chan.get_segment_at(abs_idx) !=
					chan.get_segment_at(abs_idx - 1)) ?
					Predicate::True(context) : Predicate::False(context);
		}
		if (tok_constr_ == O_LAST) {
			return (chan.get_segment_at(abs_idx) !=
					chan.get_segment_at(abs_idx + 1)) ?
					Predicate::True(context) : Predicate::False(context);
		}
	}
}

std::string AnnToken::to_string(const Corpus2::Tagset& /* tagset */) const
{
	std::ostringstream ostream;
	ostream << raw_name() << "(" << *pos_expr_ << ", "
			<< ", \"" << chan_name_ << "\")";
	return ostream.str();
}

std::ostream& AnnToken::write_to(std::ostream& ostream) const
{
	ostream << raw_name() << "(" << *pos_expr_ << ", "
			<< ", \"" << chan_name_ << "\")";
	return ostream;
}

} /* end ns Wccl */
