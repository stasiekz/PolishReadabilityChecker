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

#ifndef LIBMACA_TAGCONVERT_H
#define LIBMACA_TAGCONVERT_H

#include <libcorpus2/tagset.h>
#include <libmaca/conv/layer.h>
#include <libmaca/util/confignode.h>

namespace Maca {
namespace Conversion {

/**
 * A tag-level converter that maps values from one tagset into another.
 * The default behavior is to map by an exact name match, so that a value
 * foo in attribute bar of pos baz will remain so in the output tagset.
 * If some names have no counterparts in the output tagset, the value will
 * not be set at all. Overrides are possible to correct such cases and to
 * perform some basic rearranging.
 */
class TagConverter
{
public:
	/**
	 * Corpus2::Tag converter constructor, fills the action maps with name-based
	 * defaults.
	 */
	TagConverter(const Corpus2::Tagset& from, const Corpus2::Tagset& to);

	/**
	 * The actual conversion function which takes a tag from one tagset
	 * and returns a converted tag in another tagset.
	 */
	Corpus2::Tag cast(const Corpus2::Tag& from) const;

	/// The input tagset
	const Corpus2::Tagset& tagset_from() const {
		return tagset_from_;
	}

	/// The output tagset
	const Corpus2::Tagset& tagset_to() const {
		return tagset_to_;
	}

	/**
	 * Override function, make the a string in the input tagset map to
	 * a string in the output tagset.
	 */
	void add_override(const std::string& from, const std::string& to);

	/**
	 * Checks if all POS and values are properly mapped
	 * @param os Optional stream for error explanation
	 * @param all Continue checking and outputting errors after a fault
	 * @returns true if the mapping is complete
	 */
	bool is_complete(std::ostream* os = NULL, bool all = false) const;

	void set_late_check(bool v) {
		late_check_ = v;
	}

protected:
	/// Input tagset
	const Corpus2::Tagset& tagset_from_;
	/// Output tagset
	const Corpus2::Tagset& tagset_to_;

	/// Value mapping
	typedef std::map<Corpus2::mask_t, Corpus2::mask_t> value_map_t;
	value_map_t value_mapping_;

	/// Attribute mapping
	typedef std::map<Corpus2::idx_t, Corpus2::idx_t> attribute_map_t;
	attribute_map_t attribute_mapping_;

	/// POS mapping
	typedef std::map<Corpus2::mask_t, Corpus2::mask_t> pos_map_t;
	pos_map_t pos_mapping_;

	bool late_check_;
};

/**
 * A layer wrapper around a TagConverter
 */
class TagConvertLayer : public Layer
{
public:
	/**
	 * Create a TagConvertLayer from an existing TagConverter
	 */
	TagConvertLayer(const TagConverter& tc);

	/**
	 * Create a TagConvertLayer from a config
	 */
	TagConvertLayer(const Config::Node& cfg);

	/// Cloning
	TagConvertLayer* clone() const;

	/// TokeSource override
	Corpus2::Token* get_next_token();

	/// Layer override -- the input tagset
	const Corpus2::Tagset& tagset_from() const;

	/// Layer override -- the output tagset
	const Corpus2::Tagset& tagset_to() const;

private:
	/// The tag converter object
	TagConverter tc_;
};

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_TAGCONVERT_H
