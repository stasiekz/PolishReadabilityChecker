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

#include <libwccl/ops/matchrulesequence.h>
#include <boost/foreach.hpp>

#include <boost/shared_ptr.hpp>

namespace Wccl {
namespace Matching {

void MatchRuleSequence::apply_all(
		const boost::shared_ptr<Corpus2::AnnotatedSentence>& sentence)
{
	if(!sentence || sentence->empty()) {
		throw InvalidArgument(
				"sentence",
				"Received an empty sentence.");
	}
	BOOST_FOREACH (MatchRule& rule, *this) {
		rule.apply(sentence);
	}
}

void MatchRuleSequence::apply_all_sentence_wrapper(
		const boost::shared_ptr<Corpus2::Sentence>& sentence)
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> as;
	as = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(sentence);
	if (!as) {
		throw InvalidArgument(
				"sentence",
				"Did not get an AnnotatedSentence from reader, 'ann' option broken?");
	}
	apply_all(as);
}

std::string MatchRuleSequence::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << "match_rules(\n";
	for (size_t i = 0; i < size(); ++i) {
		if (i != 0) {
			os << ";\n";
		}
		os << at(i).to_string(tagset);
	}
	os << ")";
	return os.str();
}

std::ostream& MatchRuleSequence::write_to(std::ostream &os) const
{
	os << "match_rules(\n";
	for (size_t i = 0; i < size(); ++i) {
		if (i != 0) {
			os << ";\n";
		}
		os << at(i);
	}
	os << ")";
	return os;
}

} /* end ns Matching */
} /* end ns Wccl */

