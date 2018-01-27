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

#ifndef LIBTOKI_CONFIGNODE_H
#define LIBTOKI_CONFIGNODE_H

#include <boost/property_tree/ptree.hpp>

#include <unicode/unistr.h>

#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace Toki { namespace Config {

/**
 * Typedef for the configuration type passed around
 */
typedef boost::property_tree::ptree Node;

/**
 * Load configuration from a file (may throw)
 */
Node from_file(const std::string &filename);

/**
 * Load configuration from a file (may throw)
 */
Node from_stream(std::istream& is);

/**
 * Merge two config nodes and return a node with the merged contents
 */
Node merge_copy(const Node& accu, const Node& other);

/**
 * Merge a config node into another.
 */
Node& merge_into(Node& accu, const Node& other);

/**
 * Write a config node into a file
 */
void write(const Node& c, const std::string& filename);

/**
 * @brief Custom boolean values translator
 *
 * This translator convertes strings stored in boost::property_tree
 * (default data struct for configuration) into boolean values when
 * needed. It extends accepted strings to be one of @c true/false,
 * @c yes/no, @c on/off, and @c 1/0 (like in default translator).
 */
class BoolTranslator {
public:
	typedef std::string	internal_type;
	typedef bool		external_type;

	boost::optional<external_type> get_value(internal_type const &v);
	boost::optional<internal_type> put_value(external_type const &v);
};

} /* end ns Config */ } /* end ns Toki */

// This will make boost use our translator
// when retrieving boolean value all the time
namespace boost { namespace property_tree {
	template<>
	struct translator_between<std::string, bool>
	{
		typedef Toki::Config::BoolTranslator type;
	};
}}

#endif // LIBTOKI_CONFIGNODE_H
