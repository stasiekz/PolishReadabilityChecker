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

#include <libmaca/conv/joinrule.h>
#include <boost/foreach.hpp>
#include <libcorpus2/tagsetmanager.h>
#include <libmaca/conv/attributecopier.h>
#include <boost/algorithm/string.hpp>

namespace Maca {
namespace Conversion {

JoinRule::JoinRule(const Corpus2::Tagset& tagset)
	: tagset_(&tagset)
	, copy_t2_attrs_(0)
{
}

JoinRule::JoinRule(const Config::Node& cfg)
	: tagset_(&Corpus2::get_named_tagset(cfg.get<std::string>("tagset")))
	, copy_t2_attrs_(0)
{
	std::string pos1, pos2;
	UnicodeString orth1, orth2;
	BOOST_FOREACH(const Config::Node::value_type &v, cfg) {
		if (v.first == "t1_pos") {
			pos1 = v.second.data();
		} else if (v.first == "t2_pos") {
			pos2 = v.second.data();
		} else if (v.first == "t1_orth") {
			orth1 = UnicodeString::fromUTF8(v.second.data());
		} else if (v.first == "t2_orth") {
			orth2 = UnicodeString::fromUTF8(v.second.data());
		} else if (v.first == "post") {
			add_postcondition(v.second.data());
		} else if (v.first == "copy_attr") {
			append_copy_attrs(v.second.data());
		}
	}
	set_token1_preconditions(pos1, orth1);
	set_token2_preconditions(pos2, orth2);
}

void JoinRule::set_token1_preconditions(const PosOrthPredicate &pre)
{
	pre1_ = pre;
}

void JoinRule::set_token1_preconditions(const std::string& pos,
		const UnicodeString& orth)
{
	Corpus2::mask_t p = tagset_->get_pos_mask(pos);
	pre1_ = PosOrthPredicate(p, orth);
}

void JoinRule::set_token2_preconditions(const PosOrthPredicate &pre)
{
	pre1_ = pre;
}

void JoinRule::set_token2_preconditions(const std::string& pos,
		const UnicodeString& orth)
{
	Corpus2::mask_t p = tagset_->get_pos_mask(pos);
	pre2_ = PosOrthPredicate(p, orth);
}

void JoinRule::set_copy_attrs(Corpus2::mask_t mask)
{
	copy_t2_attrs_ = mask;
}

void JoinRule::append_copy_attrs(const std::string& names)
{
	append_attributes_mask(copy_t2_attrs_, tagset(), names);
}

void JoinRule::add_postcondition(const TagPredicate &tp)
{
	post_.push_back(tp);
}

void JoinRule::add_postcondition(const std::string& pred_string)
{
	std::vector<std::string> srv;
	boost::algorithm::split(srv, pred_string,
			boost::is_any_of(std::string(": ")));
	BOOST_FOREACH(const std::string& sr, srv) {
		if (!sr.empty()) {
			post_.push_back(TagPredicate(sr, *tagset_));
		}
	}
}

Corpus2::Token* JoinRule::try_join(Corpus2::Token* t1, Corpus2::Token* t2) const
{
	if (pre1_.check(*t1) && pre2_.check(*t2)) {
		t1->set_orth(t1->orth() + t2->orth());
		copy_attributes(*t2, copy_t2_attrs_, *t1);
		apply_predicates(post_, *t1);
		delete t2;
		return t1;
	} else {
		//std::cerr << pre1_.check(*t1) << " " << pre2_.check(*t2) << "\n";
		//std::cerr << pre2_.dump() << "\n";
		return NULL;
	}
}

} /* end ns Conversion */
} /* end ns Maca */
