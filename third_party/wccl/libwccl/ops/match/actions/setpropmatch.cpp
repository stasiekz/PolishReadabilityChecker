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
#include <libwccl/ops/match/actions/setpropmatch.h>
#include <libcorpus2/ann/annotatedsentence.h>

#include <sstream>

namespace Wccl {
namespace Matching {

void PropMatch::execute(const ActionExecContext& context) const
{
	SentenceContext& sc = context.sentence_context();
	boost::shared_ptr<Corpus2::AnnotatedSentence> as;
	as = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(sc.get_sentence_ptr());
	if (!as) {
		throw InvalidArgument("context", "Operator needs an annotated sentence.");
	}

	int abs_pos = match_->apply(context)->first_token(as);
	if(sc.is_outside(abs_pos)) {
		throw WcclError("Received starting match that points outside sentence.");
	}

	Corpus2::Token* token = ((Corpus2::Sentence&) *as)[abs_pos];
	boost::shared_ptr<Corpus2::TokenMetaData> metadata = token->get_metadata();
	if(metadata == boost::shared_ptr<Corpus2::TokenMetaData>()){
		token->create_metadata();
		metadata = token->get_metadata();
	}
	metadata->set_attribute(key_name_, value_name_);

}

std::string PropMatch::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << name() << "(" << match_->to_string(tagset) << ", \"" << key_name_ << ", \"" << value_name_ << "\")";
	return os.str();
}

std::ostream& PropMatch::write_to(std::ostream& os) const
{
	return os << name() << "(" << *match_ << ", \"" << key_name_ << ", \"" << value_name_ << "\")";
}

} /* end ns Matching */
} /* end ns Wccl */
