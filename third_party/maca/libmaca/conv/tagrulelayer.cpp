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

#include <libmaca/conv/tagrulelayer.h>
#include <boost/foreach.hpp>
#include <libcorpus2/tagsetmanager.h>

namespace Maca {
namespace Conversion {

TagRuleLayer::TagRuleLayer(const Corpus2::Tagset& tagset)
	: OneTagsetLayer(tagset), rules_()
{
}

TagRuleLayer::TagRuleLayer(const Config::Node& cfg)
	: OneTagsetLayer(Corpus2::get_named_tagset(cfg.get<std::string>("tagset")))
	, rules_()
{
	append_rule(cfg);
}

TagRuleLayer* TagRuleLayer::clone() const
{
	return new TagRuleLayer(*this);
}

void TagRuleLayer::append_rule(const TagRule &tr)
{
	rules_.push_back(tr);
}

void TagRuleLayer::append_rule(const Config::Node& cfg)
{
	TagRule tr(tagset_from());
	BOOST_FOREACH(const Config::Node::value_type &v, cfg) {
		if (v.first == "pre") {
			tr.add_precondition(v.second.data());
		} else if (v.first == "post") {
			tr.add_postcondition(v.second.data());
		}
	}
	append_rule(tr);
}

Corpus2::Token* TagRuleLayer::get_next_token()
{
	Corpus2::Token* t = source()->get_next_token();
	if (t != NULL) {
		process(t);
	}
	return t;
}

void TagRuleLayer::process(Corpus2::Token* t)
{
	BOOST_FOREACH(Corpus2::Lexeme& lex, t->lexemes()) {
		Corpus2::Tag newtag = lex.tag();
		BOOST_FOREACH(const TagRule& tr, rules_) {
			tr.apply(newtag);
		}
		lex.set_tag(newtag);
	}
}


RegexTagRuleLayer::RegexTagRuleLayer(const Corpus2::Tagset& tagset)
	: TagRuleLayer(tagset), matcher_(), pattern_()
{
}

RegexTagRuleLayer::RegexTagRuleLayer(const Config::Node& cfg)
	: TagRuleLayer(cfg), matcher_(), pattern_()
{
	append_rule(cfg);
	std::string regexp_string = cfg.get("regexp", "");
	UnicodeString u_regexp_string = UnicodeString::fromUTF8(regexp_string);
	UErrorCode status = U_ZERO_ERROR;
	pattern_.reset(RegexPattern::compile(u_regexp_string, 0, status));
	if (!U_SUCCESS(status)) {
		throw MacaError("Regexp failed to compile: " + regexp_string);
	}
	matcher_.reset(pattern_->matcher(status));
	if (!U_SUCCESS(status)) {
		throw MacaError("Regexp failed to compile: " + regexp_string);
	}
}

RegexTagRuleLayer* RegexTagRuleLayer::clone() const
{
	RegexTagRuleLayer* copy = new RegexTagRuleLayer(tagset());
	if (matcher_) {
		copy->pattern_ = pattern_;
		UErrorCode status = U_ZERO_ERROR;
		copy->matcher_.reset(copy->pattern_->matcher(status));
	}
	return copy;
}

RegexTagRuleLayer::~RegexTagRuleLayer()
{
}

Corpus2::Token* RegexTagRuleLayer::get_next_token()
{
	Corpus2::Token* t = source()->get_next_token();
	if (t != NULL) {
		if (matcher_) {
			UErrorCode e = U_ZERO_ERROR;
			matcher_->reset(t->orth());
			if (matcher_->matches(e)) {
				process(t);
			}
		} else {
			process(t);
		}
	}
	return t;
}

} /* end ns Conversion */
} /* end ns Maca */
