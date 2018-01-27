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

#include <libcorpus2/exception.h>
#include <libcorpus2/tagset.h>
#include <libcorpus2/lexeme.h>
#include <libcorpus2/token.h>
#include <libcorpus2/util/settings.h>
#include <libcorpus2/tagsetparser.h>

#include <boost/foreach.hpp>
#include <libpwrutils/util.h>

#include <boost/algorithm/string.hpp>
#include <boost/serialization/strong_typedef.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/pending/lowest_bit.hpp>

#include <sstream>
#include <iostream>
#include <fstream>

namespace Corpus2 {

TagParseError::TagParseError(const std::string &what,
		const std::string& val, const std::string& tag,
		const std::string& tagset)
	: Corpus2Error(what), val(val), tag(tag), tagset(tagset)
{
}

std::string TagParseError::info() const
{
	std::ostringstream ss;
	ss << what();
	if (!val.empty()) {
		ss << ": " << val;
	}
	if (!tag.empty()) {
		ss << " [" << tag << "]";
	}
	if (!tagset.empty()) {
		ss << " (" << tagset << ")";
	}
	return ss.str();
}

TagsetMismatch::TagsetMismatch(const std::string& where,
		const Tagset& expected, const Tagset& actual)
	: Corpus2Error("Tagset mismatch in " + where), expected_id(expected.id())
	, actual_id(actual.id())
{
}

TagsetMismatch::TagsetMismatch(const std::string& where,
		tagset_idx_t expected, tagset_idx_t actual)
	: Corpus2Error("Tagset mismatch in " + where), expected_id(expected)
	, actual_id(actual)
{
}

std::string TagsetMismatch::info() const
{
	std::ostringstream ss;
	ss << what() << ". Expected tagset id " << (int)expected_id
		<< ", got id " << actual_id;
	return ss.str();
}


tagset_idx_t Tagset::next_id_ = static_cast<tagset_idx_t>(0);

Tagset::Tagset()
	: id_(++next_id_), valid_pos_mask_(0)
{
}

Tagset Tagset::from_data(const char *s)
{
	std::stringstream ss;
	ss << s;
	return TagsetParser::load_ini(ss);
}

std::string Tagset::id_string() const
{
	std::stringstream ss;
	ss << "tagset " << name_ << " (" << (int)id_ << ")";
	return ss.str();
}

std::string Tagset::id_string(const Tag& tag) const
{
	std::stringstream ss;
	ss << "tagset " << name_ << " (" << (int)id_ << "), ";
	ss << "tag " << tag_to_string(tag);
	return ss.str();
}

Tag Tagset::parse_symbol(const std::string& s) const
{
	mask_t m = get_pos_mask(s);
	if (m.any()) {
		return Tag(m);
	}
	m = get_attribute_mask(s);
	if (m.any()) {
		return Tag(0, m);
	}
	m = get_value_mask(s);
	if (m.any()) {
		return Tag(0, m);
	}
	if (s == "@pos") {
		return Tag(valid_pos_mask_);
	}
	throw TagParseError("Not a tagset symbol", s, "", id_string());
}

Tag Tagset::parse_symbol_string(const std::string &s) const
{
	Tag t;
	std::vector<std::string> parts;
	boost::algorithm::split(parts, s, boost::is_any_of(","));
	BOOST_FOREACH(const std::string& ss, parts) {
		t.combine_with(parse_symbol(ss));
	}
	return t;

}

void Tagset::parse_tag(const string_range &s,
		boost::function<void(const Tag &)> sink,
		ParseMode mode /* = ParseDefault*/) const
{
	string_range_vector fields;
	boost::algorithm::split(fields, s, boost::is_any_of(":"));
	parse_tag(fields, sink, mode);
}

namespace {
	void append_to_multi_tag(
			std::vector< mask_t > & current,
			const std::vector<mask_t> & to_add, mask_t to_add_attr)
	{
		if (to_add.empty()) return;
		size_t current_size = current.size();
		for (size_t ai = 1; ai < to_add.size(); ++ai) {
			for (size_t oi = 0; oi < current_size; ++oi) {
				current.push_back(current[oi]);
				current.back() = (current.back() & ~to_add_attr) | to_add[ai];
			}
		}
		for (size_t i = 0; i < current_size; ++i) {
			current[i] |= to_add[0];
		}
	}
}

void Tagset::parse_tag(const string_range_vector &fields,
		boost::function<void(const Tag &)>sink,
		ParseMode mode /* = ParseDefault*/) const
{
	if (fields.empty()) {
		if (mode & ParseFailWithIgn) {
			sink(make_ign_tag());
			return;
		}
		throw TagParseError("No POS", "", "", id_string());
	}
	idx_t pos_idx = get_pos_index(fields[0]);
	if (pos_idx < 0) {
		if (mode & ParseFailWithIgn) {
			sink(make_ign_tag());
			return;
		}
		throw TagParseError("Invalid POS",
				boost::copy_range<std::string>(fields[0]), "",
				id_string());
	}
	std::vector< mask_t > all_variants;
	all_variants.push_back(0);
	for (size_t fi = 1; fi < fields.size(); ++fi) {
		const string_range& r = fields[fi];
		if (r.size() != 1 || *r.begin() != '_') {
			string_range_vector dots;
			boost::algorithm::split(dots, r, boost::is_any_of("."));
			std::vector<mask_t> values;
			mask_t amask;
			BOOST_FOREACH(string_range& dot, dots) {
				if (dot.empty()) continue;
				mask_t v = get_value_mask(boost::copy_range<std::string>(dot));
				mask_t curr = get_attribute_mask(get_value_attribute(v));


				if (amask.none()) {
					amask = curr;
				} else if (amask != curr) {
					if (mode & ParseFailWithIgn) {
						sink(make_ign_tag());
						return;
					}
					throw TagParseError("Values from two attributes split by dot",
							boost::copy_range<std::string>(r), "",
							id_string());
				}
				if (v.none()) {
					if (mode & ParseFailWithIgn) {
						sink(make_ign_tag());
						return;
					}
					throw TagParseError("Unknown attribute value",
							boost::copy_range<std::string>(r), "",
							id_string());
				}
				values.push_back(v);
			}
			append_to_multi_tag(all_variants, values, amask);
		} else if (!r.empty()) { // underscore handling
			if (fi - 1 >= pos_attributes_[pos_idx].size()) {
				if (mode & ParseFailWithIgn) {
					sink(make_ign_tag());
					return;
				}
				throw TagParseError(
						"Underscore beyond last attribute for this POS",
						"", "", id_string());
			}
			idx_t attr = pos_attributes_[pos_idx][fi - 1];
			mask_t amask = get_attribute_mask(attr);
			append_to_multi_tag(all_variants, attribute_values_[attr], amask);
		} // else empty, do nothing
	}
	BOOST_FOREACH(mask_t variant, all_variants) {
		sink(make_tag(pos_idx, variant, mode));
	}
}

std::vector<Tag> Tagset::parse_tag(const string_range& sr,
		ParseMode mode /* = ParseDefault*/) const
{
	string_range_vector fields;
	boost::algorithm::split(fields, sr, boost::is_any_of(":"));
	return parse_tag(fields, mode);
}

std::vector<Tag> Tagset::parse_tag(const string_range_vector &fields,
		ParseMode mode /* = ParseDefault*/) const
{
	std::vector<Tag> tags;
	parse_tag(fields,
		boost::bind(static_cast<void (std::vector<Tag>::*)(const Tag&)>(&std::vector<Tag>::push_back), boost::ref(tags),_1),
		mode);
	return tags;
}

Tag Tagset::parse_simple_tag(const string_range &s,
		ParseMode mode /* = ParseDefault*/) const
{
	string_range_vector fields;
	boost::algorithm::split(fields, s, boost::is_any_of(std::string(":")));
	return parse_simple_tag(fields, mode);
}

Tag Tagset::parse_simple_tag(const string_range_vector &ts,
		ParseMode mode /* = ParseDefault*/) const
{
	if (ts.empty()) {
		if (mode & ParseFailWithIgn) {
			return make_ign_tag();
		}
		throw TagParseError("Empty POS+attribute list", "", "",
				id_string());
	}
	idx_t pos_idx = get_pos_index(ts[0]);
	if (pos_idx < 0) {
		if (mode & ParseFailWithIgn) {
			return make_ign_tag();
		}
		throw TagParseError("Invalid POS",
				boost::copy_range<std::string>(ts[0]), "", id_string());
	}
	mask_t values = 0;
	for (size_t i = 1; i < ts.size(); ++i) {
		if (!ts[i].empty()) {
			mask_t val = get_value_mask(boost::copy_range<std::string>(ts[i]));
			if (val.none()) {
				mask_t a = get_attribute_mask(ts[i]);
				if (a.any()) {
					values &= (~a);
				} else {
					if (mode & ParseFailWithIgn) {
						return make_ign_tag();
					}
					throw TagParseError("Unknown attribute value",
							boost::copy_range<std::string>(ts[i]), "",
							id_string());
				}
			} else {
				mask_t a = get_attribute_mask(get_value_attribute(val));
				values = (values & ~a) | val;
			}
		}
	}
	return make_tag(pos_idx, values, mode);
}

Tag Tagset::make_tag(idx_t pos_idx, mask_t values,
		ParseMode mode /* = ParseDefault*/) const
{
	mask_t required_values = get_pos_required_mask(pos_idx);
	//std::cerr << values << "\n";
	//std::cerr << required_values << "\n";
	//std::cerr << (required_values & values) << "\n";
	//std::cerr << PwrNlp::count_bits_set(required_values & values)
	//		<< " of " << pos_required_attributes_idx_[pos_idx].size() << "\n";
	if (mode & ParseCheckRequired) {
		size_t has_req = PwrNlp::count_bits_set(required_values & values);
		if (has_req != pos_required_attributes_idx_[pos_idx].size()) {
			BOOST_FOREACH(idx_t a, get_pos_attributes(pos_idx)) {
				if (pos_requires_attribute(pos_idx, a)) {
					mask_t amask = get_attribute_mask(a);
					if ((values & amask).none()) {
						if (mode & ParseFailWithIgn) {
							return make_ign_tag();
						}
						throw TagParseError("Required attribute missing",
							tag_to_string(Tag(get_pos_mask(pos_idx), values)),
							get_attribute_name(a), id_string());
					}
				}
			}
			if (mode & ParseFailWithIgn) {
				return make_ign_tag();
			}
			throw TagParseError("Required attribute missing",
					tag_to_string(Tag(get_pos_mask(pos_idx), values)),
					get_pos_name(pos_idx), id_string());
		}
	}
	if (!(mode & ParseAllowExtra)) {
		mask_t valid_values = get_pos_value_mask(pos_idx);
		mask_t invalid = values & ~valid_values;
		if (invalid.any()) {
			if (mode & ParseFailWithIgn) {
				return make_ign_tag();
			}
			mask_t first_invalid = PwrNlp::lowest_bit(invalid);
			throw TagParseError("Attribute not valid for this POS",
					get_value_name(first_invalid),
					get_pos_name(pos_idx), id_string());
		}
	}
	Tag tag(get_pos_mask(pos_idx), values);
	if (mode & ParseCheckSingular) {
		if (!tag_is_singular(tag)) {
			if (mode & ParseFailWithIgn) {
				return make_ign_tag();
			}
			throw TagParseError("Parsed tag not singular",
					tag_to_symbol_string(tag, false),
					get_pos_name(pos_idx), id_string());
		}
	}
	return tag;
}

Tag Tagset::make_ign_tag() const
{
	return ign_tag_;
}

bool Tagset::validate_tag(const Tag &t, ParseMode mode /* = ParseDefault*/,
		std::ostream* os /* = NULL */) const
{
	if (mode & ParseCheckSingular) {
		if (t.pos_count() != 1) {
			if (os) {
				(*os) << " POS not singular :  " << t.pos_count();
			}
			return false;
		}
		size_t ts = tag_size(t);
		if (ts != 1) {
			if (os) {
				(*os) << " Tag not singular :  " << ts;
			}
			return false;
		}
	}
	idx_t pos_idx = t.get_pos_index();
	if (!pos_dict_.is_id_valid(pos_idx)) {
		if (os) {
			(*os) << " POS not valid : " << (int)pos_idx;
		}
		return false;
	}
	const std::vector<bool>& valid = get_pos_attributes_flag(pos_idx);
	const std::vector<bool>& required = get_pos_required_attributes(pos_idx);

	for (idx_t i = 0; i < attribute_count(); ++i) {
		mask_t value = t.get_values_for(get_attribute_mask(i));
		if (value.none()) {
			if ((mode & ParseCheckRequired) && required[i]) {
				if (os) {
					(*os)  << " red attribuite "
						<< get_attribute_name(i)
						<< " missing";
				}
				return false;
			}
		} else {
			if (!valid[i] && !(mode & ParseAllowExtra)) {
				if (os) {
					(*os) << " Extra attribute value: "
						<< get_value_name(value)
						<< " (attribute "
						<< get_attribute_name(i) << ")";
				}
				return false;
			}
		}
	}
	return true;
}

std::string Tagset::tag_to_string(const Tag &tag) const
{
	std::ostringstream ss;
	idx_t pos_idx = tag.get_pos_index();
	ss << get_pos_name(pos_idx);
	const std::vector<idx_t>& attrs = get_pos_attributes(pos_idx);
	BOOST_FOREACH(const idx_t& a, attrs) {
		mask_t value = tag.get_values_for(get_attribute_mask(a));
		if (pos_requires_attribute(pos_idx, a) || value.any()) {
			ss << ":";
			if (value.any()) {
				ss << get_value_name(value);
			}
		}
	}
	// print extra attributes
	for (idx_t a = 0; a < attribute_count(); ++a) {
		if (!pos_has_attribute(pos_idx, a)) {
			mask_t value = tag.get_values_for(get_attribute_mask(a));
			if (value.any()) {
				ss << ":" << get_value_name(value);
			}
		}
	}
	return ss.str();
}

std::string Tagset::tag_to_no_opt_string(const Tag &tag) const
{
	std::ostringstream ss;
	idx_t pos_idx = tag.get_pos_index();
	ss << get_pos_name(pos_idx);
	const std::vector<idx_t>& attrs = get_pos_attributes(pos_idx);
	BOOST_FOREACH(const idx_t& a, attrs) {
		mask_t value = tag.get_values_for(get_attribute_mask(a));
		ss << ":";
		if (value.any()) {
			ss << get_value_name(value);
		} else {
			ss << get_attribute_name(a);
		}
	}
	return ss.str();
}

std::vector<std::string> Tagset::tag_to_symbol_string_vector(const Tag& tag,
		bool compress_attributes /* = true */) const
{
	std::vector<std::string> ret;
	BOOST_FOREACH(mask_t p, PwrNlp::set_bits(tag.get_pos())) {
		ret.push_back(get_pos_name(p));
	}
	mask_t vals = tag.get_values();
	if (compress_attributes) {
		for (idx_t ai = 0; ai < attribute_count(); ++ai) {
			mask_t amask = get_attribute_mask(ai);
			if ((vals & amask) == amask) {
				vals ^= amask;
				ret.push_back(get_attribute_name(ai));
			}
		}
	}
	BOOST_FOREACH(mask_t p, PwrNlp::set_bits(vals)) {
		ret.push_back(get_value_name(p));
	}
	return ret;
}

std::string Tagset::tag_to_symbol_string(const Tag& tag,
		bool compress_attributes /* = true */) const
{
	return boost::algorithm::join(
			tag_to_symbol_string_vector(tag, compress_attributes), ",");
}

size_t Tagset::tag_size(const Tag& tag) const
{
	size_t s = PwrNlp::count_bits_set(tag.get_pos());
	BOOST_FOREACH(mask_t attribute_mask, all_attribute_masks()) {
		mask_t values = tag.get_values_for(attribute_mask);
		size_t x = PwrNlp::count_bits_set(values);
		if (x > 1) {
			s *= x;
		}
	}
	return s;
}

bool Tagset::tag_is_singular(const Tag& tag) const
{
	if (PwrNlp::count_bits_set(tag.get_pos()) > 1) return false;
	BOOST_FOREACH(mask_t attribute_mask, all_attribute_masks()) {
		mask_t values = tag.get_values_for(attribute_mask);
		if (PwrNlp::count_bits_set(values) > 1) return false;
	}
	return true;
}

std::vector<Tag> Tagset::split_tag(const Tag& tag) const
{
	std::vector<Tag> tags;
	mask_t pos = tag.get_pos();
	while (pos.any()) {
		idx_t pos_idx = PwrNlp::lowest_bit(pos);
		mask_t pos_mask = static_cast<mask_t>(1) << pos_idx;
		pos ^= pos_mask;
		tags.push_back(Tag(pos_mask));
	}

	for (idx_t a = 0; a < attribute_count(); ++a) {
		mask_t ma = get_attribute_mask(a);
		mask_t v = tag.get_values_for(ma);
		if (ma.any()) {
			bool dup = false;
			size_t sz = tags.size();
			BOOST_FOREACH(mask_t vm, get_attribute_values(a)) {
				if ((v & vm).any()) {
					if (dup) {
						for (size_t i = 0; i < sz; ++i) {
							Tag new_tag = tags[i];
							mask_t new_vals = new_tag.get_values();
							new_vals &= ~ma; // clear whole attr
							new_vals ^= vm; // add just the new value
							// tags[i].add_values(vm);
							new_tag.set_values(new_vals);
							tags.push_back(new_tag);
						}
					}
					else {
						dup = true;
						for (size_t i = 0; i < sz; ++i) {
							tags[i].add_values(vm);
						}
					}
				}
			}
		}
	}
	return tags;
}

Tag Tagset::select_singular(const Tag& tag) const
{
	Tag new_tag;
	// force one POS
	idx_t pos_idx = tag.get_pos_index();
	mask_t pos_mask = get_pos_mask(pos_idx);
	new_tag.set_pos(pos_mask);
	// now iterate over attrs
	const std::vector<idx_t>& attrs = get_pos_attributes(pos_idx);
	BOOST_FOREACH(const idx_t& a, attrs) {
		mask_t attr_mask = get_attribute_mask(a);
		mask_t value = tag.get_values_for(attr_mask);
		// check if the attr is multi-value
		if (PwrNlp::count_bits_set(value) > 1)
		{
			if (pos_requires_attribute(pos_idx, a)) {
				// this is a required attr, so just select first value
				idx_t val_bit = PwrNlp::lowest_bit(value);
				// well, this is not POS but attr value but the
				// implementation is ok anyway...
				mask_t one_mask = get_pos_mask(val_bit);
				new_tag.add_values(one_mask);
			}
			// else it is already null
		}
		else {
			// leave the singular value intact
			new_tag.add_values(value);
		}
	}
	return new_tag;
}

Tag Tagset::expand_optional_attrs(const Tag& tag) const
{
	Tag new_tag(tag);
	idx_t pos_idx = tag.get_pos_index();
	BOOST_FOREACH(idx_t a, get_pos_attributes(pos_idx)) {
		mask_t attr_mask = get_attribute_mask(a);
		mask_t value = tag.get_values_for(attr_mask);
		if (!value.any()) { // no value given
			const Tag all_vals(0, attr_mask);
			new_tag.combine_with(all_vals);
		}
	}
	return new_tag;
}

idx_t Tagset::get_pos_index(const string_range& pos) const
{
	return pos_dict_.get_id(pos);
}

const std::string& Tagset::get_pos_name(idx_t pos) const
{
	return pos_dict_.get_string(pos);
}

const std::string& Tagset::get_pos_name(mask_t pos) const
{
	return pos_dict_.get_string(get_pos_index(pos));
}

mask_t Tagset::get_pos_mask(const string_range& pos) const
{
	return get_pos_mask(get_pos_index(pos));
}

mask_t Tagset::get_pos_mask(idx_t pos) const
{
	if (pos >= 0) {
		return static_cast<mask_t>(1) << pos;
	} else {
		return 0;
	}
}

idx_t Tagset::get_pos_index(mask_t pos) const
{
	if (pos.none()) {
		return -1;
	} else {
		return PwrNlp::lowest_bit(pos);
	}
}

idx_t Tagset::get_attribute_index(const string_range& a) const
{
	return attribute_dict_.get_id(a);
}

idx_t Tagset::get_attribute_index(mask_t a) const
{
	std::map<mask_t, idx_t>::const_iterator ci;
	ci = attribute_mask_to_index_.find(a);
	if (ci == attribute_mask_to_index_.end()) {
		return -1;
	} else {
		return ci->second;
	}
}

const std::string& Tagset::get_attribute_name(idx_t a) const
{
	return attribute_dict_.get_string(a);
}

const std::string& Tagset::get_attribute_name(mask_t a) const
{
	static std::string nullstr;
	idx_t index = get_attribute_index(a);
	if (index < 0 || index > attribute_count()) {
		return nullstr;
	} else {
		return attribute_dict_.get_string(index);
	}
}

const std::vector<mask_t>& Tagset::get_attribute_values(idx_t a) const
{
	static std::vector<mask_t> null_vec;
	if (a < 0 || a >= attribute_count()) {
		return null_vec;
	} else {
		return attribute_values_[a];
	}
}

mask_t Tagset::get_attribute_mask(idx_t a) const
{
	if (a < 0 || a >= attribute_count()) {
		return 0;
	} else {
		return attribute_masks_[a];
	}
}

mask_t Tagset::get_attribute_mask(const string_range& a) const
{
	return get_attribute_mask(get_attribute_index(a));
}

mask_t Tagset::get_value_mask(const std::string& v) const
{
	std::map<std::string, mask_t>::const_iterator ci;
	ci = string_to_value_mask_.find(v);
	if (ci == string_to_value_mask_.end()) {
		return 0;
	} else {
		return ci->second;
	}
}

const std::string& Tagset::get_value_name(mask_t v) const
{
	static std::string nullstr;
	std::map<mask_t, std::string>::const_iterator ci;
	ci = value_mask_to_string_.find(v);
	if (ci == value_mask_to_string_.end()) {
		return nullstr;
	} else {
		return ci->second;
	}
}

idx_t Tagset::get_value_attribute(mask_t v) const
{
	std::map<mask_t, idx_t>::const_iterator ci;
	ci = value_mask_to_attribute_index_.find(v);
	if (ci == value_mask_to_attribute_index_.end()) {
		return -1;
	} else {
		return ci->second;
	}
}

const std::vector<idx_t>& Tagset::get_pos_attributes(idx_t pos) const
{
	assert(pos_dict_.is_id_valid(pos));
	return pos_attributes_[pos];
}

const std::vector<bool>& Tagset::get_pos_attributes_flag(
		idx_t pos) const
{
	assert(pos_dict_.is_id_valid(pos));
	return pos_valid_attributes_[pos];
}

const std::vector<bool>& Tagset::get_pos_required_attributes(
		idx_t pos) const
{
	assert(pos_dict_.is_id_valid(pos));
	return pos_required_attributes_[pos];
}

bool Tagset::pos_requires_attribute(idx_t pos, idx_t attribute) const
{
	return pos_required_attributes_[pos][attribute];
}

bool Tagset::pos_has_attribute(idx_t pos, idx_t attribute) const
{
	return pos_valid_attributes_[pos][attribute];
}

mask_t Tagset::get_pos_value_mask(idx_t pos) const
{
	return pos_valid_value_masks_[pos];
}

mask_t Tagset::get_pos_required_mask(idx_t pos) const
{
	return pos_required_value_masks_[pos];
}

int Tagset::pos_count() const
{
	return pos_dict_.size();
}

int Tagset::attribute_count() const
{
	return attribute_dict_.size();
}

int Tagset::value_count() const
{
	return value_mask_to_string_.size();
}

size_t Tagset::size() const
{
	size_t sum = 0;
	for (size_t p = 0; p < pos_dict_.size(); ++p) {
		size_t pos_size = 1;
		for (size_t i = 0; i < pos_attributes_[p].size(); ++i) {
			idx_t a = pos_attributes_[p][i];
			if (pos_required_attributes_[p][a]) {
				pos_size *= attribute_values_[a].size();
			} else {
				pos_size *= (attribute_values_[a].size() + 1);
			}
		}
		sum += pos_size;
	}
	return sum;
}

double Tagset::size_extra() const
{
	double vs = 1;
	for (size_t a = 0; a < attribute_dict_.size(); ++a) {
		vs *= (attribute_values_[a].size() + 1);
	}
	vs *= pos_dict_.size();
	return vs;
}

void Tagset::lexemes_into_token(Token& tok, const UnicodeString& lemma,
		const string_range& tags) const
{
	string_range_vector options;
	boost::algorithm::split(options, tags, boost::is_any_of("+|"));

	boost::function<Lexeme (const Tag&)> lex;
	lex = boost::bind(&Lexeme::create, boost::cref(lemma), _1);

	boost::function<void (const Tag&)> func;
	func = boost::bind(&Token::add_lexeme, boost::ref(tok),
			boost::bind(lex, _1));

	BOOST_FOREACH(const string_range& o, options) {
		parse_tag(o, func);
	}
}

int Tagset::get_original_pos_index(idx_t pos) const
{
	std::map<idx_t, int>::const_iterator i =
			original_pos_indices_.find(pos);
	if (i != original_pos_indices_.end()) {
		return i->second;
	} else {
		return -1;
	}
}

} /* end ns Corpus2 */
