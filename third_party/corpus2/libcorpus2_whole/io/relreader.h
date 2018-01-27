/*
	Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski, Paweł Kędzia
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

#ifndef LIBCORPUS2_WHOLE_RELREADER_H
#define LIBCORPUS2_WHOLE_RELREADER_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <libxml++/parsers/saxparser.h>

#include <libcorpus2_whole/relation.h>

#include <iostream>

namespace Corpus2 {
namespace whole {

	const static std::string RELATION_TAG = "rel";
	const static std::string RELATIONS_TAG = "relations";
	const static std::string RELATION_DIRECT_FROM = "from";
	const static std::string RELATION_DIRECT_TO = "to";

	const static std::string RELATION_NAME = "name";
	const static std::string RELATION_SET = "set";
	const static std::string RELATION_SENTENCE_ID = "sent";
	const static std::string RELATION_CHANNEL_NAME = "chan";

/**
 * A reader for realtion documents. Note that document is read into memory
 * before any processing may take place.
 */
class RelationReader : public xmlpp::SaxParser {
public:
	/**
	 * Reads a document with relations
	 * @param rela_path  path to file with relations
	 */
	RelationReader(const std::string &rela_path);

	/**
	 * Lazy relations accessor.
	 * If relations are not readed then read relations and returns list of them.
	 * @return List of readed relations
	 */
	const std::vector< boost::shared_ptr<Relation> >& relations() {
		if (!readed_) {
			read();
		}

		return relations_;
	}

protected:
	// implementations of sax parser method
	void on_start_element(const Glib::ustring& name,
						  const AttributeList& attributes);
	void on_end_element(const Glib::ustring& name);
	void on_characters(const Glib::ustring &text);

private:
	/// Reads the document. It use Glib parser (LibXML++ parser)
	void read();

	/// Validates relation
	void validate();

	//
	void parse_relation_name(const AttributeList& attributes);
	void parse_relation_set(const AttributeList& attributes);
	void parse_direction_from(const AttributeList& attributes);
	void parse_direction_to(const AttributeList& attributes);
	void parse_direction(const AttributeList& attributes,
						 boost::shared_ptr<DirectionPoint>& direct);

	/// Adds readed relation to relations list
	void add_current_relation();

	/**
	 * Gets atribute from list of attributes
	 * @param attributes List of the attributes
	 * @param name Name of attribute
	 * @return Attribute value or empty string if attribute name not found
	 */
	std::string get_attribute_value(const AttributeList& attributes,
									const std::string& name);

	// -------------------------------------------------------------------------
	/// List of the relations in given relation file
	std::vector< boost::shared_ptr<Relation> > relations_;

	/// Path to file with relations
	const std::string rela_path_;

	/// Markers:
	bool readed_;
	bool in_relation_;
	bool in_relations_;
	bool in_from_direct_;
	bool in_to_direct_;

	/// File pointer
	boost::scoped_ptr<std::istream> file_;

	// -------------------------------------------------------------------------
	// Temporary information of actual parsing relation
	std::string rel_name_;
	std::string rel_set_;
	std::string ann_number_;
	boost::shared_ptr<DirectionPoint> rel_from_;
	boost::shared_ptr<DirectionPoint> rel_to_;
};

} // whole ns
} // Corpus2 ns

#endif // LIBCORPUS2_WHOLE_RELREADER_H
