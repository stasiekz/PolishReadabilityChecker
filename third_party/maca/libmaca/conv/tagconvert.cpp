/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

#include <libmaca/conv/tagconvert.h>
#include <libcorpus2/token.h>
#include <libcorpus2/tagsetmanager.h>
#include <boost/foreach.hpp>
#include <bitset>

namespace Maca {
namespace Conversion {

TagConverter::TagConverter(const Corpus2::Tagset& from, const Corpus2::Tagset& to)
	: tagset_from_(from), tagset_to_(to), late_check_(true)
{
	for (Corpus2::idx_t pos = 0; pos < from.pos_count(); ++pos) {
		Corpus2::mask_t from_mask = from.get_pos_mask(pos);
		const std::string& name = from.get_pos_name(pos);
		Corpus2::mask_t to_mask = to.get_pos_mask(name);
		if (to_mask.any()) {
			pos_mapping_.insert(std::make_pair(from_mask, to_mask));
		}
	}
	from.attribute_dictionary().create_mapping_to(
			to.attribute_dictionary(), attribute_mapping_);

	BOOST_FOREACH(Corpus2::mask_t from_mask, from.all_value_masks()) {
		const std::string& name = from.get_value_name(from_mask);
		if (name.empty()) {
			//std::cerr << "WUT " << bs << "\n";
		}
		Corpus2::mask_t to_mask = to.get_value_mask(name);
		if (to_mask.any()) {
			value_mapping_.insert(std::make_pair(from_mask, to_mask));
		}
	}
}

Corpus2::Tag TagConverter::cast(const Corpus2::Tag& from) const
{
	pos_map_t::const_iterator pi = pos_mapping_.find(from.get_pos());
	if (pi == pos_mapping_.end()) {
		std::stringstream msg;
		msg
				<< "Don't know how to convert grammatical class '"
				<< tagset_from().get_pos_name(from.get_pos())
				<< "' from tagset "
				<< tagset_from().name()
				<< " (missing override rule in .conv file?)";
		throw MacaError(msg.str());
	}
	Corpus2::Tag to(pi->second);

	Corpus2::mask_t values_left = from.get_values();
	BOOST_FOREACH(const attribute_map_t::value_type& v, attribute_mapping_) {
		Corpus2::mask_t amask = tagset_from().get_attribute_mask(v.first);
		Corpus2::mask_t value = from.get_values_for(amask);
		if (value.any()) {
			value_map_t::const_iterator vi = value_mapping_.find(value);
			if (vi != value_mapping_.end()) {
				to.add_values(vi->second);
			} else {
				if (late_check_) {
					std::cerr << "TagConverter: Value not found in map: "
						<< (value) << " "
						<< tagset_from().get_value_name(value)
						<< "\n";
				}
			}
			values_left &= (~amask);
		}
	}
	return to;
}

void TagConverter::add_override(const std::string& from,
		const std::string& to)
{
	Corpus2::mask_t pos = tagset_from_.get_pos_mask(from);
	if (pos.any()) {
		Corpus2::mask_t pos2 = tagset_to_.get_pos_mask(to);
		if (pos2.any()) {
			pos_mapping_[pos] = pos2;
		} else {
			std::cerr << "Invalid POS/POS mapping override, "
				<< to << " not a valid POS in "
				<< tagset_to().id_string() << "\n";
		}
	} else {
		Corpus2::mask_t vto = tagset_to_.get_value_mask(to);
		if (vto.any()) {
			Corpus2::idx_t aidx = tagset_from_.get_attribute_index(from);
			if (aidx >= 0) {
				BOOST_FOREACH(Corpus2::mask_t vfrom,
						tagset_from_.get_attribute_values(aidx)) {
					value_mapping_[vfrom] = vto;
				}
			} else {
				Corpus2::mask_t vfrom = tagset_from_.get_value_mask(from);
				if (vfrom.any()) {
					value_mapping_[vfrom] = vto;
				} else {
					std::cerr << "Invalid value mapping override, "
						<< from << " not a valid value or attribute in "
						<< tagset_from().id_string() << "\n";
				}
			}
		} else {
			std::cerr << "Invalid mapping override, "
				<< to << " not a valid symbol in "
				<< tagset_to().id_string() << "\n";
		}
	}
}

bool TagConverter::is_complete(std::ostream* os, bool all /*=false*/) const
{
	bool rv = true;
	for (Corpus2::idx_t p = 0; p < tagset_from().pos_count(); ++p) {
		Corpus2::mask_t pos = tagset_from().get_pos_mask(p);
		pos_map_t::const_iterator pi = pos_mapping_.find(pos);
		if (pi == pos_mapping_.end()) {
			if (os) (*os) << "No mapping for POS "
				<< tagset_from().pos_dictionary().get_string(p)
				<< " (" << (int)p << ")";
			rv = false;
			if (!all) return rv;
			if (os) (*os) << "\n";
		}

		if (tagset_to().get_pos_name(pi->second).empty()) {
			if (os) (*os) << "Mapping for POS "
				<< tagset_from().get_pos_name(p) << " ("
				<< p << ") is invalid (" << pi->second << ")";
			rv = false;
			if (!all) return rv;
			if (os) (*os) << "\n";
		}
	}
	for (Corpus2::idx_t p = 0; p < tagset_from().attribute_count(); ++p) {
		attribute_map_t::const_iterator pi = attribute_mapping_.find(p);
		if (pi == attribute_mapping_.end()) {
			if (os) (*os) << "No mapping for attribute "
				<< tagset_from().get_attribute_name(p)
				<< " (" << (int)p << ")";
			rv = false;
			if (!all) return rv;
			if (os) (*os) << "\n";
		} else {
			Corpus2::idx_t to = pi->second;
			if (tagset_to().get_attribute_name(to).empty()) {
				if (os) (*os) << "Mapping for attribute "
					<< tagset_from().attribute_dictionary().get_string(p)
					<< " (" << p << ")" << " is invalid"
					<< " (" << to << ")";
				rv = false;
				if (!all) return rv;
				if (os) (*os) << "\n";
			}
		}
	}
	for (Corpus2::idx_t p = 0; p < tagset_from().value_count(); ++p) {
		Corpus2::mask_t val = (Corpus2::mask_t)1 << p;
		value_map_t::const_iterator pi = value_mapping_.find(val);
		if (pi == value_mapping_.end()) {
			if (os) (*os) << "No mapping for value "
				<< tagset_from().get_value_name(val)
				<< " (" << (int)p << ")";
			rv = false;
			if (!all) return rv;
			if (os) (*os) << "\n";
		}
		if (tagset_to().get_value_name(pi->second).empty()) {
			if (os) (*os) << "Mapping for value "
				<< tagset_from().get_value_name(p) << " ("
				<< p << ") is invalid (" << pi->second << ")";
			rv = false;
			if (!all) return rv;
			if (os) (*os) << "\n";
		}
	}
	return rv;
}

TagConvertLayer::TagConvertLayer(const TagConverter &tc)
	: tc_(tc)
{
}

TagConvertLayer* TagConvertLayer::clone() const
{
	return new TagConvertLayer(*this);
}

TagConvertLayer::TagConvertLayer(const Config::Node& cfg)
	: tc_(Corpus2::get_named_tagset(cfg.get<std::string>("tagset_from")),
		Corpus2::get_named_tagset(cfg.get<std::string>("tagset_to")))
{
	BOOST_FOREACH(const Config::Node::value_type &v, cfg) {
		if (v.first == "override") {
			std::string o = v.second.data();
			size_t colon = o.find(':');
			if (colon != std::string::npos) {
				std::string o_from = o.substr(0, colon);
				std::string o_to = o.substr(colon + 1);
				tc_.add_override(o_from, o_to);
			}
		}
	}
	bool check_all = cfg.get("check-all", true);
	std::string check = cfg.get("check", "warn");
	if (check == "warn") {
		tc_.is_complete(&std::cerr, check_all);
	} else if (check == "err") {
		if (!tc_.is_complete(&std::cerr, check_all)) {
			throw MacaError("TagConverter not complete and check=err");
		}
	} else if (check != "ignore") {
		throw MacaError("TagConverter check neither warn, err nor ignore");
	}
	tc_.set_late_check(cfg.get("late-check", true));
}

Corpus2::Token* TagConvertLayer::get_next_token()
{
	Corpus2::Token* t = source()->get_next_token();
	if (t != NULL) {
		BOOST_FOREACH(Corpus2::Lexeme& lex, t->lexemes()) {
			lex.set_tag(tc_.cast(lex.tag()));
		}
		t->remove_duplicate_lexemes();
	}
	return t;
}

const Corpus2::Tagset& TagConvertLayer::tagset_from() const
{
	return tc_.tagset_from();
}

const Corpus2::Tagset& TagConvertLayer::tagset_to() const
{
	return tc_.tagset_to();
}

} /* end ns Conversion */
} /* end ns Maca */
