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

#include <libwccl/ops/tagactions/relabel.h>
#include <boost/foreach.hpp>
#include <sstream>

namespace Wccl {

Bool Relabel::execute(const ActionExecContext& context) const
{
	Bool changed(false);
	Corpus2::mask_t replace_wclass = replace_with_->apply(context)->get_value().get_pos();
	int abs_pos = context.sentence_context().get_abs_position(*pos_->apply(context));
	if (context.sentence_context().is_inside(abs_pos)) {
		Corpus2::Token& token = *context.sentence_context().at(abs_pos);
		std::vector<Corpus2::Lexeme> original(token.lexemes());
		token.lexemes().clear();
		BOOST_FOREACH (Corpus2::Lexeme& lexeme, original) {
			token.add_lexeme(lexeme);
			if ((lexeme.tag().get_pos() != replace_wclass) && condition_->apply(context)->get_value()) {
				changed.set_value(true);
				lexeme.set_tag(Corpus2::Tag(replace_wclass, lexeme.tag().get_values()));
			}
			token.lexemes().pop_back();
		}
		token.lexemes() = original;
	}
	return changed;
}

std::string Relabel::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << name() << "(" << pos_->to_string(tagset) << ", "
			<< replace_with_->to_string(tagset) << ", "
			<< condition_->to_string(tagset) << ")";
	return os.str();
}

std::ostream& Relabel::write_to(std::ostream& os) const
{
	os << name() << "(" << *pos_ << ", " << *replace_with_ << ", " << *condition_ << ")";
	return os;
}


} /* end ns Wccl */
