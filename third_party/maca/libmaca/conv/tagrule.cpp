/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

#include <libmaca/conv/tagrule.h>
#include <boost/foreach.hpp>
#include <iostream>
#include <boost/algorithm/string.hpp>

namespace Maca {
namespace Conversion {

TagRule::TagRule(const Corpus2::Tagset& tagset)
	: tagset_(&tagset)
{
}

void TagRule::add_precondition(const TagPredicate &tp)
{
	pre_.push_back(tp);
}

void TagRule::add_precondition(const std::string &pred_string)
{
	std::vector<std::string> srv;
	boost::algorithm::split(srv, pred_string, boost::is_any_of(": "));
	BOOST_FOREACH(const std::string& sr, srv) {
		if (!sr.empty()) {
			pre_.push_back(TagPredicate(sr, *tagset_));
		}
	}
}

void TagRule::add_postcondition(const TagPredicate &tp)
{
	post_.push_back(tp);
}

void TagRule::add_postcondition(const std::string &pred_string)
{
	std::vector<std::string> srv;
	boost::algorithm::split(srv, pred_string, boost::is_any_of(": "));
	BOOST_FOREACH(const std::string& sr, srv) {
		if (!sr.empty()) {
			post_.push_back(TagPredicate(sr, *tagset_));
		}
	}
}

void TagRule::apply(Corpus2::Tag &tag) const
{
	BOOST_FOREACH(const TagPredicate& tp, pre_) {
		if (!tp.check(tag)) return;
	}
	BOOST_FOREACH(const TagPredicate& tp, post_) {
		tp.apply(tag);
	}
}

Corpus2::Tag TagRule::apply_copy(const Corpus2::Tag &tag) const
{
	Corpus2::Tag tag2(tag);
	apply(tag2);
	return tag2;
}

} /* end ns Conversion */
} /* end ns Maca */
