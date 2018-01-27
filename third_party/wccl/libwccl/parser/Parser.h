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

#ifndef LIBWCCL_PARSER_PARSER_H
#define LIBWCCL_PARSER_PARSER_H

#include <iostream>

// ----------------------------------------------------------------------------
#include <sstream>
#include <fstream>
// ----------------------------------------------------------------------------
#include <libcorpus2/tagset.h>

// ----------------------------------------------------------------------------
#include <libwccl/variables.h>
#include <libwccl/values/strset.h>
#include <libwccl/ops/operator.h>

// rules
#include <libwccl/ops/tagrulesequence.h>

// match actions
#include <libwccl/ops/matchrule.h>

// match actions
#include <libwccl/wcclfile.h>

// exceptions
#include <libwccl/parser/ParserException.h>

// ----------------------------------------------------------------------------

namespace Wccl{

class Parser {
public:
	explicit Parser(const Corpus2::Tagset&);
	explicit Parser(const boost::shared_ptr<const Corpus2::Tagset>);
	explicit Parser(const std::string& tagset_name);
	~Parser();

	// ---------------------------------------------------------------------------
	// methods for parsing string operators
	boost::shared_ptr<Operator<StrSet> >
			parseStringOperator(const std::string& operator_string) const;
	boost::shared_ptr<Operator<StrSet> >
			parseStringOperator(std::istream& is) const;

	// ---------------------------------------------------------------------------
	// methods for parsing bool operators
	boost::shared_ptr<Operator<Bool> >
			parseBoolOperator(const std::string& operator_string) const;
	boost::shared_ptr<Operator<Bool> >
			parseBoolOperator(std::istream& is) const;

	// ---------------------------------------------------------------------------
	// methods for parsing bool operators
	boost::shared_ptr<Operator<TSet> >
			parseSymSetOperator(const std::string& operator_string) const;
	boost::shared_ptr<Operator<TSet> >
			parseSymSetOperator(std::istream& is) const;

	// ---------------------------------------------------------------------------
	// methods for parsing position operators
	boost::shared_ptr<Operator<Position> >
			parsePositionOperator(const std::string& operator_string) const;
	boost::shared_ptr<Operator<Position> >
			parsePositionOperator(std::istream& is) const;

	// ---------------------------------------------------------------------------
	// methods for parsing any operators
	boost::shared_ptr<FunctionalOperator>
			parseAnyOperator(const std::string& operator_string) const;
	boost::shared_ptr<FunctionalOperator>
			parseAnyOperator(std::istream& is) const;

	// ---------------------------------------------------------------------------
	// Parsing rule sequence from input string
	boost::shared_ptr<TagRuleSequence>
			parseTagRuleSequence(const std::string& rule_string) const;
	boost::shared_ptr<TagRuleSequence>
			parseTagRuleSequence(std::istream& is) const;

	// ---------------------------------------------------------------------------
	// Parsing single rule from input string
	boost::shared_ptr<TagRule>
			parseSingleRule(const std::string& rule_string) const;
	boost::shared_ptr<TagRule>
			parseSingleRule(std::istream& is) const;

	// ---------------------------------------------------------------------------
	// Parsing match rule from input string
	boost::shared_ptr<Matching::MatchRule>
			parseMatchRule(const std::string& rule_string) const;
	boost::shared_ptr<Matching::MatchRule>
			parseMatchRule(std::istream& is) const;

	// ---------------------------------------------------------------------------
	// Routines for parsing a whole WCCL file -- the RECOMMENDED way of parser
	// usage. The standard whole file syntax is used (see WCCL language specs).
	// This way different WCCL expressions (any-type functional operators as well
	// as tag/match rules) may be parsed in a uniform fashion.
	// This also allows to parse WCCL expressions that reference external
	// resources (lexicon files). Those resources are sought in the given
	// search_path.
	boost::shared_ptr<WcclFile>
			parseWcclFile(const std::string& file_contents_string,
				const std::string& search_path = ".") const;
	boost::shared_ptr<WcclFile>
			parseWcclFile(std::istream& is,
				const std::string& search_path = ".") const;
	boost::shared_ptr<WcclFile>
			parseWcclFileFromPath(const std::string& filename,
				const std::string& search_path = ".") const;

	// ---------------------------------------------------------------------------
	const boost::shared_ptr<const Corpus2::Tagset> tagset() const {
		return tagset_;
	}

private:
	const boost::shared_ptr<const Corpus2::Tagset> tagset_;
};

} // end Wccl ns

#endif // LIBWCCL_PARSER_PARSER_H
