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

#include <libwccl/values/tset.h>
#include <boost/foreach.hpp>
#include <libpwrutils/bitset.h>

#include <boost/algorithm/string.hpp>
#include <sstream>

namespace Wccl {

const char* TSet::type_name = "TSet";

std::string TSet::to_raw_string() const
{
	return tag_.raw_dump();
}

std::string TSet::to_string(const Corpus2::Tagset& tagset) const
{
	return "{" + tagset.tag_to_symbol_string(tag_) + "}";
}

std::string TSet::var_repr(const std::string &var_name)
{
	std::ostringstream ss;
	ss << "$t:" << var_name;
	return ss.str();
}

int TSet::categories_count(const Corpus2::Tagset& tagset) const
{
	int cats = (tag_.get_pos().any()) ? 1 : 0;
	BOOST_FOREACH (const Corpus2::mask_t& mask, tagset.all_attribute_masks()) {
		if (tag_.get_values_for(mask).any()) {
			++cats;
		}
	}
	return cats;
}

int TSet::matching_categories(const Corpus2::Tag& tag) const
{
   const Corpus2::Tag& masked = tag_.get_masked(tag);
   return PwrNlp::count_bits_set(masked.get_pos()) + PwrNlp::count_bits_set(masked.get_values());
}

void TSet::insert_symbol(const Corpus2::Tagset& tagset, const std::string& s)
{
	tag_.combine_with(tagset.parse_symbol(s));
}

std::string TSet::to_compact_string(const Corpus2::Tagset& tagset)
		const
{
	if (tag_.is_null()) {
		return "-";
	}

	std::string body = tagset.tag_to_symbol_string(tag_);
	boost::algorithm::replace_all(body, ",", "-");
	return body;
}

} /* end ns Wccl */
