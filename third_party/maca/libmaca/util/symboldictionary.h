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

#ifndef LIBMACA_SYMBOLDICTIONARY_H
#define LIBMACA_SYMBOLDICTIONARY_H

#include <libmaca/typedefs.h>
#include <vector>
#include <string>
#include <boost/range.hpp>
#include <map>
#include <iostream>

namespace Maca {

/**
 * A template for string-index dictionaries offering lookups in both ways.
 *
 * A dictionary is created from a set of strings, which is then mapped to
 * indices which can be used.
 *
 * The class is templated on the index type, which should be some form of
 * an integer. The amount of used indices should be less than the maximum
 * value of the type. There should be no empty strings or duplicate
 * strings.
 */
template <typename IndexT>
class SymbolDictionary
{
public:
	/// Empty dictionary constructor
	SymbolDictionary();

	/// Load data into the dictionary
	void load_data(const std::vector<std::string>& data);

	/// Load data into the dictionary. The strings in the vector are
	/// assumed to be already sorted.
	void load_sorted_data(const std::vector<std::string>& data);

	/// Check if an index is valid in this dictionary
	bool is_id_valid(IndexT idx) const;

	/// Getter for the size of this dictionary
	size_t  size() const;

	/**
	 * Get the index for a given string identifier, const char* version.
	 *
	 * Essentially a wrapper for the range overload, needed to avoid
	 * confusion when a static char array is passed there.
	 */
	IndexT get_id(const char* c) const;

	/**
	 * Get the index for a given string identifier, range version.
	 *
	 * @returns a valid index into this dictionary, or an index equivalent
	 * to -1
	 */
	IndexT get_id(const string_range& r) const;

	/// Get the string identifier for a valid index into the dictionary.
	/// If the index is not valid, an empty string is returned.
	const std::string& get_string(IndexT id) const;

	/// Plumbing allow BOOST_FOREACH() iteration through the dictionary objects
	typedef typename std::vector<std::string>::iterator iterator;
	typedef typename std::vector<std::string>::const_iterator
			const_iterator;
	typedef typename std::vector<std::string>::value_type value_type;

	iterator begin() {
		return data_.begin();
	}

	const_iterator begin() const {
		return data_.begin();
	}

	iterator end() {
		return data_.end();
	}

	const_iterator end() const {
		return data_.end();
	}

	/**
	 * Create a old-index to new-index mapping between two dictionaries
	 * with mappings based on string identifier equality. Missing mappings
	 * are not set.
	 */
	void create_mapping_to(const SymbolDictionary<IndexT>& other,
			std::map<IndexT, IndexT>& map) const;

private:
	/// The contents of the dictionary (a sorted vector)
	std::vector<std::string> data_;

	/// empty string to be returned by get_string on invalid indices
	static std::string nullstr;
};

template <typename IndexT>
std::string SymbolDictionary<IndexT>::nullstr;

template <typename IndexT>
SymbolDictionary<IndexT>::SymbolDictionary()
	: data_()
{
}

template <typename IndexT>
void SymbolDictionary<IndexT>::load_data(
		const std::vector<std::string> &data)
{
	data_ = data;
	std::sort(data_.begin(), data_.end());
}

template <typename IndexT>
void SymbolDictionary<IndexT>::load_sorted_data(
		const std::vector<std::string> &data)
{
	data_ = data;
}

template <typename IndexT>
bool SymbolDictionary<IndexT>::is_id_valid(IndexT idx) const
{
	return static_cast<size_t>(idx) < data_.size();
}

template <typename IndexT>
size_t SymbolDictionary<IndexT>::size() const
{
	return data_.size();
}

template <typename IndexT>
IndexT SymbolDictionary<IndexT>::get_id(const char *c) const
{
	return get_id(std::make_pair(c, c + strlen(c)));
}

template <typename IndexT>
IndexT SymbolDictionary<IndexT>::get_id(const string_range& r) const
{
	//std::cerr << "get_id called with '" << r << "' (" << r.size() << ")\n";
	boost::sub_range< const std::vector<std::string> > sr =
			std::equal_range(data_.begin(), data_.end(), r);
	if (!sr.empty()) {
		return static_cast<IndexT>(sr.begin() - data_.begin());
	} else {
		return static_cast<IndexT>(-1);
	}
}

template <typename IndexT>
const std::string& SymbolDictionary<IndexT>::get_string(IndexT id) const
{
	size_t idx = static_cast<size_t>(id);
	if (id < data_.size()) {
		return data_[idx];
	} else {
		return nullstr;
	}
}

template <typename IndexT>
void SymbolDictionary<IndexT>::create_mapping_to(
		const SymbolDictionary<IndexT> &other,
		std::map<IndexT, IndexT> &map) const
{
	for (IndexT i = static_cast<IndexT>(0); i < size(); ++i) {
		std::string name = get_string(i);
		IndexT t = other.get_id(name);
		if (other.is_id_valid(t)) {
			map.insert(std::make_pair(i, t));
		}
	}
}

} /* end ns Maca */

#endif // LIBMACA_SYMBOLDICTIONARY_H
