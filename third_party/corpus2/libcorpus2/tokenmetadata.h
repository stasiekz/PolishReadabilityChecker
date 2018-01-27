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

#ifndef LIBCORPUS2_TOKENMETADATA_H
#define LIBCORPUS2_TOKENMETADATA_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

namespace Corpus2 {


/// Token metadata class
class TokenMetaData
{
public:
	TokenMetaData();

	boost::shared_ptr<TokenMetaData> clone() const;

	typedef std::map<std::string, std::string> attr_map_t;

	bool has_attribute(const std::string& name) const;

	std::string get_attribute(const std::string& name) const;

	void set_attribute(const std::string& name, const std::string& value);

	const attr_map_t& attributes() const {
		return attributes_;
	}

private:
	attr_map_t attributes_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_TOKENMETADATA_H
