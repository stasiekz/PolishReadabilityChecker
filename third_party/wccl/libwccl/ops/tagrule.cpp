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

#include <libwccl/ops/tagrule.h>
#include <sstream>

namespace Wccl {

Bool TagRule::execute(SentenceContext &sentence_context)
{
	if(sentence_context.size() == 0) {
		throw InvalidArgument(
				"sentence_context",
				"Received an empty sentence.");
	}
	if(!sentence_context.is_current_inside()) {
		throw InvalidArgument(
				"sentence_context",
				"Current position is outside boundaries of the sentence.");
	}
	Bool changed(false);
	ActionExecContext aec(sentence_context, variables_);
	if(condition_->apply(aec)->get_value()) {
		BOOST_FOREACH (const boost::shared_ptr<TagAction>& action, *actions_) {
			if(action->execute(aec).get_value()) {
				changed.set_value(true);
			}
		}
	}
	return changed;
}

const boost::shared_ptr<const Function<Bool> > TagRule::TrueCondition()
{
	static boost::shared_ptr<const Function<Bool> > true_constant(
			new Constant<Bool>(Bool(true)));
	return true_constant;
}


std::string TagRule::to_string(const Corpus2::Tagset &tagset) const
{
	std::ostringstream os;
	os << "rule(\"" << name_ << "\", " << condition_->to_string(tagset);
	BOOST_FOREACH (const boost::shared_ptr<TagAction>& action, *actions_) {
		os << ", " << action->to_string(tagset);
	}
	os << ")";
	return os.str();
}

std::ostream& TagRule::write_to(std::ostream& os) const {
	os << "rule(\"" << name_ << "\", " << *condition_;
	BOOST_FOREACH (const boost::shared_ptr<TagAction>& action, *actions_) {
		os << ", " << *action;
	}
	os << ")";
	return os;
}

} /* end ns Wccl */
