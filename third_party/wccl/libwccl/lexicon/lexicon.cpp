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

#include <libwccl/lexicon/lexicon.h>
#include <libpwrutils/util.h>
#include <libwccl/exception.h>
#include <boost/foreach.hpp>

#include <boost/make_shared.hpp>
#include <unicode/unistr.h>

namespace Wccl {

boost::shared_ptr<StrSet> Lexicon::translate(const UnicodeString &key) const
{
	boost::shared_ptr<StrSet> ret_set = boost::make_shared<StrSet>();
	map_t::const_iterator i = map_.find(key);
	if (i == map_.end()) {
		return ret_set;
	}

	BOOST_FOREACH (const UnicodeString& s, i->second){
		ret_set->insert(s);
	}

	return ret_set;
}

boost::shared_ptr<StrSet> Lexicon::translate(const StrSet& set) const
{
	boost::shared_ptr<StrSet> ret_set = boost::make_shared<StrSet>();
	BOOST_FOREACH (const UnicodeString& s, set.get_value()) {
		boost::shared_ptr<StrSet> v = translate(s);
		if (!v->empty()) {
			BOOST_FOREACH(const UnicodeString& s, v->contents()){
				ret_set->insert(s);
			}
		}
	}
	return ret_set;
}

void Lexicon::insert(const UnicodeString& key, const UnicodeString& value)
{
	if (key.isEmpty()) {
		throw InvalidArgument("key", "Empty key string in lexicon.");
	}
	if (value.isEmpty()) {
		throw InvalidArgument("value", "Empty value string in lexicon.");
	}
	map_[key].push_back(value);
}

} /* end ns Wccl */
