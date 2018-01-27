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

#include <libmaca/conv/joinlayer.h>
#include <libcorpus2/tagsetmanager.h>
#include <boost/foreach.hpp>

namespace Maca {
namespace Conversion {

JoinLayer::JoinLayer(const Corpus2::Tagset& tagset)
	: OneTagsetLayer(tagset), buf_(NULL)
{
}

JoinLayer::JoinLayer(const Config::Node& cfg)
	: OneTagsetLayer(Corpus2::get_named_tagset(cfg.get<std::string>("tagset")))
	, buf_(NULL)
{
	append_rule(cfg);
}

JoinLayer::~JoinLayer()
{
	delete buf_;
}

JoinLayer* JoinLayer::clone() const
{
	JoinLayer* copy = new JoinLayer(tagset());
	copy->rules_ = rules_;
	if (buf_ != NULL) {
		copy->buf_ = buf_->clone();
	}
	return copy;
}

void JoinLayer::append_rule(const JoinRule &rule)
{
	Corpus2::require_matching_tagsets(rule, *this, "appending join rule");
	rules_.push_back(rule);
}

void JoinLayer::append_rule(const Config::Node& cfg)
{
	JoinRule jr(cfg);
	append_rule(jr);
}

Corpus2::Token* JoinLayer::get_next_token()
{
	if (buf_ == NULL) {
		buf_ = source()->get_next_token();
	}
	Corpus2::Token* t = source()->get_next_token();
	if (t != NULL && t->wa() == PwrNlp::Whitespace::None) {
		BOOST_FOREACH(JoinRule& rule, rules_) {
			Corpus2::Token* joined = rule.try_join(buf_, t);
			if (joined != NULL) {
				buf_ = joined;
				t = source()->get_next_token();
				if (t == NULL || t->wa() != PwrNlp::Whitespace::None) {
					break;
				}
			}
		}
	}
	std::swap(t, buf_);
	return t;
}

} /* end ns Conversion */
} /* end ns Maca */
