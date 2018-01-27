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

#ifndef LIBWCCL_VALUES_STRSET_H
#define LIBWCCL_VALUES_STRSET_H

#include <libwccl/values/value.h>
#include <boost/unordered_set.hpp>
#include <libcorpus2/lexeme.h> // for unicodestring hash

namespace Wccl {

/**
 * A Value subtype representing a set of strings.
 *
 * No guarantees for the order of elements are given at this time.
 *
 * By default the set is empty.
 */
class StrSet : public Value
{
public:
	WCCL_VALUE_PREAMBLE

	//typedef boost::unordered_set<UnicodeString> value_type;
	typedef std::set<UnicodeString> value_type;

	StrSet()
		: set_()
	{
	}

	explicit StrSet(const value_type& s)
		: set_(s)
	{
	}

	// Good job, boost. https://svn.boost.org/trac/boost/ticket/6167
	// "Assignment from a temporary of a class containing
	// boost::unordered_map members fails with GNU GCC"
	// Work around this by manually defining op=(const&), otherwise
	// on boost 1.48 the following code will fail in non-c++0x mode:
	//     StrSet s;
	//     s = StrSet(); //compile error
	StrSet& operator=(const StrSet& s) {
		set_ = s.set_;
		return *this;
	}


	const value_type& get_value() const {
		return set_;
	}

	void set_value(const value_type& set) {
		set_ = set;
	}

	/**
	 * get_value() alias.
	 */
	const value_type& contents() const {
		return set_;
	}

	/**
	 * Nonconst variant of get_value()
	 */
	value_type& contents() {
		return set_;
	}

	/**
	 * Wrapper for boost::unordered set
	 */
	std::set<UnicodeString> to_std_set() const {
		return std::set<UnicodeString>(this->set_.begin(), this->set_.end());
	}

	void swap(StrSet& ss) {
		ss.set_.swap(set_);
	}

	/// Convenience function to add a new UnicodeString to the set
	void insert(const UnicodeString& u) {
		set_.insert(u);
	}

	/// Convenience function to add a new string to the set, treated as UTF-8
	void insert_utf8(const std::string& u) {
		insert(UnicodeString::fromUTF8(u));
	}

	/// Convenience size accesor
	int size() const {
		return set_.size();
	}

	/// Convenience empty checker
	bool empty() const {
		return set_.empty();
	}

	/**
	 * @return true if each string from this set exists in the other set
	 *         (note that an empty set is a subset of anything)
	 */
	bool is_subset_of(const StrSet& other) const;

	/**
	 * @return true if there is at least one common string between this set and
	 *         the other set (an empty set intersects with nothing)
	 */
	bool intersects(const StrSet& other) const;

	bool equals(const StrSet& other) const {
		return set_ == other.set_;
	}

	/// Value override
	std::string to_raw_string() const;

	/// Value override
	UnicodeString to_raw_string_u() const;

	/// Value override
	std::string to_compact_string(const Corpus2::Tagset& tagset) const;

	/// Value override
	UnicodeString to_compact_string_u(const Corpus2::Tagset& tagset) const;

private:
	value_type set_;
};

} /* end ns Wccl */

#endif // LIBWCCL_VALUES_STRSET_H
