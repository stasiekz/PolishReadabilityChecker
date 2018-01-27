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

#include <libcorpus2/io/xmlreader.h>
#include <boost/foreach.hpp>
#include <libxml++/libxml++.h>
#include <libxml2/libxml/parser.h>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>

namespace Corpus2 {


XmlReader::XmlReader(const TokenReader& base_reader,
		std::deque< boost::shared_ptr<Chunk> >& obuf)
	: BasicSaxParser()
	, base_reader_(base_reader), state_(STATE_NONE)
	, chunkless_(false), out_of_chunk_(false)
	, wa_(PwrNlp::Whitespace::Newline)
	, sbuf_(), tok_(NULL), sent_(), chunk_(), obuf_(obuf)
	, disamb_only_(false), disamb_sh_(false)
	, warn_on_inconsistent_(true), warn_on_unexpected_(true)
{
}

XmlReader::~XmlReader()
{
	delete tok_;
}

std::string XmlReader::get_type_from_attributes(const AttributeList& attributes) const
{
	std::string type;
	BOOST_FOREACH(const Attribute& a, attributes) {
		if (a.name == "type") {
			type = a.value;
		}
	}
	return type;
}

std::string XmlReader::get_id_from_attributes(const AttributeList& attributes) const
{
	BOOST_FOREACH(const Attribute& a, attributes) {
		if (a.name == "id") {
			return a.value;
		}
	}
	return "";
}


void XmlReader::on_start_element(const Glib::ustring &name,
		const AttributeList& attributes)
{
	//std::cerr << name << state_ << "\n";
	if (state_ == STATE_NONE && name == "chunk") {
		start_chunk(attributes);
	} else if (state_ == STATE_CHUNK && name == sentence_tag_name_) {
		start_sentence(attributes);
	} else if (state_ == STATE_SENTENCE && name == "tok") {
		start_token(attributes);
	} else if (state_ == STATE_TOK && name == "orth") {
		state_ = STATE_ORTH;
		grab_characters_ = true;
		clear_buf();
	} else if (state_ == STATE_TOK && name == "lex") {
		start_lexeme(attributes);
	} else if (state_ == STATE_LEX && name == "base") {
		state_ = STATE_LEMMA;
		grab_characters_ = true;
		clear_buf();
	} else if (state_ == STATE_LEX && name == "ctag") {
		state_ = STATE_TAG;
		grab_characters_ = true;
		clear_buf();
	} else if (state_ == STATE_LEX_SKIP &&
		(name == "lex" || name == "base" || name == "ctag")) {
		//nop
	} else if (name == "ns") {
		wa_ = PwrNlp::Whitespace::None;
	} else if (state_ == STATE_NONE && name == "tok") {
		if (warn_on_inconsistent_) {
			std::cerr << "Warning: out-of-chunk token, assuming sentence start on line ";
			std::cerr << this->context_->input->line << "\n";
		}
		AttributeList fake;
		start_chunk(fake);
		fake.push_back(Attribute("type", "s"));
		start_sentence(fake);
		chunkless_ = true;
		out_of_chunk_ = true;
		start_token(attributes);
	} else if (state_ == STATE_NONE && name == "cesAna") {
		//nop
	} else if (state_ == STATE_NONE && name == "chunkList") {
		//nop
	} else if (process_start_element(name, attributes)) {
		//nop
	} else if (out_of_chunk_ && state_ == STATE_SENTENCE && name == "chunk") {
		finish_sentence();
		out_of_chunk_ = false;
		start_chunk(attributes);
	} else if (warn_on_unexpected_) {
		std::cerr << "Unexpected tag <" << name << "> on line ";
		std::cerr << this->context_->input->line << " (" << state_ << ")\n";
	}
}

bool XmlReader::process_start_element(const Glib::ustring &/*name*/,
	const AttributeList &/*attributes*/)
{
	return false;
}

bool XmlReader::process_end_element(const Glib::ustring & /*name*/)
{
	return false;
}


void XmlReader::start_chunk(const AttributeList& attributes)
{
	std::string type = get_type_from_attributes(attributes);
	chunk_ = boost::make_shared<Chunk>();
	if (type == "s") {
		// top-level chunk is a sentence
		start_sentence(attributes);
		chunkless_ = true;
	} else {
		BOOST_FOREACH(const Attribute& a, attributes) {
			chunk_->set_attribute(a.name, a.value);
		}
		state_ = STATE_CHUNK;
	}
}

void XmlReader::start_sentence(const AttributeList &attributes)
{
	std::string type = get_type_from_attributes(attributes);
	if (type != "s") {
		throw XcesError("Sub level <chunk> not type=\"s\"");
	}
	sent_ = base_reader_.make_sentence();
	state_ = STATE_SENTENCE;
}

void XmlReader::start_token(const AttributeList &/*attributes*/)
{
	state_ = STATE_TOK;
	tok_ = new Token();
	tok_->set_wa(wa_);
	wa_ = PwrNlp::Whitespace::Space;
}

void XmlReader::start_lexeme(const AttributeList &attributes)
{
	assert(tok_ != NULL);
	bool is_disamb = false;
	if (!disamb_sh_) {
		BOOST_FOREACH(const Attribute& a, attributes) {
			if (a.name == "disamb" && a.value == "1") {
				is_disamb = true;
			}
		}
	} else {
		is_disamb = true;
		BOOST_FOREACH(const Attribute& a, attributes) {
			if (a.name == "disamb_sh" && a.value == "0") {
				is_disamb = false;
			}
		}
	}
	if (!disamb_only_ || is_disamb) {
		tok_->add_lexeme(Lexeme());
		tok_->lexemes().back().set_disamb(is_disamb);
		state_ = STATE_LEX;
	} else {
		state_ = STATE_LEX_SKIP;
	}
}

void XmlReader::finish_chunk()
{
	assert(chunk_);
	obuf_.push_back(chunk_);
	chunk_.reset();
	state_ = STATE_NONE;
}

void XmlReader::finish_sentence()
{
	assert(chunk_);
	chunk_->append(sent_);
	sent_.reset();
	if (chunkless_) {
		obuf_.push_back(chunk_);
		chunk_.reset();
		state_ = STATE_NONE;
		chunkless_ = false;
	} else {
		state_ = STATE_CHUNK;
	}
}

void XmlReader::finish_token()
{
	assert(sent_);
	sent_->append(tok_);
	tok_ = NULL;
	state_ = STATE_SENTENCE;
}

void XmlReader::on_end_element(const Glib::ustring &name)
{
	//std::cerr << "/" << name << state_ << "\n";
	if (state_ == STATE_ORTH && name == "orth") {
		std::string tmp_buf = get_buf();
		boost::trim(tmp_buf);
		tok_->set_orth(UnicodeString::fromUTF8(tmp_buf));
		grab_characters_ = false;
		state_ = STATE_TOK;
	} else if (state_ == STATE_LEMMA && name == "base") {
		std::string tmp_buf = get_buf();
		boost::trim(tmp_buf);
		tok_->lexemes().back().set_lemma(UnicodeString::fromUTF8(tmp_buf));
		grab_characters_ = false;
		state_ = STATE_LEX;
	} else if (state_ == STATE_TAG && name == "ctag") {
		std::string tmp_buf = get_buf();
		boost::trim(tmp_buf);
		Tag tag = base_reader_.parse_tag(tmp_buf);
		tok_->lexemes().back().set_tag(tag);
		grab_characters_ = false;
		state_ = STATE_LEX;
	} else if ((state_ == STATE_LEX || state_ == STATE_LEX_SKIP) && name == "lex") {
		state_ = STATE_TOK;
	} else if (state_ == STATE_TOK && name == "tok") {
		finish_token();
	} else if (state_ == STATE_SENTENCE && name == sentence_tag_name_) {
		finish_sentence();
	} else if (state_ == STATE_CHUNK && name == "chunk") {
		finish_chunk();
	} else {
		process_end_element(name);
	}
}


} /* end ns Corpus2 */
