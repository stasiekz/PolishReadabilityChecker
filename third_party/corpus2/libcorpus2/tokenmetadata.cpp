/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.CORPUS2, LICENSE.POLIQARP, COPYING.LESSER and COPYING files for more details.
*/

#include <libcorpus2/tokenmetadata.h>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

namespace Corpus2 {

TokenMetaData::TokenMetaData()
{
}

boost::shared_ptr<TokenMetaData> TokenMetaData::clone() const
{
	return boost::make_shared<TokenMetaData>(*this);
}

bool TokenMetaData::has_attribute(const std::string &name) const
{
	return attributes_.find(name) != attributes_.end();
}

std::string TokenMetaData::get_attribute(const std::string &name) const
{
	std::map<std::string, std::string>::const_iterator i;
	i = attributes_.find(name);
	if (i != attributes_.end()) {
		return i->second;
	} else {
		return "";
	}
}

void TokenMetaData::set_attribute(const std::string &name,
		const std::string &value)
{
	attributes_[name] = value;
}

} /* end ns Corpus2 */
