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

#include <libwccl/values/strset.h>
#include <boost/foreach.hpp>
#include <libpwrutils/util.h>
#include <sstream>
#include <boost/algorithm/string.hpp>

namespace Wccl {

const char* StrSet::type_name = "StrSet";

std::string StrSet::to_raw_string() const
{
	std::stringstream ss;
	ss << "[";
	value_type::const_iterator it = set_.begin();
	while(it != set_.end()) {
		ss << '\"';
		std::string item = PwrNlp::to_utf8(*it);
		boost::algorithm::replace_all(item, "\\", "\\\\");
		boost::algorithm::replace_all(item, "\"", "\\\"");
		ss << item;
		ss << '\"';
		if(++it != set_.end()) {
			ss << ", ";
		}
	}
	ss << "]";
	return ss.str();
}

UnicodeString StrSet::to_raw_string_u() const
{
	UnicodeString u;
	u.append(UNICODE_STRING("[", 1));
	value_type::const_iterator it = set_.begin();
	while(it != set_.end()) {
		u.append(UNICODE_STRING("\"", 1));
		UnicodeString item = *it;
		item.findAndReplace(UNICODE_STRING("\\", 1), UNICODE_STRING("\\\\", 2));
		item.findAndReplace(UNICODE_STRING("\"", 1), UNICODE_STRING("\\\"", 2));
		u.append(item);
		u.append(UNICODE_STRING("\"", 1));
		if(++it != set_.end()) {
			u.append(UNICODE_STRING(", ", 2));
		}
	}
	u.append(UNICODE_STRING("]", 1));
	return u;
}

std::string StrSet::to_compact_string(const Corpus2::Tagset& /* tagset */)
		const
{
	if (set_.empty()) {
		return "-";
	}

	std::stringstream ss;
	value_type::const_iterator it = set_.begin();
	while(it != set_.end()) {
		ss << '\"';
		std::string item = PwrNlp::to_utf8(*it);
		boost::algorithm::replace_all(item, "-", "\\u002d");
		boost::algorithm::replace_all(item, ".", "\\u002e");
		boost::algorithm::replace_all(item, " ", "\\u0020");
		boost::algorithm::replace_all(item, "\t", "\\u0009");
		ss << item;
		ss << '\"';
		if(++it != set_.end()) {
			ss << "-";
		}
	}
	return ss.str();
}

UnicodeString StrSet::to_compact_string_u(const Corpus2::Tagset& /* tagset */)
		const
{
	UnicodeString u;

	if (set_.empty()) {
		u.append(UNICODE_STRING("-", 1));
		return u;
	}

	value_type::const_iterator it = set_.begin();
	while(it != set_.end()) {
		u.append(UNICODE_STRING("\"", 1));
		UnicodeString item = *it;
		item.findAndReplace(UNICODE_STRING("-", 1), UNICODE_STRING("\\u002d", 6));
		item.findAndReplace(UNICODE_STRING(".", 1), UNICODE_STRING("\\u002e", 6));
		item.findAndReplace(UNICODE_STRING(" ", 1), UNICODE_STRING("\\u0020", 6));
		item.findAndReplace(UNICODE_STRING("\t", 1), UNICODE_STRING("\\u0009", 6));
		u.append(item);
		u.append(UNICODE_STRING("\"", 1));
		if(++it != set_.end()) {
			u.append(UNICODE_STRING("-", 1));
		}
	}
	return u;
}

bool StrSet::intersects(const StrSet &other) const {
	if (empty() || other.empty()) {
		return false;
	}
	//We just want to check if there is an intersection, no
	//need to actually compute it to check if it's empty.
	//Doing it like below sounds faster than, say, sorting
	//the sets and using set_intersection.
	//It's faster to iterate through the smaller set and check in
	//the larger than it is to do the opposite, hence the &?: below.
	const value_type& smaller = size() < other.size() ? set_ : other.set_;
	const value_type& bigger = size() < other.size() ? other.set_ : set_;
	BOOST_FOREACH (const UnicodeString& u, smaller) {
		if (bigger.find(u) != bigger.end()) {
			return true;
		}
	}
	return false;
}

bool StrSet::is_subset_of(const StrSet &other) const
{
	if (size() > other.size()) {
		return false;
	}
	BOOST_FOREACH (const UnicodeString& u, set_) {
		if (other.set_.find(u) == other.set_.end()) {
			return false;
		}
	}
	return true;
}

std::string StrSet::var_repr(const std::string &var_name)
{
	std::stringstream ss;
	ss << "$s:" << var_name;
	return ss.str();
}

} /* end ns Wccl */
