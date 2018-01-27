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

#ifndef LIBWCCL_LEXICON_LEXICON_H
#define LIBWCCL_LEXICON_LEXICON_H

#include <boost/unordered_map.hpp>
#include <boost/noncopyable.hpp>
#include <libcorpus2/lexeme.h> // for unicodestring hash

#include <libwccl/values/strset.h>

namespace Wccl {

class Lexicon : boost::noncopyable
{
public: 
	typedef boost::unordered_map<UnicodeString, std::vector<UnicodeString> > map_t;

	Lexicon(const std::string& name, const std::string& file_name)
		: name_(name),
		  file_name_(file_name)
	{
		BOOST_ASSERT(!name_.empty());
	}

	/**
	 * Translate given key to a value held in this lexicon.
	 * @returns Value assigned to the given key, if present.
	 *          Empty UnicodeString if the key was not present.
	 */
	boost::shared_ptr<StrSet> translate(const UnicodeString& key) const;


	/**
	 * Translate given set of strings to corresponding values
	 * from the lexicon. 
	 * Nonexisting keys will translate to nothing (will be removed
	 * from output).
	 */
	boost::shared_ptr<StrSet> translate(const StrSet& set) const;

	std::string name() const {
		return name_;
	}

	std::string file_name() const {
		return file_name_;
	}

	bool has_key(const UnicodeString& key) const {
		return map_.find(key) != map_.end();
	}

	void insert(const UnicodeString& key, const UnicodeString& value);
	void insert(const UnicodeString& key) {
		insert(key, key);
	}

	const map_t& map() const {
		return map_;
	}
private:
	map_t map_;
	const std::string name_;
	const std::string file_name_;
};

} /* end ns Wccl */

#endif // LIBWCCL_LEXICON_LEXICON_H
