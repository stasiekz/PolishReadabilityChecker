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

#ifndef LIBCORPUS2_IO_XMLREADER_H
#define LIBCORPUS2_IO_XMLREADER_H

#include <libcorpus2/io/reader.h>
#include <libcorpus2/io/xces.h>
#include <libcorpus2/chunk.h>
#include <deque>
#include <boost/scoped_ptr.hpp>
#include <libcorpus2/io/sax.h>
#include <boost/foreach.hpp>

namespace Corpus2 {

/**
  * Helper class for reading xml, xces like
  */
class XmlReader : public BasicSaxParser
{
public:
	XmlReader(const TokenReader& base_reader,
		std::deque< boost::shared_ptr<Chunk> >& obuf);

	virtual ~XmlReader();

	bool get_disamb_only() const { return disamb_only_; }
	void set_disamb_only(bool v) { disamb_only_ = v; }

	bool get_disamb_sh() const { return disamb_sh_; }
	void set_disamb_sh(bool v) { disamb_sh_ = v; }

	bool get_warn_on_unexpected() const { return warn_on_unexpected_; }
	void set_warn_on_unexpected(bool v) { warn_on_unexpected_ = v; }

	bool get_warn_on_inconsistent() const { return warn_on_inconsistent_; }
	void set_warn_on_inconsistent(bool v) { warn_on_inconsistent_ = v; }

protected:
	std::string get_type_from_attributes(const AttributeList& attributes) const;
	std::string get_id_from_attributes(const AttributeList& attributes) const;

	void on_start_element(const Glib::ustring & name,
			const AttributeList& attributes);

	void on_end_element(const Glib::ustring & name);

	virtual bool process_start_element(const Glib::ustring & name,
			const AttributeList& attributes);

	virtual bool process_end_element(const Glib::ustring & name);

	virtual void start_chunk(const AttributeList& attributes);

	virtual void start_sentence(const AttributeList& attributes);

	virtual void start_token(const AttributeList& attributes);

	void start_lexeme(const AttributeList& attributes);

	virtual void finish_chunk();

	virtual void finish_sentence();

	virtual void finish_token();

	const TokenReader& base_reader_;

	static const int STATE_NONE = 0;
	static const int STATE_CHUNK = 1;
	static const int STATE_SENTENCE = 2;
	static const int STATE_TOK = 3;
	static const int STATE_ORTH = 4;
	static const int STATE_LEX = 5;
	static const int STATE_LEMMA = 6;
	static const int STATE_TAG = 7;
	static const int STATE_LEX_SKIP = 8;


	/// The state of the parser
	int state_;

	/// Flag signyfying there was a sentence outside of a chunk
	bool chunkless_;

	/// Flag signyfying there was a token outside of a sentence/chunk
	bool out_of_chunk_;

	/// Whitespace for the next token
	PwrNlp::Whitespace::Enum wa_;

	/// Character data buffer
	Glib::ustring sbuf_;

	/// Token being constructed
	Token* tok_;

	/// Sentence being constructed
	Sentence::Ptr sent_;

	/// Chunk being constructed
	boost::shared_ptr<Chunk> chunk_;

	/// Output chunk buffer
	std::deque< boost::shared_ptr<Chunk> >& obuf_;

	/// Flag to only read disamb tags
	bool disamb_only_;

	/// Read Pantera-like disamb_sh diamb tag markings
	bool disamb_sh_;

	/// Flag to control warning messages on state errors
	bool warn_on_inconsistent_;

	/// Floag to control warning messages on unknown XML tags
	bool warn_on_unexpected_;

	/// Tag name for sentence objects, customized in child class ctors
	std::string sentence_tag_name_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_XMLREADER_H
