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

#include <libcorpus2/tagsetparser.h>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <boost/foreach.hpp>

namespace Corpus2 {

std::string TagsetParseError::info() const
{
	std::stringstream ss;
	ss << "Line " << line << ": " << what() << " `" << data << "`";
	return ss.str();
}

Tagset TagsetParser::load_ini(const std::string &filename)
{
	std::ifstream ifs(filename.c_str());
	return load_ini(ifs);
}

void TagsetParser::save_ini(const Tagset &tagset,
		const std::string &filename)
{
	std::ofstream ofs(filename.c_str());
	save_ini(tagset, ofs);
}

Tagset TagsetParser::load_ini(std::istream &is)
{
	int line_no = 0;
	std::string line;
	std::string sep(" \t=");
	std::set<std::string> values;
	values.insert("/null");
	std::set<std::string> symbols(values);
	typedef std::map< std::string, std::deque<std::string> > vmap_t;
	vmap_t vmap;
	typedef std::map< std::string, std::vector<idx_t> > pmap_t;
	pmap_t pmap;
	typedef std::map< std::string, std::vector<bool> > reqmap_t;
	reqmap_t reqmap;
	Tagset tagset;

	while (std::getline(is, line)) {
		boost::algorithm::trim(line);
		++line_no;
		if (line == "[ATTR]") break;
	}

	//attribute-value defs
	while (std::getline(is, line)) {
		boost::algorithm::trim(line);
		++line_no;
		if (line == "[POS]") break;
		if (!line.empty() && line[0] != '#') {
			std::deque<std::string> v;
			boost::algorithm::split(v, line, boost::is_any_of(sep),
					boost::algorithm::token_compress_on);
			if (v.size() < 3) {
				throw TagsetParseError("Attribute with less than 2 values",
						line_no, line);
			}
			if (!symbols.insert(v[0]).second) {
				throw TagsetParseError("Duplicate symbol", line_no, v[0]);
			}
			std::deque<std::string>& avalues = vmap[v[0]];
			v.pop_front();
			avalues = v;
			BOOST_FOREACH(const std::string& s, v) {
				if (!symbols.insert(s).second && false) {// Allow duplicate values in different attributes
					throw TagsetParseError("Duplicate symbol", line_no, s);
				}
				values.insert(s);
			}
		}
	}

	if (*values.begin() != "/null") {
		throw TagsetParseError("First value not '@null'", line_no,
			*values.begin());
	}

	mask_t current_value = 1;
	std::vector<std::string> vec;
	idx_t current_attribute_index = 0;
	BOOST_FOREACH(const vmap_t::value_type v, vmap) {
		mask_t attribute_mask = 0;
		vec.push_back(v.first);
		tagset.attribute_values_.resize(
				tagset.attribute_values_.size() + 1);
		BOOST_FOREACH(const std::string& s, v.second) {
			tagset.attribute_values_.back().push_back(current_value);
			tagset.value_mask_to_attribute_index_.insert(
					std::make_pair(current_value, current_attribute_index));
			tagset.string_to_value_mask_.insert(
					std::make_pair(s, current_value));
			tagset.value_mask_to_string_.insert(
					std::make_pair(current_value, s));
			attribute_mask |= current_value;
			current_value <<= 1;
		}
		tagset.attribute_masks_.push_back(attribute_mask);
		tagset.attribute_mask_to_index_.insert(std::make_pair(
			attribute_mask, current_attribute_index));
		++current_attribute_index;
	}
	tagset.attribute_dict_.load_sorted_data(vec);

	std::vector<std::string> poses_plain;
	while (std::getline(is, line)) {
		boost::algorithm::trim(line);
		++line_no;
		if (!line.empty() && line[0] != '#') {
			if (line[0] == '[') break;
			std::deque<std::string> v;
			boost::algorithm::split(v, line, boost::is_any_of(sep),
					boost::algorithm::token_compress_on);
			if (!symbols.insert(v[0]).second) {
				throw TagsetParseError("Duplicate symbol", line_no, v[0]);
			}
			poses_plain.push_back(v[0]);
			std::vector<idx_t>& pattrs = pmap[v[0]];
			std::vector<bool>& req_mask = reqmap[v[0]];
			req_mask.resize(tagset.attribute_dict_.size());
			v.pop_front();
			BOOST_FOREACH(std::string s, v) {
				if (s.empty()) continue;
				bool required = true;
				if (s[0] == '[' && s[s.size() - 1] == ']') {
					required = false;
					s = s.substr(1, s.size() - 2);
				}
				idx_t a = tagset.attribute_dict_.get_id(s);
				if (!tagset.attribute_dict_.is_id_valid(a)) {
					throw TagsetParseError("Attribute name invalid",
							line_no, s);
				}
				pattrs.push_back(a);
				req_mask[a] = required;
			}
		}
	}

	std::string ign_tag_string = "ign";
	if (line != "[IGN]") {
		while (std::getline(is, line)) {
			if (line == "[IGN]") break;
		}
	}
	if (line == "[IGN]") {
		if (std::getline(is, line)) {
			ign_tag_string = line;
		}
	}

	vec.clear();
	BOOST_FOREACH(const pmap_t::value_type v, pmap) {
		vec.push_back(v.first);
		mask_t valid(0);
		mask_t required(0);
		tagset.pos_attributes_.push_back(v.second);
		tagset.pos_required_attributes_idx_.resize(
				tagset.pos_required_attributes_idx_.size() + 1);
		tagset.pos_valid_attributes_.push_back(
				std::vector<bool>(tagset.attribute_values_.size(), false));
		BOOST_FOREACH(idx_t a, v.second) {
			valid |= tagset.get_attribute_mask(a);
			if (reqmap[v.first][a]) {
				required |= tagset.get_attribute_mask(a);
				tagset.pos_required_attributes_idx_.back().push_back(a);
			}
			tagset.pos_valid_attributes_.back()[a] = true;
		}
		tagset.pos_required_attributes_.push_back(reqmap[v.first]);
		tagset.pos_valid_value_masks_.push_back(valid);
		tagset.pos_required_value_masks_.push_back(required);
	}
	tagset.pos_dict_.load_sorted_data(vec);
	if (tagset.pos_dict_.size() == 0) {
		throw TagsetParseError("No POS in tagset", line_no, "");
	}
	for (size_t i = 0; i < poses_plain.size(); ++i) {
		idx_t p = tagset.pos_dictionary().get_id(poses_plain[i]);
		tagset.original_pos_indices_.insert(std::make_pair(p,i));
		tagset.valid_pos_mask_ |= (mask_t(1) << i);
	}
	tagset.ign_tag_ = tagset.parse_simple_tag(ign_tag_string);
	return tagset;
}

void TagsetParser::save_ini(const Tagset &tagset, std::ostream &os)
{
	os << "# Autogenerated by Corpus2\n\n";
	os << "[ATTR]\n";
	idx_t a(0);
	while (tagset.attribute_dict_.is_id_valid(a)) {
		os << tagset.attribute_dict_.get_string(a) << "\t= ";
		BOOST_FOREACH(mask_t m, tagset.get_attribute_values(a)) {
			os << tagset.get_value_name(m) << " ";
		}
		os << "\n";
		++a;
	}
	os << "\n[POS]\n";
	idx_t p(0);
	while (tagset.pos_dict_.is_id_valid(p)) {
		os << tagset.pos_dict_.get_string(p) << "\t= ";
		BOOST_FOREACH(idx_t a, tagset.get_pos_attributes(p)) {
			if (tagset.pos_required_attributes_[p][a]) {
				os << tagset.attribute_dict_.get_string(a) << " ";
			} else {
				os << '[' << tagset.attribute_dict_.get_string(a) << "] ";
			}
		}
		os << "\n";
		++p;
	}
	os << "# End autogenerated file\n";
}

} /* end ns Corpus2 */
