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

#include <libcorpus2/io/fastxces.h>
#include <boost/make_shared.hpp>
#include <boost/regex.hpp>
#include <fstream>

namespace Corpus2 {

bool FastXcesReader::registered = TokenReader::register_reader<FastXcesReader>(
	"xces-fast","ign,loose,strict");

class FastXcesReaderImpl
{
public:
	FastXcesReaderImpl(const TokenReader& base_reader,
		std::deque< boost::shared_ptr<Chunk> >& obuf)
		: base_reader_(base_reader), obuf_(obuf)
	{
		tok_ = new Token();
		sent_ = boost::make_shared<Sentence>();
		chunk_ = boost::make_shared<Chunk>();
	}

	~FastXcesReaderImpl()
	{
		delete tok_;
	}

	void parse_stream(std::istream& is)
	{
		boost::regex parsing_regex = boost::regex(
				"(<ns\\>)|"
				"(?:<chunk[^>]*\\<type=[\"']?([a-zA-Z]*)[\"']?[^>]*>)|"
				"(?:<orth>\\s*(.*?)\\s*</orth>)|"
				"(?:<lex\\>([^>]*\\<disamb=[\"']?1[\"']?)?[^>]*>\\s*"
				"<base>\\s*(.*?)\\s*</base>\\s*<ctag>\\s*(.*?)\\s*</ctag>"
				"\\s*</lex>)|"
				"(</tok>)");
		enum {
			MATCH_NS = 1,
			MATCH_CHUNK_START = 2,
			MATCH_ORTH = 3,
			MATCH_DISAMB = 4,
			MATCH_BASE = 5,
			MATCH_CTAG = 6,
			MATCH_ETOK = 7
		};
		// This code is heavily based on example from Boost.Regex
		// (http://www.boost.org/doc/libs/1_41_0/libs/regex/doc/html/boost_regex/partial_matches.html)
		char buf[4096];
		const char* next_pos = buf + sizeof(buf);
		while (!is.eof()) {
			std::streamsize leftover = (buf + sizeof(buf)) - next_pos;
			std::streamsize size = next_pos - buf;
			memcpy(buf, next_pos, leftover);
			is.read(buf + leftover, size);
			std::streamsize read = is.gcount();
			next_pos = buf + sizeof(buf);

			boost::cregex_iterator i(buf, buf + read + leftover, parsing_regex,
					boost::match_default | boost::match_partial);
			boost::cregex_iterator end;
			for (; i != end; ++i) {
				if ((*i)[0].matched == false) {
					// Partial match, save position and break:
					next_pos = (*i)[0].first;
					break;
				}
				if ((*i)[MATCH_ORTH].matched) {
					orth(i->str(MATCH_ORTH));
				} else if ((*i)[MATCH_CTAG].matched) {
					ctag(i->str(MATCH_BASE),
						i->str(MATCH_CTAG),
						(*i)[MATCH_DISAMB].matched);
				} else if ((*i)[MATCH_ETOK].matched) {
					token_end();
				} else if ((*i)[MATCH_NS].matched) {
					no_space();
				} else if ((*i)[MATCH_CHUNK_START].matched) {
					chunk_start(i->str(MATCH_CHUNK_START));
				}
			}
		}

		// Finally close all remaining chunks.
		finish_all();
	}
private:
	const TokenReader& base_reader_;

	/// Token being constructed
	Token* tok_;

	/// Sentence being constructed
	Sentence::Ptr sent_;

	/// Chunk being constructed
	boost::shared_ptr<Chunk> chunk_;

	/// Output chunk buffer
	std::deque< boost::shared_ptr<Chunk> >& obuf_;

	void chunk_start(const std::string& type) {
		if (type == "s") {
			if (!sent_->empty()) {
				chunk_->append(sent_);
			}
			sent_ = base_reader_.make_sentence();
			tok_->set_wa(PwrNlp::Whitespace::Newline);
		} else {
			if (!chunk_->empty()) {
				obuf_.push_back(chunk_);
				chunk_ = boost::make_shared<Chunk>();
			}
			return;
		}
	}

	void finish_all() {
		if (!sent_->empty()) {
			chunk_->append(sent_);
			sent_ = boost::make_shared<Sentence>();
		}
		if (!chunk_->empty()) {
			obuf_.push_back(chunk_);
			chunk_ = boost::make_shared<Chunk>();
		}
	}

	void no_space() {
		tok_->set_wa(PwrNlp::Whitespace::None);
	}

	void orth(const std::string& orth) {
		tok_->set_orth(UnicodeString::fromUTF8(orth));
	}

	void ctag(const std::string& base, const std::string& ctag, bool disamb) {
		Tag tag = base_reader_.parse_tag(ctag);
		Lexeme lex(UnicodeString::fromUTF8(base), tag);
		lex.set_disamb(disamb);
		tok_->add_lexeme(lex);
	}

	void token_end() {
		sent_->append(tok_);
		tok_ = new Token();
		tok_->set_wa(PwrNlp::Whitespace::Space);
	}
};

FastXcesReader::FastXcesReader(const Tagset &tagset, std::istream &is)
	: BufferedChunkReader(tagset),
	impl_(new FastXcesReaderImpl(*this, chunk_buf_))
{
	this->is_ = &is;
}

FastXcesReader::~FastXcesReader()
{
}


void FastXcesReader::set_option(const std::string& /*option*/)
{
}

std::string FastXcesReader::get_option(const std::string& option) const
{
	return BufferedChunkReader::get_option(option);
}

FastXcesReader::FastXcesReader(const Tagset &tagset, const std::string &filename)
	: BufferedChunkReader(tagset),
	impl_(new FastXcesReaderImpl(*this, chunk_buf_))
{
	this->is_owned_.reset(new std::ifstream(filename.c_str(), std::ifstream::in));

	if (!this->is_owned_->good()) {
		throw FileNotFound(filename);
	}
	else {
		this->is_ = is_owned_.get();
	}
}

void FastXcesReader::ensure_more()
{
	if (is_->good()) {
		impl_->parse_stream(*is_);
	}
}



} /* end ns Corpus2 */
