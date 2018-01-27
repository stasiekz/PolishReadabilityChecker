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

#ifndef LIBMACA_MAPANALYSER_H
#define LIBMACA_MAPANALYSER_H

#include <libmaca/morph/morphanalyser.h>
#include <libmaca/util/settings.h>

#include <libtoki/util/confignode.h>
#include <boost/foreach.hpp>

#include <boost/unordered_map.hpp>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <libmaca/typedefs.h>

namespace Maca {

/**
 * A standard-container-based analyser template for use with map-like
 * containers. Data is loaded from a file in tab-separated format of
 * orth, lemma and tag-string triplets.
 *
 * Distinct variants of this template are provided for case sensitive and
 * cas insensitive analysis, and std::map or boost::unordered_map backend.
 *
 * std::map, case-sensitive configuration class key: \b map-case
 * std::map, case-insensitive configuration class key: \b map
 * unordered_map, case-sensitive configuration class key: \b hashmap-case
 * unordered_map, case-insensitive configuration class key: \b hashmap
 */
template<typename MapT>
class MapAnalyser : public MorphAnalyser
{
public:
	/// Constructor for an empty analyser working with a tagset
	explicit MapAnalyser(const Corpus2::Tagset* tagset);

	/**
	 * Config node constructor. Recognized keys are:
	 * - data - the data file to load
	 */
	explicit MapAnalyser(const Config::Node& cfg);

	/// Cloning
	MapAnalyser* clone() const;

	/// Data file loading function
	void load_m_dictionary(const std::string& fn);

	/// MorphAnalyser override
	bool process_functional(const Toki::Token &t,
			boost::function<void (Corpus2::Token*)> sink);

	/// Class identifier
	static const char* identifier;

	/// Registered flag
	static bool registered;

private:
	/// the orth to analysis map
	MapT map_;
};

/// Helper struct for ICU string caseless compare
struct IcuStringCaselessCompare
{
	bool operator()(const UnicodeString& u1,
			const UnicodeString& u2) const {
		return u1.caseCompare(u2, 0) < 0;
	}
};

/// Helper struct for ICU hashing
struct IcuHash
{
	std::size_t operator()(const UnicodeString& x) const
	{
		return x.hashCode();
	}
};

/// Helper struct for ICU string caseless compare and hashing
struct IcuStringCaselessEqual
{
	bool operator()(const UnicodeString& u1,
			const UnicodeString& u2) const {
		return u1.caseCompare(u2, 0) == 0;
	}

	std::size_t operator()(const UnicodeString& x) const
	{
		UnicodeString xx = x;
		xx.toLower();
		return xx.hashCode();
	}
};


/// typedef for a tree-map (std::map) analyser
typedef MapAnalyser<
	std::map<
		UnicodeString,
		std::vector< std::pair<std::string, std::string>
		>
	>
> StdMapAnalyser;

/// typedef for a tree-map (std::map) analyser, caseless
typedef MapAnalyser<
	std::map<
		UnicodeString,
		std::vector< std::pair<std::string, std::string> >,
		IcuStringCaselessCompare
	>
> StdMapCaselessAnalyser;

/// typedef for a hashmap (std::unordered_map) analyser
typedef MapAnalyser<
	boost::unordered_map<
		UnicodeString,
		std::vector< std::pair<std::string, std::string> >,
		IcuHash
	>
> HashMapAnalyser;

/// typedef for a hashmap (std::unordered_map) analyser, caseless
typedef MapAnalyser<
	boost::unordered_map<
		UnicodeString,
		std::vector< std::pair<std::string, std::string> >,
		IcuStringCaselessEqual,
		IcuStringCaselessEqual
	>
> HashMapCaselessAnalyser;

/* implementation */

template<typename MapT>
MapAnalyser<MapT>::MapAnalyser(const Corpus2::Tagset* tagset)
	: MorphAnalyser(tagset), map_()
{
}

template<typename MapT>
MapAnalyser<MapT>::MapAnalyser(const Config::Node &cfg)
	: MorphAnalyser(cfg), map_()
{
	load_m_dictionary(cfg.get<std::string>("data"));
}

template<typename MapT>
MapAnalyser<MapT>* MapAnalyser<MapT>::clone() const
{
	MapAnalyser<MapT>* copy = new MapAnalyser<MapT>(&tagset());
	copy->map_ = map_;
	return copy;
}

template<typename MapT>
void MapAnalyser<MapT>::load_m_dictionary(const std::string &fn)
{
	std::ifstream ifs;
	Path::Instance().open_stream_or_throw(fn, ifs, "map analyser data");
	static const size_t BUFSIZE = 2000;
	char buf[BUFSIZE + 1];
	while (ifs.good()) {
		ifs.getline(buf, BUFSIZE);
		//std::vector< boost::iterator_range<const char*> > v;
		//std::string b(buf);
		size_t len = ifs.gcount();
		size_t i = 0;
		while (i < BUFSIZE && (buf[i] == ' ' || buf[i] == '\t')) {
			++i;
		}
		if (i + 1 < len) {
			std::vector< std::string > v;
			// do not include the trailing null
			string_range r(buf + i, buf + len - 1);
			boost::algorithm::split(v, r, boost::is_any_of("\t "),
					boost::algorithm::token_compress_on);
			if (v.size() == 3) {
				const UnicodeString& key = UnicodeString::fromUTF8(v[0]);
				map_[key].push_back(std::make_pair(v[1], v[2]));
			} else {
				std::cerr << "Invalid map line (" << v.size() << "): "
						<< buf << "\n";
			}
		}
	}
}

template<typename MapT>
bool MapAnalyser<MapT>::process_functional(const Toki::Token &t,
		boost::function<void (Corpus2::Token*)> sink)
{
	typename MapT::const_iterator i;
	const UnicodeString& key = t.orth();
	i = map_.find(key);
	if (i != map_.end()) {
		Corpus2::Token* tt = create_from_toki(t);
		typedef std::pair<std::string, std::string> sp;
		BOOST_FOREACH(const sp& o, i->second) {
			tagset().lexemes_into_token(*tt, o.first, o.second);
		}
		sink(tt);
		return true;
	} else {
		return false;
	}
}


} /* end ns Maca */

#endif // LIBMACA_MAPANALYSER_H
