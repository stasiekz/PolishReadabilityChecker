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

#include <libwccl/wcclfile.h>

namespace Wccl {

FunctionalOpSequence::name_op_v_t WcclFile::gen_all_op_pairs()
{
	FunctionalOpSequence::name_op_v_t v;
	BOOST_FOREACH (const boost::shared_ptr<FunctionalOpSequence>& s, all_sections_) {
		s->add_name_op_pairs_untyped(v);
	}
	return v;
}

FunctionalOpSequence::name_op_v_c_t WcclFile::gen_all_op_pairs() const
{
	FunctionalOpSequence::name_op_v_c_t v;
	BOOST_FOREACH (const boost::shared_ptr<FunctionalOpSequence>& s, all_sections_) {
		s->add_name_op_pairs_untyped(v);
	}
	return v;
}

boost::shared_ptr<TagRuleSequence> WcclFile::get_tag_rules_ptr()
{
	if (!has_tag_rules()) {
		throw WcclError("There are no tag rules.");
	}
	return tag_rules_;
}

boost::shared_ptr<const TagRuleSequence> WcclFile::get_tag_rules_ptr() const
{
	if (!has_tag_rules()) {
		throw WcclError("There are no tag rules.");
	}
	return tag_rules_;
}

boost::shared_ptr<Matching::MatchRuleSequence> WcclFile::get_match_rules_ptr()
{
	if (!has_match_rules()) {
		throw WcclError("There are no match rules.");
	}
	return match_rules_;
}

boost::shared_ptr<const Matching::MatchRuleSequence> WcclFile::get_match_rules_ptr() const
{
	if (!has_match_rules()) {
		throw WcclError("There are no match rules.");
	}
	return match_rules_;
}

std::ostream& WcclFile::write_to(std::ostream& os) const
{
	if (has_lexicons()) {
		BOOST_FOREACH (const Lexicons::map_t::value_type& v, lexicons_->get_lexicons()) {
			os << "import(\"" << v.second->file_name() << ", \""
				<< v.second->name() << "\")\n";
		}
	}
	BOOST_FOREACH (const boost::shared_ptr<FunctionalOpSequence>& s, all_sections_) {
		os << s->to_string(*tagset_) << '\n';
	}
	if (has_tag_rules()) {
		os << tag_rules_->to_string(*tagset_) << '\n';
	}
	if (has_match_rules()) {
		os << match_rules_->to_string(*tagset_) << '\n';
	}
	return os;
}

std::string WcclFile::to_string() const
{
	std::ostringstream os;
	os << *this;
	return os.str();
}

} /* end ns Wccl */
