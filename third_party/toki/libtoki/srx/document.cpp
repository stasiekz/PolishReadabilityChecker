/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <boost/foreach.hpp>
#include <libtoki/util/xmlutil.h>
#include <libtoki/srx/document.h>

#include <libxml++/libxml++.h>
#include <libxml++/nodes/node.h>
#include <libxml++/nodes/element.h>

namespace Toki {
namespace Srx {

using XmlUtil::get_child_or_null;
using XmlUtil::get_child_text_or_empty;

namespace {
	const xmlpp::Node* get_child_or_throw(const xmlpp::Node* n,
			const char* name) {
		return XmlUtil::get_child_or_throw<Toki::Srx::ParseError>(n, name);
	}
} /* end anon ns */

Document::Document()
{
}

Document::~Document()
{
	BOOST_FOREACH (language_map_t::value_type v, language_map_) {
		delete v.first;
	}
}

void Document::load(std::istream &is)
{
	xmlpp::DomParser dom;
	dom.set_substitute_entities();
	dom.parse_stream(is);
	if (dom) {
		const xmlpp::Node* root = dom.get_document()->get_root_node();
		if (!root) throw ParseError("no root");
		if (root->get_name() != "srx") {
			throw ParseError("root node not <srx>");
		}
		const xmlpp::Node* header = get_child_or_null(root, "header");
		if (header) {
			process_header_node(header);
		}
		const xmlpp::Node* body = get_child_or_throw(root, "body");
		const xmlpp::Node* languagerules =
				get_child_or_throw(body, "languagerules");
		const xmlpp::Node::NodeList lrl =
				languagerules->get_children("languagerule");
		if (lrl.empty()) throw ParseError("no <languagerule>");
		BOOST_FOREACH (const xmlpp::Node* n, lrl) {
			process_languagerule_node(n);
		}
		const xmlpp::Node* maprules = get_child_or_throw(body, "maprules");
		const xmlpp::Node::NodeList mrl =
				maprules->get_children("languagemap");
		if (mrl.empty()) throw ParseError("no <languagemap>");
		BOOST_FOREACH (const xmlpp::Node* n, mrl) {
			process_languagemap_node(n);
		}

	} else {
		throw ParseError("Parse error");
	}
}

void Document::process_header_node(const xmlpp::Node *n)
{
	const xmlpp::Element* el = dynamic_cast<const xmlpp::Element*>(n);
	if (!el) throw ParseError("<header> not an Element");
	std::string casc = el->get_attribute_value("cascade");
	if (casc.empty() || casc == "yes") {
		cascade_ = true;
	} else if (casc == "no") {
		cascade_ = false;
	} else {
		throw ParseError("<header> cascade attribute invalid");
	}
	// formathandle elements ignored, we assume no markup
	// segmentsubflows attribute ignored, we assume no subflows
}

void Document::process_languagerule_node(const xmlpp::Node *n)
{
	const xmlpp::Element* el = dynamic_cast<const xmlpp::Element*>(n);
	if (!el) throw ParseError("<languagerule> not an Element");
	std::string name = el->get_attribute_value("languagerulename");
	if (name.empty()) {
		throw ParseError(
				"<languagerule> with empty languagerulename attribute");
	}
	const xmlpp::Node::NodeList lr = n->get_children("rule");
	BOOST_FOREACH (const xmlpp::Node* n, lr) {
		process_rule_node(name, n);
	}
}

void Document::process_rule_node(const std::string &language,
		const xmlpp::Node *n)
{
	const xmlpp::Element* el = dynamic_cast<const xmlpp::Element*>(n);
	if (!el) throw ParseError("<rule> not an Element");
	Rule rule;
	std::string break_attr = el->get_attribute_value("break");
	if (break_attr.empty() || break_attr == "yes") {
		rule.breaks = true;
	} else if (break_attr == "no") {
		rule.breaks = false;
	} else {
		throw ParseError("<rule> with invalid break attribute");
	}
	rule.before = get_child_text_or_empty(n, "beforebreak");
	rule.after = get_child_text_or_empty(n, "afterbreak");
	if (rule.before.empty() && rule.after.empty()) {
		throw ParseError(
				"<rule> with empty <beforebreak> and <afterbreak>");
	}
	language_rules_[language].push_back(rule);
}

void Document::process_languagemap_node(const xmlpp::Node *n)
{
	const xmlpp::Element* el = dynamic_cast<const xmlpp::Element*>(n);
	if (!el) throw ParseError("<languagemap> not an Element");
	std::string pattern = el->get_attribute_value("languagepattern");
	std::string name = el->get_attribute_value("languagerulename");
	UnicodeString upattern = UnicodeString::fromUTF8(pattern);
	UErrorCode status = U_ZERO_ERROR;
	RegexMatcher* matcher = new RegexMatcher(upattern, 0, status);
	if (!U_SUCCESS(status)) {
		throw ParseError("<languagemap> pattern failed to compile");
	}
	language_map_.push_back(std::make_pair(matcher, name));
}

std::vector<Rule> Document::get_all_rules() const
{
	std::vector<Rule> rules;
	language_rules_t::const_iterator li = language_rules_.begin();
	for (; li != language_rules_.end(); ++li) {
		std::vector<Rule>::const_iterator i = li->second.begin();
		for (; i != li->second.end(); ++i) {
			rules.push_back(*i);
		}
	}
	return rules;
}

std::vector<Rule> Document::get_rules_for_lang(
		const std::string &lang) const
{
	UnicodeString ulang = UnicodeString::fromUTF8(lang);
	std::vector<Rule> rules;
	BOOST_FOREACH (language_map_t::value_type v, language_map_) {
		RegexMatcher& m = *v.first;
		m.reset(ulang);
		UErrorCode status = U_ZERO_ERROR;
		if (m.matches(status)) {
			language_rules_t::const_iterator ruleset =
					language_rules_.find(v.second);
			if (ruleset != language_rules_.end()) {
				std::vector<Rule>::const_iterator i, e;
				i = ruleset->second.begin();
				e = ruleset->second.end();
				for (; i != e; ++i) {
					rules.push_back(*i);
				}
			}
			if (!cascade_) break;
		}
	}
	return rules;
}

std::string Document::info() const
{
	std::stringstream ss;
	ss << "SRX " << language_rules_.size() << " languages with ";
	language_rules_t::const_iterator li = language_rules_.begin();
	for (; li != language_rules_.end(); ++li) {
		ss << li->second.size() << " ";
	}
	ss << "rules";
	return ss.str();
}

} /* end ns Srx */ } /* end ns Toki */
