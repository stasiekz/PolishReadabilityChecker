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

#include <libwccl/ops/tagrulesequence.h>
#include <boost/foreach.hpp>


namespace Wccl {

Bool TagRuleSequence::execute_once(const boost::shared_ptr<Corpus2::Sentence>& sentence)
{
	if(!sentence || sentence->empty()) {
		throw InvalidArgument(
				"sentence",
				"Received an empty sentence.");
	}
	Bool changed(false);
	SentenceContext sc(sentence);
	while(sc.is_current_inside()) {
		BOOST_FOREACH (TagRule& rule, *this) {
			if (rule.execute(sc).get_value()) {
				changed.set_value(true);
			}
		}
		sc.advance();
	}
	return changed;
}

int TagRuleSequence::execute_until_done(const boost::shared_ptr<Corpus2::Sentence>& sentence, int max_iter)
{
	if(max_iter < 0) {
		throw InvalidArgument(
				"max_iter",
				"Supplied a negative value for maximum number of iterations.");
	}
	int iter_no = 0;
	while(iter_no < max_iter) {
		++iter_no;
		if (!execute_once(sentence).get_value()) {
			return iter_no;
		}
	}
	return iter_no;
}

std::string TagRuleSequence::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << "tag_rules(";
	for (size_t i = 0; i < size(); ++i) {
		if (i != 0) {
			os << ";\n";
		}
		os << at(i).to_string(tagset);
	}
	os << ")";
	return os.str();
}

std::ostream& TagRuleSequence::write_to(std::ostream &os) const
{
	os << "tag_rules(";
	for (size_t i = 0; i < size(); ++i) {
		if (i != 0) {
			os << ";\n";
		}
		os << at(i);
	}
	os << ")";
	return os;
}

} /* end ns Wccl */
