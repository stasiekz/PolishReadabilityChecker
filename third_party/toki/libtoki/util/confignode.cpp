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

#ifdef HAVE_CONFIG_D_H
#include <libtoki/config_d.h>
#endif

#include <libtoki/util/confignode.h>
#include <libtoki/exception.h>
#include <libtoki/parser/loose_ini_paser.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

#include <iostream>

namespace Toki {
namespace Config {

Node from_file(const std::string &filename)
{
	Node p;
	try {
		loose_ini_parser::read_loose_ini(filename, p);
	} catch (boost::property_tree::file_parser_error& e) {
		throw TokiError(e.what());
	}

	return p;
}

Node from_stream(std::istream &is)
{
	Node p;
	try {
		loose_ini_parser::read_loose_ini(is, p);
	} catch (boost::property_tree::file_parser_error& e) {
		throw TokiError(e.what());
	}
	return p;
}

Node& merge_into(Node& accu, const Node& other)
{
	accu.data() = other.data();
	using boost::property_tree::ptree;
	BOOST_FOREACH (const ptree::value_type& in_other, other) {
		boost::optional< ptree& > in_one;
		if ((in_one = accu.get_child_optional(in_other.first))) {
			if (in_other.second.get("_merge", "") == "override") {
				*in_one = in_other.second;
			} else {
				merge_into(*in_one, in_other.second);
			}
		} else {
			accu.push_back(in_other);
		}
	}
	return accu;
}

Node merge_copy(const Node &one, const Node &other)
{
	Node accu(one);
	merge_into(accu, other);
	return accu;
}

void write(const Node &c, const std::string &filename)
{
	int errors = 0;
	try {
		boost::property_tree::ini_parser::write_ini(filename + ".ini", c);
	} catch (boost::property_tree::file_parser_error& e) {
		errors++;
	}
	try {
		boost::property_tree::xml_parser::write_xml(filename + ".xml", c);
	} catch (boost::property_tree::file_parser_error& e) {
		errors++;
	}
	try {
		boost::property_tree::json_parser::write_json(filename + ".json",
				c);
	} catch (boost::property_tree::file_parser_error& e) {
		errors++;
	}
	if (errors > 0) {
		std::cerr << "Config write failed!\n";
	}
}

boost::optional<BoolTranslator::external_type>
BoolTranslator::get_value(BoolTranslator::internal_type const &v)
{
	std::string v_lower = boost::algorithm::to_lower_copy(v);
	if( v_lower.compare("true") == 0 ||
		v_lower.compare("yes") == 0 ||
		v_lower.compare("on") == 0 ||
		v_lower.compare("1") == 0 )
			return true;
	else if(v_lower.compare("false") == 0 ||
			v_lower.compare("off") == 0 ||
			v_lower.compare("no") == 0 ||
			v_lower.compare("0") == 0 )
			return false;
	else
			return boost::none;
}

boost::optional<BoolTranslator::internal_type>
BoolTranslator::put_value(BoolTranslator::external_type const &v)
{
	if(v)
		return boost::optional<BoolTranslator::internal_type>("true");
	else
		return boost::optional<BoolTranslator::internal_type>("false");
}

} /* end ns Config */
} /* end namespace Toki */
