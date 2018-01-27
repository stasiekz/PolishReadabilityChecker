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

#include <libcorpus2/io/reader.h>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>
#include <libcorpus2/ann/annotatedsentence.h>
#include <sstream>
#include <libpwrutils/plugin.h>
#include <libcorpus2/util/settings.h>

namespace Corpus2 {

namespace detail {
/**
 * Declaration of the TokenWriter factory as a singleton Loki object
 * factory. The factory instance can be accessed as
 * TokenLayerFactory::Instance(). It is assumed that all derived classes
 * have the same constructor signature.
 */
typedef Loki::SingletonHolder<
	TokenReaderFactory,
	Loki::CreateUsingNew, // default, needed to change the item below
	Loki::LongevityLifetime::DieAsSmallObjectChild // per libloki docs
>
TokenReaderFactorySingleton;

TokenReaderFactory& token_reader_factory()
{
	return TokenReaderFactorySingleton::Instance();
}
} /* ned ns detail */


TokenReader::TokenReader(const Tagset& tagset)
	: tagset_(tagset), tag_parse_mode_(Tagset::ParseDefault),
	use_annotated_sentences_(false)
{
	sent_number_ = 0;
	chunk_number_ = 0;
}

TokenReader::~TokenReader()
{
}

const std::string TokenReader::SENT_ID_PREFFIX = "s";
const std::string TokenReader::CHUNK_ID_PREFFIX = "ch";

void TokenReader::set_option(const std::string &option)
{
	if (option == "ign") {
		tag_parse_mode_ = static_cast<Tagset::ParseMode>(
			tag_parse_mode_ | Tagset::ParseFailWithIgn);
	} else if (option == "loose") {
		tag_parse_mode_ = static_cast<Tagset::ParseMode>(
			Tagset::ParseLoose | (tag_parse_mode_ & Tagset::ParseFailWithIgn));
	} else if (option == "strict") {
		tag_parse_mode_ = static_cast<Tagset::ParseMode>(
			Tagset::ParseDefault | (tag_parse_mode_ & Tagset::ParseFailWithIgn));
	} else if (option == "ann") {
		use_annotated_sentences_ = true;
	} else {
		throw Corpus2Error("Unknown option passed to reader: " + option);
	}
}

std::string TokenReader::get_option(const std::string &option) const
{
	if (option == "ign") {
		return tag_parse_mode_ & Tagset::ParseFailWithIgn ? option : "";
	} else if (option == "loose") {
		return (tag_parse_mode_ & ~Tagset::ParseFailWithIgn)
			== Tagset::ParseLoose ? option : "";
	} else if (option == "strict") {
		return (tag_parse_mode_ & ~Tagset::ParseFailWithIgn)
			== Tagset::ParseDefault ? option : "";
	} else if (option == "ann") {
		return use_annotated_sentences_ ? option : "";
	} else {
		return "unknown";
	}
}

boost::shared_ptr<Sentence> TokenReader::make_sentence() const
{
	if (use_annotated_sentences_) {
		return boost::make_shared<AnnotatedSentence>();
	} else {
		return boost::make_shared<Sentence>();
	}
}


bool TokenReader::name_sent(boost::shared_ptr<Sentence> sent)
{
	if (!sent) {
		return false;
	}
	if (sent->id().empty()) {
		std::stringstream id_string;
		id_string << SENT_ID_PREFFIX << (++sent_number_);
		sent->set_id(id_string.str());
		return true;
	}
	return false;
}

bool TokenReader::name_chunk(boost::shared_ptr<Chunk> chunk)
{
	if (!chunk) {
		return false;
	}
	if (chunk->has_attribute("id") && !chunk->get_attribute("id").empty()) {
		// already a non-empty id
		return false;
	}
	else {
		std::stringstream id_string;
		id_string << CHUNK_ID_PREFFIX << (++chunk_number_);
		chunk->set_attribute("id", id_string.str());
		return true;
	}
}

namespace {
std::string guess_plugin_name(const std::string& reader_class_id, int idx)
{
	switch (idx) {
		case 0: return reader_class_id + "reader";
		case 1: return reader_class_id;
		default: return "";
	}
}
}

boost::shared_ptr<TokenReader> TokenReader::create_path_reader(
	const std::string& class_id_params,
	const Tagset& tagset,
	const std::string& path)
{
	string_range_vector params;
	boost::algorithm::split(params, class_id_params,
							boost::is_any_of(std::string(",")));
	std::string class_id = boost::copy_range<std::string>(params[0]);
	params.erase(params.begin(), params.begin() + 1);
	int plugin_name_idx = 0;
	while (plugin_name_idx >=0) {
		try {
			return boost::shared_ptr<TokenReader>(
				detail::TokenReaderFactorySingleton::Instance().
				path_factory.CreateObject(class_id, tagset, path, params));
		} catch (detail::TokenReaderFactoryException&) {
			std::string next_plugin = guess_plugin_name(class_id, plugin_name_idx);
			if (!next_plugin.empty()) {
				PwrNlp::Plugin::load("corpus2", next_plugin, !Path::Instance().get_verbose());
				plugin_name_idx++;
			} else {
				plugin_name_idx = -1;
			}
		}
	}
	throw Corpus2Error("Reader class not found: " + class_id);
}

boost::shared_ptr<TokenReader> TokenReader::create_stream_reader(
	const std::string& class_id_params,
	const Tagset& tagset,
	std::istream& stream)
{
	string_range_vector params;
	boost::algorithm::split(params, class_id_params,
							boost::is_any_of(std::string(",")));
	std::string class_id = boost::copy_range<std::string>(params[0]);
	params.erase(params.begin(), params.begin() + 1);
	int plugin_name_idx = 0;
	while (plugin_name_idx >=0) {
		try {
			return boost::shared_ptr<TokenReader>(
			detail::TokenReaderFactorySingleton::Instance()
			.stream_factory.CreateObject(class_id, tagset, stream, params));
		} catch (detail::TokenReaderFactoryException&) {
			std::string next_plugin = guess_plugin_name(class_id, plugin_name_idx);
			if (!next_plugin.empty()) {
				PwrNlp::Plugin::load("corpus2", next_plugin, !Path::Instance().get_verbose());
				plugin_name_idx++;
			} else {
				plugin_name_idx = -1;
			}
		}
	}
	std::vector<std::string> ids;
	ids = detail::TokenReaderFactorySingleton::Instance().path_factory.RegisteredIds();
	if (std::find(ids.begin(), ids.end(), class_id) == ids.end()) {
		throw Corpus2Error("Reader class not found: " + class_id);
	} else {
		throw Corpus2Error("This reader does not support stream mode: " + class_id);
	}
}

std::vector<std::string> TokenReader::available_reader_types()
{
	return detail::TokenReaderFactorySingleton::Instance().path_factory.RegisteredIds();
}

std::string TokenReader::reader_help(const std::string& class_id)
{
	std::map<std::string, std::string>::const_iterator c;
	c = detail::TokenReaderFactorySingleton::Instance().help.find(class_id);
	if (c != detail::TokenReaderFactorySingleton::Instance().help.end()) {
		return c->second;
	} else {
		return "";
	}
}

std::vector<std::string> TokenReader::available_reader_types_help()
{
	std::vector<std::string> v = available_reader_types();
	BOOST_FOREACH(std::string& id, v) {
		std::stringstream ss;
		std::map<std::string, std::string>::const_iterator c;
		c = detail::TokenReaderFactorySingleton::Instance().help.find(id);
		if (c != detail::TokenReaderFactorySingleton::Instance().help.end()) {
			ss << id << "[";
			ss << c->second;
			ss << "]";
		}
		id = ss.str();
	}
	return v;
}

BufferedChunkReader::BufferedChunkReader(const Tagset& tagset)
	: TokenReader(tagset)
{
}

BufferedChunkReader::~BufferedChunkReader()
{
	BOOST_FOREACH(Token* t, token_buf_) {
		delete t;
	}
}

Token* BufferedChunkReader::get_next_token()
{
	bool more = true;
	while (token_buf_.empty() && more) {
		ensure_more();
		Sentence::Ptr s = get_next_sentence();
		if (s) {
			std::copy(s->tokens().begin(), s->tokens().end(),
					std::back_inserter(token_buf_));
			s->release_tokens();
		} else {
			more = false;
		}
	}
	if (token_buf_.empty()) {
		return NULL;
	} else {
		Token* t = token_buf_.front();
		token_buf_.pop_front();
		return t;
	}
}

Sentence::Ptr BufferedChunkReader::get_next_sentence()
{
	bool more = true;
	while (sentence_buf_.empty() && more) {
		ensure_more();
		boost::shared_ptr<Chunk> c = get_next_chunk();
		if (c) {
			std::copy(c->sentences().begin(), c->sentences().end(),
					std::back_inserter(sentence_buf_));
		} else {
			more = false;
		}
	}
	if (sentence_buf_.empty()) {
		return Sentence::Ptr();
	} else {
		Sentence::Ptr s = sentence_buf_.front();
		sentence_buf_.pop_front();
		name_sent(s);
		return s;
	}
}

boost::shared_ptr<Chunk> BufferedChunkReader::get_next_chunk()
{
	ensure_more();
	if (chunk_buf_.empty()) {
		return boost::shared_ptr<Chunk>();
	} else {
		boost::shared_ptr<Chunk> t = chunk_buf_.front();
		chunk_buf_.pop_front();
		if (t) {
			name_chunk(t);
			BOOST_FOREACH(Sentence::Ptr s, t->sentences()) {
				name_sent(s);
			}
		}
		return t;
	}
}

BufferedSentenceReader::BufferedSentenceReader(const Tagset& tagset)
	: TokenReader(tagset), chunkify_(true)
	, sentence_buf_(), token_buf_()
{
}

Token* BufferedSentenceReader::get_next_token()
{
	bool more = true;
	while (token_buf_.empty() && more) {
		Sentence::Ptr s = get_next_sentence();
		if (s) {
			std::copy(s->tokens().begin(), s->tokens().end(),
				std::back_inserter(token_buf_));
			s->release_tokens();
		} else {
			more = false;
		}
	}
	if (token_buf_.empty()) {
		return NULL;
	} else {
		Token* t = token_buf_.front();
		token_buf_.pop_front();
		return t;
	}
}

Sentence::Ptr BufferedSentenceReader::get_next_sentence()
{
	if (sentence_buf_ != NULL) {
		Sentence::Ptr s = sentence_buf_;
		sentence_buf_.reset();
		name_sent(s);
		return s;
	} else {
		Sentence::Ptr s = actual_next_sentence();
		name_sent(s);
		return s;
	}
}

boost::shared_ptr<Chunk> BufferedSentenceReader::get_next_chunk()
{
	Sentence::Ptr s = get_next_sentence();
	if (!s) {
		return boost::shared_ptr<Chunk>();
	} else {
		boost::shared_ptr<Chunk> c = boost::make_shared<Chunk>();
		c->append(s);
		s = get_next_sentence();
		while (s && (!chunkify_ || s->first_token()->wa() !=
				PwrNlp::Whitespace::ManyNewlines)) {
			c->append(s);
			s = get_next_sentence();
		}
		if (s) {
			sentence_buf_ = s;
		}
		name_chunk(c);
		return c;
	}
}

} /* end ns Corpus2 */
