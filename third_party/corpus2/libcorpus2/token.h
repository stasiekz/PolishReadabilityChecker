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

#ifndef LIBCORPUS2_TOKEN_H
#define LIBCORPUS2_TOKEN_H

#include <libcorpus2/lexeme.h>
#include <libcorpus2/tagset.h>
#include <libcorpus2/tokenmetadata.h>

#include <libpwrutils/util.h>
#include <libpwrutils/whitespace.h>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <boost/iterator/filter_iterator.hpp>
#include <boost/function.hpp>

#include <unicode/unistr.h>
#include <memory>
#include <string>
#include <vector>

namespace Corpus2 {


/// Forward declaration of optional Token metadata class
//class TokenMetaData;

/**
 * A single token with morphological analyses.
 *
 * The orth is stored as an ICU UnicodeString, the token is also defined
 * by the preceeding whitespace measuere, and most importantly by the set
 * of possible interpretations stored as lexemes.
 */
class Token
	: boost::equality_comparable<Token>, boost::noncopyable
{
public:
	/// Creates an empty Token
	Token();

	/// Create a Token with the given orth and whitespace amount
	Token(const UnicodeString& orth, PwrNlp::Whitespace::Enum wa);

	/// Token creation, UTF-8
	static Token* create_utf8(const std::string& orth_utf8,
		PwrNlp::Whitespace::Enum wa = PwrNlp::Whitespace::Space);

	/// Create a duplicate Token
	Token* clone() const;

	/// Orth getter
	const UnicodeString& orth() const {
		return orth_;
	}

	/// Orth getter (UTF-8)
	std::string orth_utf8() const {
		return PwrNlp::to_utf8(orth_);
	}

	/// Orth setter
	void set_orth(const UnicodeString& orth) {
		orth_ = orth;
	}

	/// Orth setter (UTF-8)
	void set_orth_utf8(const std::string& orth_utf8) {
		orth_ = UnicodeString::fromUTF8(orth_utf8);
	}

	/// WA getter
	const PwrNlp::Whitespace::Enum& wa() const {
		return wa_;
	}

	/// WA setter
	void set_wa(const PwrNlp::Whitespace::Enum& wa) {
		wa_ = wa;
	}
	
	/// If any whitespace came before this token.
	bool after_space() const {
		return wa_ != PwrNlp::Whitespace::None;
	}

	/// Lexemes getter
	const std::vector<Lexeme>& lexemes() const {
		return lexemes_;
	}

	/// Lexemes nonconst getter
	std::vector<Lexeme>& lexemes() {
		return lexemes_;
	}

	/// Lexemes adder
	void add_lexeme(const Lexeme& lex) {
		lexemes_.push_back(lex);
	}

	/// Leaves a faulty token with no lexemes (use with care!)
	void remove_all_lexemes() {
		lexemes_.clear();
	}

	/// Replaces all the lexemes with the given vector
	void replace_lexemes(const std::vector<Lexeme>& new_lexemes) {
		lexemes_ = new_lexemes;
	}

	/**
	 * Get the 'best' lexeme available, with best meaning disambiguated if
	 * possible, and when there is more than one candidate, some form of
	 * sorting is performed to always return the same lexeme.
	 */
	const Lexeme& get_preferred_lexeme(const Tagset& tagset) const;

	/// Returns the index of the best lexeme, @see get_best_lexeme
	size_t get_preferred_lexeme_index(const Tagset& tagset) const;

	/// Clear all lexemes and add one lexeme with lemma=orth and an ign tag
	void make_ign(const Tagset& tagset);

	/// Add a lexeme with lemma=orth and an ign tag
	void add_ign(const Tagset& tagset);

	/// Equality operator (!= is provided by boost template magic)
	bool operator==(const Token& other) const;

	/**
	 * Check for duplicate lexemes.
	 *
	 * Returns true if there are duplicate lexemes, false otherwise.
	 */
	bool check_duplicate_lexemes() const;

	/**
	 * Remove duplicate lexemes.
	 *
	 * Removes any duplicate lexemes and returns true if there were any.
	 * Ordering of the lexemes can change.
	 */
	bool remove_duplicate_lexemes();

	/**
	 * Return true if there is at least one disambed lexeme in this token.
	 */
	bool has_disamb_lexeme() const;

	/**
	 * Return the count of disambed lexemes in this token.
	 */
	int count_disamb_lexemes() const;

	/// Helper iterator typedef
	typedef boost::filter_iterator<
		boost::function<bool (const Lexeme&)>,
		std::vector<Lexeme>::const_iterator
	> lexeme_filter_iterator;

	/**
	 * Helper to iterate through disamb-marked lexemes only (lexemes with
	 * the disamb flag false are skipped).
	 */
	std::pair<lexeme_filter_iterator, lexeme_filter_iterator> disamb_lexemes() const;

	/**
	 * Check if all the lexemes of the token have the given POS, and and
	 * the token orth caselessly matches the given orth.
	 *
	 * If the given orth is empty, it is not checked.
	 * If the given pos idx is -1, it is not checked.
	 *
	 * @returns true if the orth and lexemes pass the check, false
	 * otherwise
	 */
	bool orth_pos_match(mask_t pos, const UnicodeString& orth) const;

	/// Metadata setter
	void set_metadata(TokenMetaData& md) {
		// metadata_.reset(md);
		metadata_ = md.clone();
	}

	/// Metadata setter (sets metadata as shared_ptr)
	void set_metadata_ptr(boost::shared_ptr<TokenMetaData> md) {
		metadata_ = md;
	}

	/// Metadata getter
	boost::shared_ptr<TokenMetaData> get_metadata() const {
        return metadata_;
	}

    /// Metadata check
    bool has_metadata() {
        return get_metadata() != 0;
    }

	/// Creates an empty metdata object for this Token
	void create_metadata();

private:
	/// The orth (actual encountered form)
	//boost::flyweight<UnicodeString> orth_;
	UnicodeString orth_;

	/// The preceeding whitespace amount marker
	PwrNlp::Whitespace::Enum wa_;

	/// The possible lexemes
	std::vector<Lexeme> lexemes_;

	/// Metadata
	// std::auto_ptr<TokenMetaData> metadata_;
	boost::shared_ptr<TokenMetaData> metadata_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_TOKEN_H
