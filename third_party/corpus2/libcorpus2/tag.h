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

#ifndef LIBCORPUS2_TAG_H
#define LIBCORPUS2_TAG_H

#include <string>
#include <vector>
#include <cassert>
#include <boost/cstdint.hpp>
#include <boost/serialization/strong_typedef.hpp>
#include <boost/operators.hpp>
#include <libpwrutils/bitset.h>

namespace Corpus2 {

class Tagset;

/// Typedefs for the string -> index mappings
typedef boost::int8_t idx_t;
typedef PwrNlp::bitset<64> mask_t;
BOOST_STRONG_TYPEDEF(boost::uint32_t, tagset_idx_t);

/// Full mask (1's only), may be used for all-POS tags.
const mask_t filled_mask(PwrNlp::filled_bitset<64>());

/**
 * This is an extended version of a POS tag, which also includes a number
 * of attributes which may have values set.
 *
 * The tag is stored in a compact binary firmat, with POS names etc. mapped to
 * numbers. Tags don't "know" their tagset. To perform some basic operations,
 * you an appropriate Tagset object. These operations include tag creation,
 * getting tag string representation and retrieving values of particular
 * attributes.
 *
 * NOTE: tags are essentialy binary masks, hence they may be either valid
 * "singular" tags or they may be used as POS or attribute masks used to
 * retrieve values of some attributes/POS from another tags. Note that given
 * a Tag object alone it is not possible to determine if a tag is singular or
 * valid. See comments of the Tagset class for those details.
 */
class Tag
//	: boost::equality_comparable<Tag>, boost::less_than_comparable<Tag>
{
public:
	/// Empty tag constructor
	Tag()
		: pos_(0), values_(0)
	{
	}

	/// Tagset-and-POS (no values) constructor
	explicit Tag(mask_t pos)
		: pos_(pos), values_(0)
	{
	}

	/// Tagset-POS-values constructor
	Tag(mask_t pos, mask_t values)
		: pos_(pos), values_(values)
	{
	}

	bool is_null() const {
		return pos_.none() && values_.none();
	}

	int pos_count() const;

	int get_pos_index() const;

	/// POS (part-of-speech) accessor
	mask_t get_pos() const {
		return pos_;
	}

	/// POS setter
	void set_pos(mask_t v) {
		pos_ = v;
	}

	void add_pos(mask_t v) {
		pos_ |= v;
	}

	/// values accessor
	mask_t get_values() const {
		return values_;
	}

	mask_t get_values_for(mask_t mask) const {
		return values_ & mask;
	}

	/// values accessor -- nonconst reference
	void set_values(mask_t v) {
		values_ = v;
	}

	void add_values(mask_t v) {
		values_ |= v;
	}

	void add_values_masked(mask_t value, mask_t mask) {
		//values_ = (values_ & ~mask) | (value & mask);
		//see http://graphics.stanford.edu/~seander/bithacks.html#MaskedMerge
		values_ = values_ ^ ((values_ ^ value) & mask);
	}

	Tag& combine_with(const Tag& other) {
		pos_ |= other.pos_;
		values_ |= other.values_;
		return *this;
	}

	Tag get_combined(const Tag& other) const {
		Tag t(*this);
		return t.combine_with(other);
	}

	Tag& mask_with(const Tag& other) {
		pos_ &= other.pos_;
		values_ &= other.values_;
		return *this;
	}

	Tag get_masked(const Tag& other) const {
		Tag t(*this);
		return t.mask_with(other);
	}

	/// debug aid, dump the tag's internal numeric representation
	std::string raw_dump() const;

	/**
	 * Tag comparison.
	 */
	bool operator<(const Tag& other) const;

	/**
	 * Tag equality.
	 */
	bool operator==(const Tag& other) const;

	/**
	 * Tag inequality.
	 */
	bool operator!=(const Tag& other) const {
		return !((*this) == other);
	}

	/**
	 * Boost seralization support. Users must include appropriate boost headers.
	 */
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & pos_;
		ar & values_;
	}

	/**
	  * A mask with the whole POS part filled with 1's, useful to extract POS
	  * values from other tags.
	  */
	const static Tag all_pos_mask;

private:
	/// the POS id
	mask_t pos_;

	/// the values
	mask_t values_;

};

size_t hash_value(const Tag &tag);

} /* end ns Corpus2 */


#endif // LIBCORPUS2_TAG_H
