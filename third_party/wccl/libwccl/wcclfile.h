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

#ifndef LIBWCCL_WCCLFILE_H
#define LIBWCCL_WCCLFILE_H

#include <libwccl/values/bool.h>
#include <libwccl/values/match.h>
#include <libwccl/values/position.h>
#include <libwccl/values/strset.h>
#include <libwccl/values/tset.h>
#include <libwccl/wcclfileopsections.h>
#include <libwccl/ops/tagrulesequence.h>
#include <libwccl/ops/matchrulesequence.h>
#include <libwccl/lexicon/lexicons.h>
#include <libwccl/exception.h>
#include <libpwrutils/pathsearch.h>

namespace Wccl {

/**
 * Class representing contents of parsed WCCL files
 * - imported lexicons, any typed and untyped named
 * operator sections, up to one tag rules section,
 * and up to one match rules section.
 */
class WcclFile
	: WcclFileOpSections<UntypedOpSequence>,
	  WcclFileOpSections<OpSequence<StrSet> >,
	  WcclFileOpSections<OpSequence<TSet> >,
	  WcclFileOpSections<OpSequence<Bool> >,
	  WcclFileOpSections<OpSequence<Position> >,
	  WcclFileOpSections<OpSequence<Match> >
{
public:
	WcclFile(const Corpus2::Tagset& tagset, const std::string& search_path);
	WcclFile(const boost::shared_ptr<const Corpus2::Tagset> tagset, const std::string& search_path);

	/////////////////////
	// Untyped and typed operator sections: @X:"sectioname" ( op1; op2 )
	/////////////////////

	/**
	 * @returns All untyped sections from the WCCL file: @"name" ( anyop0; ...; anyopN )
	 */
	const std::vector<boost::shared_ptr<UntypedOpSequence> >& untyped_sections();
	/**
	 * @returns All sections of given type T from the WCCL file: @T:"name" ( op<T>0; ...; op<T>N )
	 */
	template<class T>
	const typename std::vector<boost::shared_ptr<OpSequence<T> > >& sections();

	/**
	 * @returns True if the WCCL file contains untyped section of given name.
	 *          False otherwise.
	 */
	bool has_untyped_section(const std::string& name) const;
	/**
	 * @returns True if the WCCL file contains section of given name and type T.
	 *          False otherwise.
	 */
	template<class T>
	bool has_section(const std::string& name) const;

	/**
	 * @returns Names of all untyped sections in this WCCL file, in the order they
	 *			were found in the file.
	 */
	std::vector<std::string> untyped_section_names() const;
	/**
	 * @returns Names of all sections of given type T from this WCCL file, in the
	 *          order they were found in the file.
	 */
	template<class T>
	std::vector<std::string> section_names() const;

	/**
	 * @returns Untyped section of given name.
	 * @throws InvalidArgument if there is no untyped section of given name.
	 */
	UntypedOpSequence& get_untyped_section(const std::string& name);
	/**
	 * @returns Untyped section of given name (const).
	 * @throws InvalidArgument if there is no untyped section of given name.
	 */
	const UntypedOpSequence& get_untyped_section(const std::string& name) const;
	/**
	 * @returns Section of given name and type T.
	 * @throws InvalidArgument if there is no section of given name and type T.
	 */
	template<class T>
	OpSequence<T>& get_section(const std::string& name);
	/**
	 * @returns Section of given name and type T (const).
	 * @throws InvalidArgument if there is no section of given name and type T.
	 */
	template<class T>
	const OpSequence<T>& get_section(const std::string& name) const;
	/**
	 * @returns Shared pointer to an untyped section of the given name.
	 * @throws InvalidArgument if there is no untyped section of given name.
	 */
	boost::shared_ptr<UntypedOpSequence> get_untyped_section_ptr(const std::string& name);
	/**
	 * @returns Shared pointer to an untyped section of the given name (const).
	 * @throws InvalidArgument if there is no untyped section of given name.
	 */
	boost::shared_ptr<const UntypedOpSequence> get_untyped_section_ptr(const std::string& name) const;
	/**
	 * @returns Shared pointer to a section of the given name and type T.
	 * @throws InvalidArgument if there is no section of given name and type T.
	 */
	template<class T>
	boost::shared_ptr<OpSequence<T> > get_section_ptr(const std::string& name);
	/**
	 * @returns Shared pointer to a section of the given name and type T (const).
	 * @throws InvalidArgument if there is no section of given name and type T.
	 */
	template<class T>
	boost::shared_ptr<const OpSequence<T> > get_section_ptr(const std::string& name) const;
	/**
	 * @returns Operator from an untyped section of given name, present at given index.
	 * @note Index is zero-based and default is 0 (no idx given = the first operator in the sequence).
	 * @throws InvalidArgument if there is no untyped section of given name or if idx is out of range.
	 */
	FunctionalOperator& get_untyped_op(const std::string& name, size_t idx = 0);
	/**
	 * @returns Operator from an untyped section of given name, present at given index (const).
	 * @note Index is zero-based and default is 0 (no idx given = the first operator in the sequence).
	 * @throws InvalidArgument if there is no untyped section of given name or if idx is out of range.
	 */
	const FunctionalOperator& get_untyped_op(const std::string& name, size_t idx = 0) const;
	/**
	 * @returns Operator from a section of given name and type T, present at given index.
	 * @note Index is zero-based and default is 0 (no idx given = the first operator in the sequence).
	 * @throws InvalidArgument if there is no section of given name and type T or if idx is out of range.
	 */
	template<class T>
	Operator<T>& get_op(const std::string& name, size_t idx = 0);
	/**
	 * @returns Operator from a section of given name and type T, present at given index (const).
	 * @note Index is zero-based and default is 0 (no idx given = the first operator in the sequence).
	 * @throws InvalidArgument if there is no section of given name and type T or if idx is out of range.
	 */
	template<class T>
	const Operator<T>& get_op(const std::string& name, size_t idx = 0) const;
	/**
	 * @returns Shared pointer to an operator from an untyped section of given name, present at given index.
	 * @note Index is zero-based and default is 0 (no idx given = the first operator in the sequence).
	 * @throws InvalidArgument if there is no untyped section of given name or if idx is out of range.
	 */
	boost::shared_ptr<FunctionalOperator> get_untyped_op_ptr(const std::string& name, size_t idx = 0);
	/**
	 * @returns Shared pointer to an operator from an untyped section of given name, present at given index (const).
	 * @note Index is zero-based and default is 0 (no idx given = the first operator in the sequence).
	 * @throws InvalidArgument if there is no untyped section of given name or if idx is out of range.
	 */
	boost::shared_ptr<const FunctionalOperator> get_untyped_op_ptr(const std::string& name, size_t idx = 0) const;
	/**
	 * @returns Shared pointer to an operator from a section of given name and type T, present at given index.
	 * @note Index is zero-based and default is 0 (no idx given = the first operator in the sequence).
	 * @throws InvalidArgument if there is no section of given name and type T, or if idx is out of range.
	 */
	template<class T>
	boost::shared_ptr<Operator<T> > get_op_ptr(const std::string& name, size_t idx = 0);
	/**
	 * @returns Shared pointer to an operator from a section of given name and type T, present at given index (const).
	 * @note Index is zero-based and default is 0 (no idx given = the first operator in the sequence).
	 * @throws InvalidArgument if there is no section of given name and type T, or if idx is out of range.
	 */
	template<class T>
	boost::shared_ptr<const Operator<T> > get_op_ptr(const std::string& name, size_t idx = 0) const;

	/**
	 * @returns Vector of all name-operator pairs from untyped sections, in the order they were found in the file.
	 * @note Generated names of operators are derived from section name they are contained within,
	 *       suffixed with hyphen and zero-based index. E.g. the first operator in section named "seq"
	 *       will have name "seq-0", second operator's name will be "seq-1" etc.
	 */
	UntypedOpSequence::name_op_v_t gen_name_untyped_op_pairs();
	/**
	 * @returns Vector of all name-operator pairs from untyped sections, in the order they were found in the file (const).
	 * @note Generated names of operators are derived from section name they are contained within,
	 *       suffixed with hyphen and zero-based index. E.g. the first operator in section named "seq"
	 *       will have name "seq-0", second operator's name will be "seq-1" etc.
	 */
	UntypedOpSequence::name_op_v_c_t gen_name_untyped_op_pairs() const;
	/**
	 * @returns Vector of all name-operator pairs from sections of given type T, in the order they were found in the file.
	 * @note Generated names of operators are derived from section name they are contained within,
	 *       suffixed with hyphen and zero-based index. E.g. the first operator in section named "seq"
	 *       will have name "seq-0", second operator's name will be "seq-1" etc.
	 */
	template<class T>
	typename OpSequence<T>::name_op_v_t gen_name_op_pairs();
	/**
	 * @returns Vector of all name-operator pairs from sections of given type T, in the order they were found in the file (const).
	 * @note Generated names of operators are derived from section name they are contained within,
	 *       suffixed with hyphen and zero-based index. E.g. the first operator in section named "seq"
	 *       will have name "seq-0", second operator's name will be "seq-1" etc.
	 */
	template<class T>
	typename OpSequence<T>::name_op_v_c_t gen_name_op_pairs() const;

	/**
	 * @returns Vector of all name-operator pairs from all sections of the file (both typed and untyped),
	 *          in the order they were found in the file.
	 * @note Generated names of operators are derived from section name they are contained within,
	 *       suffixed with hyphen and zero-based index. E.g. the first operator in section named "seq"
	 *       will have name "seq-0", second operator's name will be "seq-1" etc.
	 */
	FunctionalOpSequence::name_op_v_t gen_all_op_pairs();
	/**
	 * @returns Vector of all name-operator pairs from all sections of the file (both typed and untyped),
	 *          in the order they were found in the file (const).
	 * @note Generated names of operators are derived from section name they are contained within,
	 *       suffixed with hyphen and zero-based index. E.g. the first operator in section named "seq"
	 *       will have name "seq-0", second operator's name will be "seq-1" etc.
	 */
	FunctionalOpSequence::name_op_v_c_t gen_all_op_pairs() const;

	
	///////////////////////////
	// Lexicons, import sections: import("path", "name")
	///////////////////////////

	/**
	 * @returns True if there is an imported Lexicon of given name. False otherwise.
	 */
	bool has_lexicon(const std::string& name) const;
	/**
	 * @returns True if there are any Lexicons imported.
	 */
	bool has_lexicons() const;
	/**
	 * @returns Shared pointer to a Lexicon of given name (const).
	 * @throws InvalidArgument if there is no Lexicon of given name imported.
	 */
	boost::shared_ptr<const Lexicon> get_lexicon_ptr(const std::string& name) const;
	/**
	 * @returns Lexicon of given name (const).
	 * @throws InvalidArgument if there is no Lexicon of given name imported.
	 */
	const Lexicon& get_lexicon(const std::string& name) const;
	/**
	 * @returns Shared pointer to collection of all imported Lexicons (const).
	 */
	boost::shared_ptr<const Lexicons> get_lexicons_ptr() const;
	/**
	 * @returns Collection of all imported Lexicons (const).
	 */
	const Lexicons& get_lexicons() const;
	
	///////////////////////////
	// Tag rules section: tag_rules ( rule1; rule2 )
	///////////////////////////

	/**
	 * @returns True if this WcclFile has tag_rules section. False otherwise.
	 */
	bool has_tag_rules() const;

	/**
	 * @returns The tag rules section from this WCCL file.
	 * @throws WcclError if there is no tag rules section.
	 */
	const TagRuleSequence& get_tag_rules() const;
	/**
	 * @returns The tag rules section from this WCCL file.
	 * @throws WcclError if there is no tag rules section.
	 */
	boost::shared_ptr<TagRuleSequence> get_tag_rules_ptr();
	/**
	 * @returns The tag rules section from this WCCL file (const).
	 * @throws WcclError if there is no tag rules section.
	 */
	boost::shared_ptr<const TagRuleSequence> get_tag_rules_ptr() const;

	///////////////////////////
	// Match rules section: match_rules ( apply1; apply2 )
	///////////////////////////

	/**
	 * @returns True if this WcclFile has match_rules section. False otherwise.
	 */
	bool has_match_rules() const;

	/**
	 * @returns The match rules section from this WCCL file.
	 * @throws WcclError if there is no match_rules section.
	 */
	const Matching::MatchRuleSequence& get_match_rules() const;
	/**
	 * @returns The match rules section from this WCCL file.
	 * @throws WcclError if there is no match_rules section.
	 */
	boost::shared_ptr<Matching::MatchRuleSequence> get_match_rules_ptr();
	/**
	 * @returns The match rules section from this WCCL file (const).
	 * @throws WcclError if there is no match_rules section.
	 */
	boost::shared_ptr<const Matching::MatchRuleSequence> get_match_rules_ptr() const;

	///////////////////////////
	// Miscelaneous
	///////////////////////////
	friend std::ostream& operator<<(std::ostream& ostream, const WcclFile& wccl_file);
	std::string to_string() const;

	const boost::shared_ptr<const Corpus2::Tagset> tagset() const;
	const PwrNlp::PathSearcher<Wccl::FileNotFound> path() const { return path_; }
	PwrNlp::PathSearcher<Wccl::FileNotFound> path() { return path_; }

	///////////////////////////
	// Internal parser-related methods to add sections.
	///////////////////////////
	void add_untyped_section(const boost::shared_ptr<UntypedOpSequence>& section);
	void add_untyped_section(const boost::shared_ptr<const UntypedOpSequence>& section);
	void add_untyped_section(const UntypedOpSequence& section);
	template<class T>
	void add_section(const boost::shared_ptr<OpSequence<T> >& section);
	template<class T>
	void add_section(const boost::shared_ptr<const OpSequence<T> >& section);
	template<class T>
	void add_section(const OpSequence<T>& section);

	void import_lexicon(const boost::shared_ptr<Lexicon>& lexicon);

	void set_tag_rules(const boost::shared_ptr<TagRuleSequence>& tag_rules);

	void set_match_rules(const boost::shared_ptr<Matching::MatchRuleSequence>& match_rules);

private:
	std::ostream& write_to(std::ostream& ostream) const;
	std::vector<boost::shared_ptr<FunctionalOpSequence> > all_sections_;
	boost::shared_ptr<TagRuleSequence> tag_rules_;
	boost::shared_ptr<Matching::MatchRuleSequence> match_rules_;
	boost::shared_ptr<Lexicons> lexicons_;
	const boost::shared_ptr<const Corpus2::Tagset> tagset_;
	PwrNlp::PathSearcher<Wccl::FileNotFound> path_;
};

} /* end ns Wccl */

//
// Implementation
//
namespace Wccl {

inline
WcclFile::WcclFile(const Corpus2::Tagset& tagset, const std::string& search_path)
  : lexicons_(boost::make_shared<Lexicons>()), tagset_(new Corpus2::Tagset(tagset)),
    path_(":")
{
	path_.set_search_path(search_path);
	path_.set_verbose(false);
}

inline
WcclFile::WcclFile(const boost::shared_ptr<const Corpus2::Tagset> tagset, const std::string& search_path)
  : lexicons_(boost::make_shared<Lexicons>()), tagset_(tagset),
    path_(":")
{
	path_.set_search_path(search_path);
	path_.set_verbose(false);
}

inline
const std::vector<boost::shared_ptr<UntypedOpSequence> >& WcclFile::untyped_sections()
{
	return WcclFileOpSections<UntypedOpSequence>::sections();
}

template<class T> inline
const typename std::vector<boost::shared_ptr<OpSequence<T> > >& WcclFile::sections()
{
	return WcclFileOpSections<OpSequence<T> >::sections();
}

inline
bool WcclFile::has_untyped_section(const std::string& name) const
{
	return WcclFileOpSections<UntypedOpSequence>::has_section(name);
}

template<class T> inline
bool WcclFile::has_section(const std::string &name) const
{
	return WcclFileOpSections<OpSequence<T> >::has_section(name);
}

inline
std::vector<std::string> WcclFile::untyped_section_names() const
{
	return WcclFileOpSections<UntypedOpSequence>::section_names();
}

template<class T> inline
std::vector<std::string> WcclFile::section_names() const
{
	return WcclFileOpSections<OpSequence<T> >::section_names();
}

inline
UntypedOpSequence& WcclFile::get_untyped_section(const std::string& name)
{
	return WcclFileOpSections<UntypedOpSequence>::get_section(name);
}

inline
const UntypedOpSequence& WcclFile::get_untyped_section(const std::string& name) const
{
	return WcclFileOpSections<UntypedOpSequence>::get_section(name);
}

template<class T> inline
OpSequence<T>& WcclFile::get_section(const std::string& name)
{
	return WcclFileOpSections<OpSequence<T> >::get_section(name);
}

template<class T> inline
const OpSequence<T>& WcclFile::get_section(const std::string& name) const
{
	return WcclFileOpSections<OpSequence<T> >::get_section(name);
}

inline
boost::shared_ptr<UntypedOpSequence> WcclFile::get_untyped_section_ptr(const std::string& name)
{
	return WcclFileOpSections<UntypedOpSequence>::get_section_ptr(name);
}
inline
boost::shared_ptr<const UntypedOpSequence> WcclFile::get_untyped_section_ptr(const std::string& name) const
{
	return WcclFileOpSections<UntypedOpSequence>::get_section_ptr(name);
}

template<class T> inline
typename boost::shared_ptr<OpSequence<T> > WcclFile::get_section_ptr(const std::string& name)
{
	return WcclFileOpSections<OpSequence<T> >::get_section_ptr(name);
}

template<class T> inline
typename boost::shared_ptr<const OpSequence<T> > WcclFile::get_section_ptr(const std::string& name) const
{
	return WcclFileOpSections<OpSequence<T> >::get_section_ptr(name);
}

inline
FunctionalOperator& WcclFile::get_untyped_op(const std::string &name, size_t idx)
{
	return WcclFileOpSections<UntypedOpSequence>::get_op(name, idx);
}

inline
const FunctionalOperator& WcclFile::get_untyped_op(const std::string& name, size_t idx) const
{
	return WcclFileOpSections<UntypedOpSequence>::get_op(name, idx);
}

template<class T> inline
Operator<T>& WcclFile::get_op(const std::string& name, size_t idx)
{
	return WcclFileOpSections<Operator<T> >::get_op(name, idx);
}

template<class T> inline
const Operator<T>& WcclFile::get_op(const std::string& name, size_t idx) const
{
	return WcclFileOpSections<Operator<T> >::get_op(name, idx);
}

inline
boost::shared_ptr<FunctionalOperator> WcclFile::get_untyped_op_ptr(
	const std::string& name,
	size_t idx)
{
	return WcclFileOpSections<UntypedOpSequence>::get_op_ptr(name, idx);
}

inline
boost::shared_ptr<const FunctionalOperator> WcclFile::get_untyped_op_ptr(
	const std::string& name,
	size_t idx) const
{
	return WcclFileOpSections<UntypedOpSequence>::get_op_ptr(name, idx);
}

template<class T> inline
boost::shared_ptr<Operator<T> > WcclFile::get_op_ptr(
	const std::string& name,
	size_t idx)
{
	return WcclFileOpSections<OpSequence<T> >::get_op_ptr(name, idx);
}

template<class T> inline
boost::shared_ptr<const Operator<T> > WcclFile::get_op_ptr(
	const std::string& name,
	size_t idx) const
{
	return WcclFileOpSections<OpSequence<T> >::get_op_ptr(name, idx);
}

inline
UntypedOpSequence::name_op_v_t WcclFile::gen_name_untyped_op_pairs()
{
	return WcclFileOpSections<UntypedOpSequence>::gen_name_op_pairs();
}

inline
UntypedOpSequence::name_op_v_c_t WcclFile::gen_name_untyped_op_pairs() const
{
	return WcclFileOpSections<UntypedOpSequence>::gen_name_op_pairs();
}

template<class T> inline
typename OpSequence<T>::name_op_v_t WcclFile::gen_name_op_pairs()
{
	return WcclFileOpSections<OpSequence<T> >::gen_name_op_pairs();
}

template<class T> inline
typename OpSequence<T>::name_op_v_c_t WcclFile::gen_name_op_pairs() const
{
	return WcclFileOpSections<OpSequence<T> >::gen_name_op_pairs();
}

inline
void WcclFile::import_lexicon(const boost::shared_ptr<Lexicon>& lexicon)
{
	lexicons_->insert(lexicon);
}

inline
bool WcclFile::has_lexicon(const std::string& name) const
{
	return lexicons_->has_lexicon(name);
}

inline
bool WcclFile::has_lexicons() const
{
	return static_cast<bool>(lexicons_);
}

inline
boost::shared_ptr<const Lexicon> WcclFile::get_lexicon_ptr(const std::string& name) const
{
	return lexicons_->get_ptr(name);
}

inline
const Lexicon& WcclFile::get_lexicon(const std::string &name) const
{
	return lexicons_->get(name);
}

inline
boost::shared_ptr<const Lexicons> WcclFile::get_lexicons_ptr() const
{
	return lexicons_;
}

inline
const Lexicons& WcclFile::get_lexicons() const
{
	return *lexicons_;
}

inline
void WcclFile::add_untyped_section(const boost::shared_ptr<UntypedOpSequence>& section)
{
	WcclFileOpSections<UntypedOpSequence>::append(section);
	all_sections_.push_back(section);
}

inline
void WcclFile::add_untyped_section(const boost::shared_ptr<const UntypedOpSequence>& section)
{
	boost::shared_ptr<UntypedOpSequence> s = section->clone();
	add_untyped_section(s);
}

inline
void WcclFile::add_untyped_section(const UntypedOpSequence& section)
{
	boost::shared_ptr<UntypedOpSequence> s = section.clone();
	add_untyped_section(s);
}

template<class T> inline
void WcclFile::add_section(const boost::shared_ptr<OpSequence<T> >& section)
{
	WcclFileOpSections<OpSequence<T> >::append(section);
	all_sections_.push_back(section);
}

template<class T> inline
void WcclFile::add_section(const boost::shared_ptr<const OpSequence<T> >& section)
{
	boost::shared_ptr<OpSequence<T> > s = section->clone();
	add_section(s);
}

template<class T> inline
void WcclFile::add_section(const OpSequence<T>& section)
{
	boost::shared_ptr<OpSequence<T> > s = section.clone();
	add_section(s);
}

inline
bool WcclFile::has_tag_rules() const
{
	return static_cast<bool>(tag_rules_);
}

inline
const TagRuleSequence& WcclFile::get_tag_rules() const
{
	return *get_tag_rules_ptr();
}

inline
void WcclFile::set_tag_rules(const boost::shared_ptr<TagRuleSequence>& tag_rules)
{
	if (has_tag_rules()) {
		throw WcclError("Tag rules already added.");
	}
	tag_rules_ = tag_rules;
}

inline
bool WcclFile::has_match_rules() const
{
	return static_cast<bool>(match_rules_);
}

inline
const Matching::MatchRuleSequence& WcclFile::get_match_rules() const
{
	return *get_match_rules_ptr();
}

inline
void WcclFile::set_match_rules(const boost::shared_ptr<Matching::MatchRuleSequence>& match_rules)
{
	if (has_match_rules()) {
		throw WcclError("Tag rules already added.");
	}
	match_rules_ = match_rules;
}

inline
std::ostream& operator <<(std::ostream& ostream, const WcclFile& wccl_file) {
	return wccl_file.write_to(ostream);
}

inline
const boost::shared_ptr<const Corpus2::Tagset> WcclFile::tagset() const {
	return tagset_;
}

} /* end ns Wccl */

#endif // LIBWCCL_WCCLFILE_H
