/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBTOKI_LEXICONCLASSIFYLAYER_H
#define LIBTOKI_LEXICONCLASSIFYLAYER_H

#include <boost/foreach.hpp>
#include <libtoki/layers/tokenlayer.h>
#include <libtoki/token.h>

#include <unicode/unistr.h>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include <fstream>
#include <set>

namespace Toki {

/**
 * Classification layer that changes a token type if the orth matches one
 * of the words in the given lexicon. The lexicon is a set of words.
 *
 * This layer is a templated class to allow various comparison functions in
 * the word set, mainly to allow case-insensitive lexicons. Two concrete
 * typedefs are provided:
 *  - CaseLexiconClassifyLayer, for case-sensitive lexicons
 *    -- "lexicon" key
 *  - CaselessLexiconClassifyLayer, for case-insensitive lexicons
 *    -- "lexicon_caseless" key.
 * @todo use a hash set
 */
template <typename CMP>
class LexiconClassifyLayer : public TokenLayer
{
public:
	/**
	 * Constructor.
	 *
	 * Keys recognized in the configuration:
	 * - lexicon      - The set of orths to recognize, space or comma
	 *                  separated.
	 * - token_type   - Token type to set in the recognized tokens.
	 * - lexicon_file - Path to lexicon file, with one word per line,
	 *                  UTF-8.
	 */
	LexiconClassifyLayer(TokenSource* input, const Config::Node& props);

	/// Destructor
	~LexiconClassifyLayer();

	/// TokenLayer override
	Token* process_token(Token *t);

	/// TokenLayer override
	virtual std::string info() const;

	/// TokenLayer override
	std::string long_info() const;

private:
	/// The set of orths to recognize
	std::set<UnicodeString, CMP> lex_;

	/// Token type to set in the recognized tokens
	std::string token_type_;
};


/* Implementation */

template<typename CMP>
LexiconClassifyLayer<CMP>::LexiconClassifyLayer(TokenSource *input,
		const Config::Node &props)
	: TokenLayer(input, props), lex_(), token_type_()
{
	token_type_ = props.get<std::string>("token_type", "lex");
	std::vector<std::string> v;
	std::string sep(", ");
	const std::string& sepstring = props.get<std::string>("lexicon", "");
	boost::algorithm::split(v, sepstring, boost::is_any_of(sep));
	BOOST_FOREACH (const std::string& s, v) {
		if (!s.empty()) {
			lex_.insert(UnicodeString::fromUTF8(s).unescape());
		}
	}
	const std::string& file_string = props.get<std::string>("lexicon_file",
			"");
	if (!file_string.empty()) {
		std::ifstream ifs;
		Path::Instance().open_stream_or_throw(file_string, ifs, "lexicon");
		while (ifs.good()) {
			std::string s;
			std::getline(ifs, s);
			if (!s.empty()) {
				lex_.insert(UnicodeString::fromUTF8(s).unescape());
			}
		}
	}
}

template<typename CMP>
LexiconClassifyLayer<CMP>::~LexiconClassifyLayer()
{
}

template<typename CMP>
std::string LexiconClassifyLayer<CMP>::info() const
{
	std::stringstream ss;
	ss << "lexicon{" << lex_.size() << "}";
	return ss.str();
}

template<typename CMP>
std::string LexiconClassifyLayer<CMP>::long_info() const
{
	std::stringstream ss;
	ss << ", lexicon: " << lex_.size() << " words";
	return TokenLayer::long_info() + ss.str();
}

template<typename CMP>
Token* LexiconClassifyLayer<CMP>::process_token(Token *t)
{
	if (lex_.find(t->orth()) != lex_.end()) {
		t->set_type(token_type_);
	}
	return t;
}

struct IcuStringCaselessCompare
{
public:
	bool operator()(const UnicodeString& u1, const UnicodeString& u2) {
		return u1.caseCompare(u2, 0) < 0;
	}
};

typedef LexiconClassifyLayer<
	std::less< UnicodeString >
> CaseLexiconClassifyLayer;

typedef LexiconClassifyLayer<
	IcuStringCaselessCompare
> CaselessLexiconClassifyLayer;


template<>
inline std::string CaselessLexiconClassifyLayer::info() const
{
	std::stringstream ss;
	ss << "lexicon_caseless{" << lex_.size() << "}";
	return ss.str();
}

template<>
inline std::string CaselessLexiconClassifyLayer::long_info() const
{
	std::stringstream ss;
	ss << ", lexicon_caseless: " << lex_.size() << " words";
	return TokenLayer::long_info() + ss.str();
}

} //end namespace Toki

#endif // LIBTOKI_LEXICONCLASSIFYLAYER_H
