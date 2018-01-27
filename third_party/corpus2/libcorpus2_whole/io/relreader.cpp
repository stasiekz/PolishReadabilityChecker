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

#include <boost/foreach.hpp>
#include <libcorpus2/exception.h>
#include <libcorpus2_whole/io/relreader.h>

#include <fstream>
#include <boost/make_shared.hpp>

namespace Corpus2 {
namespace whole {

RelationReader::RelationReader(const std::string &rela_path)
	: rela_path_(rela_path)
{
	readed_ = false;
	in_relation_ = false;
	in_relations_ = false;
	in_from_direct_ = false;
	in_to_direct_ = false;

	file_.reset(new std::ifstream(rela_path.c_str(), std::ifstream::in));

	if (!file_->good()) {
		file_.reset();
		throw FileNotFound(rela_path);
	}
}

void RelationReader::read()
{
	std::istream* is = NULL;
	static const int BUFSIZE = 1024;

	if (!file_) {
		throw FileNotFound(rela_path_);
	}
	else {
		is = file_.get();
	}

	while (is->good()) {
		unsigned char buf[BUFSIZE + 1];
		is->read(reinterpret_cast<char*>(buf), BUFSIZE);

		parse_chunk_raw(buf, is->gcount());
		if (is->eof()) {
			finish_chunk_parsing();
		}
	}

	// mark that document has been readed
	readed_ = true;
}

void RelationReader::on_start_element(const Glib::ustring& name,
									  const AttributeList& attributes) {
	if (name == RELATIONS_TAG) {
		in_relations_ = true;
	}
	else if (in_relations_ && name == RELATION_TAG) {
		in_relation_ = true;
		parse_relation_name(attributes);
		parse_relation_set(attributes);
	}
	else if (in_relation_ && name == RELATION_DIRECT_FROM) {
		in_from_direct_ = true;
		parse_direction_from(attributes);
	}
	else if (in_relation_ && name == RELATION_DIRECT_TO) {
		in_to_direct_ = true;
		parse_direction_to(attributes);
	}
	else {
		//
	}
}

void RelationReader::on_end_element(const Glib::ustring& name) {
	int annotation_number = 99999999;

	if (name == RELATIONS_TAG) {
		in_relations_ = false;
	}
	else if (in_relations_ && name == RELATION_TAG) {
		in_relation_ = false;
		try {
			validate();
			add_current_relation();
		} catch (...) {
			throw;
		}
	}
	else if (in_from_direct_) {
		std::istringstream (ann_number_) >> annotation_number;
		rel_from_ = boost::make_shared<DirectionPoint>(
			rel_from_->sentence_id(),
			rel_from_->channel_name(),
			annotation_number
		);
		ann_number_ = "";
		in_from_direct_ = false;
	}
	else if (in_to_direct_) {
		std::istringstream (ann_number_) >> annotation_number;
		rel_to_ = boost::make_shared<DirectionPoint>(
			rel_to_->sentence_id(),
			rel_to_->channel_name(),
			annotation_number
		);
		ann_number_ = "";
		in_to_direct_ = false;
	}
	else {
		//
	}
}

void RelationReader::on_characters(const Glib::ustring &text)
{
	if (in_relation_) {
		ann_number_ += text.raw();
	}
}

void RelationReader::validate()
{
	if (rel_name_.empty()) {
		throw Corpus2Error("Relation name is empty!");
	}
	if (!rel_from_) {
		throw Corpus2Error("Cannot find \"from\" in relation " + rel_name_);
	}
	if (!rel_to_) {
		throw Corpus2Error("Cannot find \"to\" in relation " + rel_name_);
	}
	if (ann_number_.empty()) {
		throw Corpus2Error("Cannot find annotation number in relation " + rel_name_);
	}
}

void RelationReader::add_current_relation()
{
	boost::shared_ptr<Relation> relation;
	relation = boost::make_shared<Relation>(rel_name_, rel_set_, rel_from_, rel_to_);
	relations_.push_back(relation);
}

void RelationReader::parse_relation_name(const AttributeList& attributes)
{
	rel_name_ = get_attribute_value(attributes, RELATION_NAME);
	ann_number_ = "";
	rel_from_.reset();
	rel_to_.reset();
}

void RelationReader::parse_relation_set(const AttributeList& attributes)
{
	rel_set_ = get_attribute_value(attributes, RELATION_SET);
}

void RelationReader::parse_direction_from(const AttributeList& attributes)
{
	parse_direction(attributes, rel_from_);
}

void RelationReader::parse_direction_to(const AttributeList& attributes)
{
	parse_direction(attributes, rel_to_);
}

void RelationReader::parse_direction(const AttributeList& attributes,
	boost::shared_ptr<DirectionPoint>& direct)
{
	int annotation_number = 99999999;
	std::string sentence_id = get_attribute_value(attributes, RELATION_SENTENCE_ID);
	std::string channel_name = get_attribute_value(attributes, RELATION_CHANNEL_NAME);

	direct = boost::make_shared<DirectionPoint>(
			sentence_id, channel_name, annotation_number);
}

std::string RelationReader::get_attribute_value(
		const AttributeList& attributes, const std::string& name)
{
	BOOST_FOREACH(const Attribute& a, attributes) {
		if (a.name == name) {
			return a.value;
		}
	}
	return "";
}

} // whole ns
} // Corpus2 ns
