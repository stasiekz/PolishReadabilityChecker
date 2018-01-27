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

#ifndef LIBTOKI_SRX_DOCUMENT_H
#define LIBTOKI_SRX_DOCUMENT_H


#include <libtoki/srx/exception.h>
#include <libtoki/srx/rule.h>

#include <boost/utility.hpp>

#include <istream>
#include <map>
#include <string>
#include <vector>

// forward decl
namespace xmlpp {
	class Node;
}

namespace Toki {
namespace Srx {

/**
 * Exception class for Srx parse errors
 */
class ParseError : public SrxError
{
public:
	ParseError(const std::string& what)
		: SrxError("Parse error: " + what)
	{
	}

	~ParseError() throw()
	{
	}
};

/**
 * A class to parse and hold a SRX document
 * Language rules are processed, but not everything from the SRX standard
 * is. In particular, the header format tags are ignored.
 */
class Document : private boost::noncopyable
{
public:
	/**
	 * Constructor. Creates an empty document.
	 */
	Document();

	/// Destructor
	~Document();

	/**
	 * Load a SRX document from the given stream.
	 * May throw Srx:::ParseError if the document id not valid.
	 */
	void load(std::istream &is);

	/**
	 * Get all the rules in the document, regardless of language
	 */
	std::vector<Rule> get_all_rules() const;

	/**
	 * Get the rules for a given language code, according to the rules
	 * in the SRX document.
	 */
	std::vector<Rule> get_rules_for_lang(const std::string& lang) const;

	/**
	 * Get a human-readable summary about the loaded SRX
	 */
	std::string info() const;

private:
	/// internal helper for processing the header
	void process_header_node(const xmlpp::Node* n);

	/// internal helper for processing the languagerule node
	void process_languagerule_node(const xmlpp::Node* n);

	/// internal helper for processing a single rule
	void process_rule_node(const std::string& language,
			const xmlpp::Node* n);

	/// internal helper for processing the language map
	void process_languagemap_node(const xmlpp::Node* n);

	/// typedef for the rules storage
	typedef std::map<std::string, std::vector<Rule> > language_rules_t;

	/// the available rules
	language_rules_t language_rules_;

	/// typedef for the language mappings
	typedef std::vector< std::pair<RegexMatcher*, std::string> >
			language_map_t;

	/// the language mappings
	language_map_t language_map_;


	/// the SRX "cascade" parameter -- if true, return all rules matching
	/// a given language instead of just the first group.
	bool cascade_;
};

} /* end ns Srx */
} /* end ns Toki */

#endif // LIBTOKI_SRX_DOCUMENT_H
