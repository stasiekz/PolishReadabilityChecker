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

#ifndef LIBCORPUS2_TAGSET_H
#define LIBCORPUS2_TAGSET_H

#include <libcorpus2/tag.h>

#include <set>
#include <map>
#include <vector>

#include <boost/range.hpp>
#include <boost/function.hpp>

#include <libcorpus2/exception.h>
#include <libcorpus2/util/symboldictionary.h>
#include <libcorpus2/typedefs.h>

#include <unicode/unistr.h>

namespace Corpus2 {

/**
 * Exception class for signalling tag parse errors
 */
class TagParseError : public Corpus2Error
{
public:
	/// Constructor
	TagParseError(const std::string &what, const std::string& val,
			const std::string& tag, const std::string& tagset);

	/// Destructor
	~TagParseError() throw() {}

	/// Error override
	std::string info() const;

	std::string val, tag, tagset;
};

/// Exception class for signalling tagset mismatches in various situations
class TagsetMismatch : public Corpus2Error
{
public:
	/// Constructor from Tagset objects
	TagsetMismatch(const std::string& where, const Tagset& expected,
			const Tagset& actual);

	/// Constructor from tagset ids
	TagsetMismatch(const std::string& where, tagset_idx_t expected,
			tagset_idx_t actual);

	/// Destructor
	~TagsetMismatch() throw() {}

	/// Error override
	std::string info() const;

	/// ids of the mismatching tagsets
	tagset_idx_t expected_id, actual_id;
};

/**
 * Convenience function to check if tagsets match and throw if not, with the
 * given "where" circumstance string
 */
void require_matching_tagsets(const Tagset& expected, const Tagset& actual,
		const std::string& where);

/**
 * Convenience function that calls the non-template require_matching_tagsets
 * but can be called with something that has a tagset() method or a tagset
 * object.
 */
template<class T>
void require_matching_tagsets(const T& expected, const T& actual,
		const std::string& where);

class TagsetParser;

class Token;

/**
 * A tagset is the set of possible tags, the Tagset class also contains the
 * necessary string--number mappings for POS, attribute and value names.
 *
 * Tagset objects are used for tag string parsing and to create tag strings
 * from tags. Created tags must be used with the same tagset object.
 *
 * Tagsets are usually loaded from ini-like files, but note that tagset
 * file loading is delegated to a separate class, TagsetParser.
 *
 * Tagsets are referred to by name and managed centrally by TagsetManager,
 * though it is possible to have tagset objects not using the manager.
 */
class Tagset
{
public:
	/// Empty tagset constructor
	Tagset();

	/**
	 * Tagset convenience creation function, parse a string as if it were the
	 * contents of a tagset ini file
	 */
	static Tagset from_data(const char*);

	/**
	 * Mode enum for tag parsing
	 */
	enum ParseMode {
		ParseCheckRequired = 1, /// Check for presence of required attributes
		ParseAllowExtra = 2, /// Allow extra attributes
		ParseCheckSingular = 4, /// Check tag singularity
		ParseFailWithIgn = 8, /// Parsing error results in an ign tag, not an exception

		ParseDefault = ParseCheckRequired, /// Default mode
		ParseRequiredWithExtra = ParseCheckRequired | ParseAllowExtra,
		ParseStrict = ParseCheckRequired | ParseCheckSingular,
		ParseLoose = ParseAllowExtra
	};

	/**
	 * Parse a single tagset symbol and return the correspondig (partial) tag.
	 *
	 * Pos and value names result in a single-bit-set tag, attribite names
	 * result in a tag with all values from that attribute set.
	 *
	 * The resulting tags will usually be invalid as standalone tags, so
	 * there is no validation performed.
	 *
	 * An invalid string will result in a TagParseError exception.
	 */
	Tag parse_symbol(const std::string& s) const;

	/**
	 * Parse a comma-separated list of tagset symbols, calling parse_symbol
	 * repeatedly. No validation is performed other than the validity of
	 * each individual symbol.
	 */
	Tag parse_symbol_string(const std::string& s) const;


	/**
	 * Tag parsing -- functional version, whole tag string.
	 *
	 * A simple wrapper for string split and a call to the split string
	 * version.
	 */
	void parse_tag(const string_range& s,
		boost::function<void (const Tag&)> sink,
		ParseMode mode = ParseDefault) const;

	/**
	 * Tag parsing -- functional version, whole tag string, char* overload.
	 * TODO explain why needed
	 *
	 * A simple wrapper for string split and a call to the split string
	 * version.
	 */
	void parse_tag(const char* c,
			boost::function<void (const Tag&)> sink,
			ParseMode mode = ParseDefault) const {
		parse_tag(std::string(c), sink, mode);
	}

	/**
	 * Tag parsing -- functional version, split tag string.
	 *
	 * This is the main tag parsing function, other variants end up calling
	 * this with some preprocessing and tweaks. Parses the tag string and
	 * calls the sink function with each of the resulting tag objects.
	 *
	 * May throw various tag parse errors.
	 *
	 * The tag string is assumed to be in colon-saprated format, with the
	 * POS name first, followed by attribute values.
	 * The tag string may contain special characters that make the parsing
	 * end up with more than one tag:
	 * - a dot (.) separates several possible values for one field
	 * - an underscore (_) indicates that all values for the attribute at
	 *   the underscore's position should be taken.
	 */
	void parse_tag(const string_range_vector& ts,
			boost::function<void (const Tag&)> sink,
			ParseMode mode = ParseDefault) const;

	/**
	 * Tag parsing -- plain version, whole string.
	 *
	 * A simple wrapper for string split and a call to the split string
	 * version.
	 */
	std::vector<Tag> parse_tag(const string_range& s,
			ParseMode mode = ParseDefault) const;

	/**
	 * Tag parsing -- plain version, whole string, char* overload.
	 *
	 * A simple wrapper for string split and a call to the split string
	 * version.
	 */
	std::vector<Tag> parse_tag(const char* c,
			ParseMode mode = ParseDefault) const {
		return parse_tag(std::string(c), mode);
	}

	/**
	 * Tag parsing -- plain version, split string.
	 *
	 * A wrapper to the functional version that sets up parameters so that
	 * the tags end up in a vector, which is then returned.
	 */
	std::vector<Tag> parse_tag(const string_range_vector& ts,
			ParseMode mode = ParseDefault) const;

	/**
	 * Simple tag parsing -- whole string version.
	 *
	 * A simple wrapper for string split and a call to the split string
	 * version.
	 */
	Tag parse_simple_tag(const string_range& s,
			ParseMode mode = ParseDefault) const;

	/**
	 * Simple tag parsing -- whole string version, char* overload.
	 *
	 * A simple wrapper for string split and a call to the split string
	 * version.
	 */
	Tag parse_simple_tag(const char* c,
			ParseMode mode = ParseDefault) const {
		return parse_simple_tag(std::string(c), mode);
	}

	/**
	 * Simple tag parsing -- split string version.
	 *
	 * The tag string must not contain any special characters that make the
	 * string result on more than one actual tag object (i.e. no dots,
	 * underscores or plus / pipe characters).
	 */
	Tag parse_simple_tag(const string_range_vector& ts,
			ParseMode mode = ParseDefault) const;

	/**
	 * Create a tag from the given POS and a (unsorted) vector of values.
	 *
	 * The POS is assumed to be valid in this tagset.
	 * The values are assumed to be valid in this tagset, but are checked
	 * for correctness with regards to the POS according to the mode.
	 */
	Tag make_tag(idx_t pos, mask_t values,
			ParseMode mode = ParseDefault) const;

	/**
	 * Convenience function for creating a 'ign' (ignored) tag within this
	 * tagset.
	 */
	Tag make_ign_tag() const;

	/**
	 * Check if the tag is valid, that is:
	 * * has a valid POS
	 * * all atributes are valid, with either no value or a valid value for
	 *   that attribute
	 * * all required attributes are set
	 * * no extra attrbutes are set, unless allow_extra is true
	 * @return true if the tag is valid, false otherwise
	 */
	bool validate_tag(const Tag& t, ParseMode mode = ParseDefault,
			std::ostream* os = NULL) const;

	/**
	 * Create the string representation of a tag.
	 *
	 * The output format is the same as the input format of the parsing
	 * functions. If the tag is valid, parsing the resulting string again
	 * should result in the exact same tag object (invalid tags will be
	 * output as strings anyway, but the string may trigger a parse error).
	 */
	std::string tag_to_string(const Tag& tag) const;

	/**
	 * Create the string representation of a tag with no extra attributes
	 * and optional and missing attributes with 0 values represented as
	 * attribute names.
	 *
	 * The output format is not currently valid for parsing if there are
	 * missing attributes.
	 */
	std::string tag_to_no_opt_string(const Tag &tag) const;

	/**
	 * Create and return a string representation of the symbols contained
	 * within a tag when treated as separate tagset symbols.
	 *
	 * There will be one string for each POS set in the tag, and enough symbols
	 * to cover all the values. If compress_tags is false, there will be one
	 * value name per value set. If compress_tags is true, in case there are
	 * attributes with all values setin the tag, the name of the attribiute
	 * will be used instead of separate names of the attribute's values.
	 */
	std::vector<std::string> tag_to_symbol_string_vector(const Tag& tag,
			bool compress_attribites = true) const;

	/**
	 * Return a comma-separated string representation of all symbols contained
	 * within a tag.
	 *
	 * @see tag_to_symbol_string_vector.
	 */
	std::string tag_to_symbol_string(const Tag& tag,
			bool compress_attribites = true) const;

	/**
	 * Compute the number of singular tags that can be represented by the given
	 * tag, with the following restrictions:
	 * - the tags must be sub-tags of the given tag
	 * - the tags must have a value for every attribute where the given tag has
	 *   a non-zero value
	 *
	 * @returns 0 if the tag is null, 1 if the tag is sigular, otherwise the
	 *          number of different singular tags conforming to the
	 *          restrictions above that can be constructed from the given tag.
	 */
	size_t tag_size(const Tag& tag) const;

	/**
	 * Check if a tag actually represents only one tag.
	 *
	 * A tag is singular if it:
	 *  - has exactly one POS bit set
	 *  - has at most one bit set in each attribute.
	 * Note that the tag might be invalid, this is not checked.
	 */
	bool tag_is_singular(const Tag& tag) const;

	/**
	 * Split a tag into a vector of singular tags. Validity is not checked.
	 *
	 * @see tag_size
	 *
	 * @returns a vector of tags, each of which is singular, with size equal
	 *          to tag_size called on the tag being split. Each returned tag
	 *          in the resulting vector is a sub-tag of the original tag, and
	 *          all not-empty attributes of the original tag are not empty in
	 *          the split tag as well.
	 */
	std::vector<Tag> split_tag(const Tag& tag) const;

	/**
	  * Creates a singular tag from this one, possibly being a "multi-tag".
	  * POS and required attribute sets are reduced to tagset-wise first
	  * values, while optional attributes are reduced to unspecified if
	  * multiple values given (left intact otherwise).
	  */
	Tag select_singular(const Tag& tag) const;

	/**
	  * Creates a copy of the given tag where any optional attribute with no
	  * value given is encoded as each possible value set. NOTE: this may
	  * an invalid tag (multiple values set for one attribute), yet it is
	  * convenient for some tagging scenarios to be able to retrieve
	  * unspecified attr value as a non-zero mask. This can always be decoded
	  * into a valid tag by using select_singular.
	  */
	Tag expand_optional_attrs(const Tag& tag) const;

	/// POS name <-> index dictionary getter
	const SymbolDictionary<idx_t>& pos_dictionary() const {
		return pos_dict_;
	}

	/// attribute name <-> index dictionary getter
	const SymbolDictionary<idx_t>& attribute_dictionary() const {
		return attribute_dict_;
	}

	/// POS name -> index mapping
	/// @returns -1 on invalid name
	idx_t get_pos_index(const string_range& pos) const;

	/// POS index -> name
	/// @returns empty string on invalid index
	const std::string& get_pos_name(idx_t pos) const;

	/// POS mask -> name
	/// @returns empty string on invalid index
	const std::string& get_pos_name(mask_t pos) const;

	/// POS name -> mask mapping
	/// @return null mask on invalid name
	mask_t get_pos_mask(const string_range& pos) const;

	/// POS index -> mask mapping
	/// @return null mask on invalid index
	mask_t get_pos_mask(idx_t pos) const;

	/// POS mask -> index mapping
	/// @return -1 on empty mask, unspecified in more tha one POS set
	idx_t get_pos_index(mask_t pos) const;


	/// Attribute name -> index mapping
	/// @returns -1 on invalid name
	idx_t get_attribute_index(const string_range& a) const;

	/// Attribute mask -> index mapping
	/// @returns -1 on invalid mask
	idx_t get_attribute_index(mask_t a) const;

	/// Attribute index -> name
	/// @returns empty string on invalid index
	const std::string& get_attribute_name(idx_t a) const;

	/// Attribute mask -> name
	/// @returns empty string on invalid mask
	const std::string& get_attribute_name(mask_t a) const;

	/// Value mask -> attribute index mapping.
	/// if the value mask contains values from more than one attribute,
	/// behavior is not well defined
	/// @return -1 on invalid mask
	idx_t get_value_attribute(mask_t v) const;

	/// Attribute index -> vector of valid value masks mapping
	/// @return empty vector on invalid index
	const std::vector<mask_t>& get_attribute_values(idx_t a) const;

	/// Attribute index -> combined value mask
	/// @return null mask on invalid index
	mask_t get_attribute_mask(idx_t a) const;

	/// Attribute name -> combined value mask
	/// @return null mask on invalid name
	mask_t get_attribute_mask(const string_range& a) const;


	/// Value name -> mask
	/// @returns null mask on invalid name
	mask_t get_value_mask(const std::string& v) const;

	/// Value mask -> name
	/// @returns empty string on invalid mask
	const std::string& get_value_name(mask_t v) const;


	/// Getter for the pos -> valid attributes (in order) mapping
	/// Valid attributes are both the required and optional attributes.
	/// Generally the optonal ones should be after the required ones.
	const std::vector<idx_t>& get_pos_attributes(idx_t pos) const;

	/// Getter for the pos -> valid attributes flag vector
	const std::vector<bool>& get_pos_attributes_flag(idx_t pos) const;

	/// Getter for the pos -> required attributes flag vector
	const std::vector<bool>& get_pos_required_attributes(idx_t pos) const;

	/// @returns true if the given pos _requires_ the given attribute
	bool pos_requires_attribute(idx_t pos, idx_t attribute) const;

	/// @returns true if the given pos _allows_ the given attribute
	bool pos_has_attribute(idx_t pos, idx_t attribute) const;

	/// Getter for a mask covering all valid values for a given pos
	mask_t get_pos_value_mask(idx_t pos) const;

	/// Getter for a mask covering all required attributes of a pos
	mask_t get_pos_required_mask(idx_t pos) const;

	/// The number of POSes in this tagset
	int pos_count() const;

	/// The number of attributes in this tagset
	int attribute_count() const;

	/// The number of values in this tagset
	int value_count() const;

	/**
	 * Tagset cardinality counter -- the number of different valid tags
	 * this tagset represents.
	 */
	size_t size() const;

	/**
	 * Tagset cardinality counter -- the number of all the different tags
	 * this tagset represents, both valid and invalid.
	 *
	 * Return type is double since this can easily become huge.
	 */
	double size_extra() const;

	/// Tagset ID accessor
	tagset_idx_t id() const {
		return id_;
	}

	/// Tagset name accessor
	std::string name() const {
		return name_;
	}

	/// Tagset name setter
	void set_name(const std::string& name) {
		name_ = name;
	}

	/// Return a pretty name/id string for the tagset
	std::string id_string() const;

	/// Return a pretty name/id string for the tagset and a tag
	std::string id_string(const Tag& tag) const;

	/**
	 * Convenience function for the case when there is a lemma and a
	 * possibly complex tag string and we want to add the resulting lexemes
	 * into a token by parsing the tag string and creating one lexeme for
	 * every separate tag, all with the same lemma.
	 *
	 * The new lexemes are appended, duplicates are not checked.
	 */
	void lexemes_into_token(Token& tok, const UnicodeString& lemma,
			const string_range& tags) const;

	/**
	 * Convenience overload
	 */
	void lexemes_into_token(Token& tok, const std::string& lemma,
			const string_range& tags) const {
		UnicodeString u = UnicodeString::fromUTF8(lemma);
		lexemes_into_token(tok, u, tags);
	}

	/// get the original index of the POS in the tagset definition
	int get_original_pos_index(idx_t pos) const;

	/// Helper iterator class for the mask ranges
	struct mask_iterator
	{
		typedef mask_t value_type;
		typedef std::forward_iterator_tag iterator_category;
		typedef int difference_type;
		typedef const mask_t *pointer;
		typedef const mask_t &reference;
		mask_iterator(const mask_iterator &i): i_(i.i_) {}
		mask_iterator(const mask_t& i) : i_(i) {}

		mask_iterator &operator++() { i_ <<= 1; return *this; }
		mask_iterator operator++(int) { return mask_iterator(i_ << 1); }
		mask_iterator &operator--() { i_ >>= 1; return *this; }
		mask_iterator operator--(int) { return mask_iterator(i_ >> 1); }

		const mask_t &operator*() const { return i_; }

		bool operator==(const mask_iterator &i) const { return i_ == i.i_; }
		bool operator!=(const mask_iterator &i) const { return i_ != i.i_; }

	private:
		mask_t i_;
	};

	/// Range getter for all the valid POS masks, in order, compatible with
	/// boost's foreach
	/// It is possible to use a BOOST_FOREACH(mask_t m, tagset.all_*_masks()) {...}
	boost::iterator_range<mask_iterator> all_pos_masks() const {
		return boost::iterator_range<mask_iterator>(static_cast<mask_t>(1),
				static_cast<mask_t>(1) << pos_count());
	}

	/// Range getter for all valid value masks
	boost::iterator_range<mask_iterator> all_value_masks() const {
		return boost::iterator_range<mask_iterator>(static_cast<mask_t>(1),
				static_cast<mask_t>(1) << value_count());
	}

	/// Getter attribute masks
	const std::vector<mask_t>& all_attribute_masks() const {
		return attribute_masks_;
	}


private:
	/// Temporary solution to allow splitting the parser into a separate
	/// class
	friend class TagsetParser;

	/// Tagset name
	std::string name_;

	/// Tagset ID
	tagset_idx_t id_;

	/// Next ID
	static tagset_idx_t next_id_;

	/// String - number dictionary for the POS names
	SymbolDictionary<idx_t> pos_dict_;

	/// String - number dictionary for the attribute names
	SymbolDictionary<idx_t> attribute_dict_;

	/// Value names to masks
	std::map<std::string, mask_t> string_to_value_mask_;

	/// Value masks to names
	std::map<mask_t, std::string> value_mask_to_string_;

	/// The original indices of the POSes in the tagset definition
	std::map<idx_t, int> original_pos_indices_;

	/// mapping from attribute indices to valid value masks
	std::vector< std::vector<mask_t> > attribute_values_;

	/// Attribute index to combined value mask
	std::vector<mask_t> attribute_masks_;

	/// Attribute combined mask to attribute name
	std::map<mask_t, idx_t> attribute_mask_to_index_;

	/// reverse mapping, from a value mask to the respective attribute
	/// index (values are assumed to be unique and not shared between
	/// attributes)
	std::map<mask_t, idx_t> value_mask_to_attribute_index_;

	/// POS to valid attribute indices mapping
	/// The order of the attributes is important, as it affects string
	/// output and the behavior of the _ special character in parsing
	std::vector< std::vector<idx_t> > pos_attributes_;

	/// POS to required attribute indices
	std::vector< std::vector<idx_t> > pos_required_attributes_idx_;

	/// POS to combined valid attriubute value mask
	std::vector<mask_t> pos_valid_value_masks_;

	/// POS to combined required attriubute value mask
	std::vector<mask_t> pos_required_value_masks_;

	/// Flags for attribute indices which are valid for a given POS
	std::vector< std::vector<bool> > pos_valid_attributes_;

	/// Flags for attribute indices which are required for a given POS
	std::vector< std::vector<bool> > pos_required_attributes_;

	/// Valid POS mask
	mask_t valid_pos_mask_;

	/// The ign tag
	Tag ign_tag_;
};

/* implementation */

template<class T, class U> inline
void require_matching_tagsets(const T& expected, const U& actual,
		const std::string& where)
{
	require_matching_tagsets(expected.tagset(), actual.tagset(), where);
}

template<class T> inline
void require_matching_tagsets(const T& expected, const Tagset& actual,
		const std::string& where)
{
	require_matching_tagsets(expected.tagset(), actual, where);
}

template<class T> inline
void require_matching_tagsets(const Tagset& expected, const T& actual,
		const std::string& where)
{
	require_matching_tagsets(expected, actual.tagset(), where);
}

inline
void require_matching_tagsets(const Tagset& expected, const Tagset& actual,
		const std::string& where)
{
	if (actual.id() != expected.id()) {
		throw TagsetMismatch(where, expected, actual);
	}
}

} /* end ns Corpus2 */

#endif // LIBCORPUS2_TAGSET_H
