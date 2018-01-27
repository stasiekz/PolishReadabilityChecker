/*
    Copyright (C) 2011 Adam Wardyński, Tomasz Śniatowski, Paweł Kędzia,
    Adam Radziszewski, Bartosz Broda
    Part of the WCCL project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBWCCL_VALUES_TSET_H
#define LIBWCCL_VALUES_TSET_H

#include <libwccl/values/value.h>
#include <libcorpus2/tag.h>

namespace Wccl {

/**
 * A Value subtype representing a set of tagset symbols, essentially
 * a wrapper on a Corpus2::Tag.
 *
 * Default-constructed TSets are empty.
 */
class TSet : public Value
{
public:
	WCCL_VALUE_PREAMBLE;

	typedef Corpus2::Tag value_type;

	TSet()
		: tag_()
	{
	}

	explicit TSet(Corpus2::Tag tag)
		: tag_(tag)
	{
	}

	const Corpus2::Tag& get_value() const {
		return tag_;
	}

	void set_value(const Corpus2::Tag& tag) {
		tag_ = tag;
	}

	/**
	 * Alias of get_value()
	 */
	const Corpus2::Tag& contents() const {
		return tag_;
	}

	/**
	 * Nonconst variant of contents()
	 */
	Corpus2::Tag& contents() {
		return tag_;
	}

	/**
	 * Convenience function to add a symbol from a tagset by name.
	 *
	 * Note: slow. Avoid in code that gets repeatedly executed.
	 */
	void insert_symbol(const Corpus2::Tagset& tagset, const std::string& s);

	bool empty() const {
		return tag_.is_null();
	}

	size_t size() const {
		return tag_.pos_count() + PwrNlp::count_bits_set(tag_.get_values());
	}

	/**
	 * @return true if each tagset symbol from this set exists in the other set
	 *         (note that an empty set is a subset of anything)
	 */
	bool is_subset_of(const TSet& other) const {
		return tag_.get_masked(other.tag_) == tag_;
	}

	/**
	 * @return true if there is at least one common symbol between this set and
	 *         the other set (an empty set intersects with nothing)
	 */
	bool intersects(const TSet& other) const {
		return !tag_.get_masked(other.tag_).is_null();
	}

	bool equals(const TSet& other) const {
		return tag_ == other.tag_;
	}

	/**
	 * @return Number of categories present in this symbol set according
	 *         to supplied tagset.
	 * @note A category is word class or an attribute.
	 */
	int categories_count(const Corpus2::Tagset& tagset) const;

	/**
	 * @return How many categories present in the supplied tag match with
	 *         this symbol set.
	 * @warning The underlying assumption is that the supplied tag has at most
	 *          1 value per category. Otherwise the value will be incorrect.
	 * @note The symbol set may have partially defined categories. Only values
	 *       present in this symbol set count when matching values in the tag.
	 */
	int matching_categories(const Corpus2::Tag& tag) const;

	void combine_with(const Corpus2::Tag& other) {
		tag_.combine_with(other);
	}

	void combine_with(const TSet& other) {
		tag_.combine_with(other.get_value());
	}

	std::string to_string(const Corpus2::Tagset &) const;

	std::string to_raw_string() const;

	/// Value override
	std::string to_compact_string(const Corpus2::Tagset& tagset) const;

private:
	Corpus2::Tag tag_;
};


} /* end ns Wccl */

#endif // LIBWCCL_VALUES_TSET_H
