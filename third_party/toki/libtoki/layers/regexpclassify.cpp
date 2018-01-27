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
#include <libtoki/layers/regexpclassify.h>
#include <libtoki/token.h>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

namespace Toki {

RegexpClassifyLayer::RegexpClassifyLayer(TokenSource* input,
		const Config::Node& props)
	: TokenLayer(input, props)
{
	BOOST_FOREACH (const Config::Node::value_type &v, props) {
		if (boost::algorithm::starts_with(v.first, "type:")) {
			std::string type = v.first.substr(5); //length of "type:"
			UErrorCode status = U_ZERO_ERROR;
			RegexMatcher *m = new RegexMatcher(
					UnicodeString::fromUTF8(v.second.data()), 0, status);
			if (U_SUCCESS(status)) {
				classifiers_.push_back(std::make_pair(type, m));
			} else {
				if (error_stream_) {
					(*error_stream_) << "Error in regexp for type:" << type
						<< " -- " << v.second.data() << "\n";
				}
				delete m;
			}
		}
	}
}

RegexpClassifyLayer::~RegexpClassifyLayer()
{
	typedef std::pair<std::string, RegexMatcher*> vt;
	BOOST_FOREACH (const vt &v, classifiers_) {
		delete v.second;
	}
}

std::string RegexpClassifyLayer::info() const
{
	std::stringstream ss;
	ss << "regexp{" << classifiers_.size() << "}";
	return ss.str();
}

std::string RegexpClassifyLayer::long_info() const
{
	std::stringstream ss;
	ss << ", regexp: " << classifiers_.size() << " classifiers";
	return TokenLayer::long_info() + ss.str();
}

Token* RegexpClassifyLayer::process_token(Token* t)
{
	for (size_t i = 0; i < classifiers_.size(); ++i) {
		RegexMatcher& m = *classifiers_[i].second;
		m.reset(t->orth());
		UErrorCode status = U_ZERO_ERROR;
		if (m.matches(status)) {
			t->set_type(classifiers_[i].first);
			break;
		}
	}
	return t;
}

} /* end namespace Toki */
