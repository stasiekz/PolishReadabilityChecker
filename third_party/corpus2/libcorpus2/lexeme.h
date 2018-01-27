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

#ifndef LIBCORPUS2_LEXEME_H
#define LIBCORPUS2_LEXEME_H

#include <unicode/unistr.h>
#include <libcorpus2/tag.h>
#include <libpwrutils/util.h>

//#include <boost/flyweight.hpp>

// This is to allow boost hashed containers to work with ICU UnicodeStrings
// The function apparently needs to be in the same namespace as UnicodeString.
U_NAMESPACE_BEGIN
inline size_t hash_value(const UnicodeString& u)
{
	return u.hashCode();
}
U_NAMESPACE_END


namespace Corpus2 {

/**
 * A lexeme bundles together a dictionary form (lemma) and a
 * morphosyntactic tag (tag).
 */
class Lexeme
	: boost::equality_comparable<Lexeme>
	, boost::less_than_comparable<Lexeme>
{
public:
	/// Null lexeme creation
	Lexeme();

	/// Lemma and tag constructor
	Lexeme(const UnicodeString& lemma, const Tag& tag);

	/// Helper creation function
	static Lexeme create(const UnicodeString& lemma, const Tag& tag);

	/// Helper creation function, UTF-8 variant
	static Lexeme create_utf8(const std::string& lemma_utf8, const Tag& tag);

	/// Lemma accessor
	const UnicodeString& lemma() const {
		return lemma_;
	}

	/// Lemma setter
	void set_lemma(const UnicodeString& l) {
		lemma_ = l;
	}

	/// Lemma setter, UTF-8 variant
	void set_lemma_utf8(const std::string& lemma_utf8) {
		lemma_ = UnicodeString::fromUTF8(lemma_utf8);
	}

	/// UTF-8 lemma convenience accessor
	const std::string lemma_utf8() const {
		return PwrNlp::to_utf8(lemma_);
	}

	/// Tag accessor
	const Tag& tag() const {
		return tag_;
	}

	/// Tag setter
	void set_tag(const Tag& tag) {
		tag_ = tag;
	}

	/// Validity checking -- a lexeme is invalid (null) when it has
	/// a zero-length lemma or a tag with no tagset
	bool is_null() const;

	/// Disambiguated flag setter
	void set_disamb(bool value) {
		disamb_ = value;
	}

	/// Disambiguated flag getter
	bool is_disamb() const {
		return disamb_;
	}
	
	/// Convert lexeme's lemma to be entirely lower-case
	void lowercase_lemma() {
		lemma_.toLower();
	}

	/**
	 * Lexeme comparison. Lexemes are ordered by lemma and then by tag, see
	 * Tag::operator< . Boost template magic provides other comparison ops.
	 */
	bool operator<(const Lexeme& other) const;

	/**
	 * Lexeme equality. Lemma and tag must compare equal. Boost template
	 * magic provides operator!=.
	 */
	bool operator==(const Lexeme& other) const;

	/**
	 * Disamb-ignoring lexeme comparison
	 */
	struct DisamblessComparator
	{
		bool operator()(const Lexeme& l1, const Lexeme& l2) const;
	};

private:
	/// The lemma -- basic form
	//boost::flyweight<UnicodeString> lemma_;
	UnicodeString lemma_;

	/// The tag
	//boost::flyweight<Tag> tag_;
	Tag tag_;

	/// Compatibility "disambiguated" flag
	bool disamb_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_LEXEME_H
