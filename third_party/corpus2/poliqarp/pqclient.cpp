/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.POLIQARP and COPYING files for more details.
*/
#include "pqclient.h"
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

extern "C" {
	void async_notify_new_results(void* session)
	{
	}
}

namespace Corpus2
{

PoliqarpClient::PoliqarpClient(const Tagset& tagset, const std::string path)
	: tagset_(tagset)
{
	query_compiled_ = false;
	poliqarp_error error = poliqarp_error_none;
	if (poliqarp_create("", &error) != 0) {
		throw Corpus2Error(poliqarp_error_message_get(&error));
	}
	progress_init(&progress_);
	count_so_far_ = 0;
	err_ = 0;
	if (poliqarp_open_corpus(&corpus_, path.c_str(), &progress_, &error) == -1) {
		throw Corpus2Error(poliqarp_error_message_get(&error));
	} else {
		poliqarp_create_match_buffer(&buffer_, 1000);
	}
	poliqarp_corpus_info cinfo;
	poliqarp_get_corpus_info(&corpus_, &cinfo);
	corpus_size_ = cinfo.num_segments;
	curr_chunk_doc_id_ = 0;
}

PoliqarpClient::~PoliqarpClient()
{
	poliqarp_close_corpus(&corpus_);
	poliqarp_destroy_match_buffer(&buffer_);
	if (query_compiled_) {
		poliqarp_destroy_query(&query_);
		query_compiled_ = false;
	}
	poliqarp_destroy();
};

void PoliqarpClient::compile_query(const std::string & q)
{
	count_so_far_ = 0;
	last_query_ = q;
	if (query_compiled_) {
		poliqarp_destroy_query(&query_);
		query_compiled_ = false;
	}
	poliqarp_error error = poliqarp_error_none;
	if (q.empty()) {
		throw Corpus2Error("EmptyQuery");
	} else if (poliqarp_create_query(&query_, q.c_str(), &corpus_,
			0, NULL, NULL, &error) == -1) {
		throw Corpus2Error(std::string("QueryFailed: ") + poliqarp_error_message_get(&error));
	} else {
		query_compiled_ = true;
	}
}

void PoliqarpClient::reset_query()
{
	compile_query(last_query_);
}

void PoliqarpClient::execute_query()
{
	if (query_compiled_) {
		poliqarp_forget(&buffer_);
		if (poliqarp_produce(&buffer_, 1000, &query_, &progress_,
				NULL, 0, 1000)) {
			throw Corpus2Error("query execution error");
		}
		if (poliqarp_get_match_buffer_info(&buffer_, &info_)) {
			throw Corpus2Error("buffer read error");
		}
		count_so_far_ += buffer_.used;
		buffer_pos_ = 0;
	} else {
		throw Corpus2Error("Query not compiled");
	}
}


bool PoliqarpClient::next_match(poliqarp_match& match)
{
	if (info_.used > 0) {
		if (buffer_pos_ < info_.used) {
			poliqarp_get_match(&buffer_, &match, buffer_pos_++);
			return true;
		} else if (info_.used == buffer_.capacity) {
			poliqarp_forget(&buffer_);
			execute_query();
			if (info_.used > 0) {
				poliqarp_get_match(&buffer_, &match, buffer_pos_++);
				return true;
			}
		}
	}
	return false;
}

Token* PoliqarpClient::get_next_focus_token()
{
	poliqarp_match match;
	if (next_match(match)) {
		return get_token(match.focus);
	} else {
		return NULL;
	}
};

Sentence::Ptr PoliqarpClient::get_next_match_sequence(bool getWholeSentence)
{
	poliqarp_match match;
	if (next_match(match)) {
                if(getWholeSentence) return get_token_range(match.withinStart, match.withinEnd);
                else return get_token_range(match.start, match.end);
	} else {
		return Sentence::Ptr();
	}
}

Token* PoliqarpClient::get_token(size_t pos)
{
	poliqarp_segment segment;
	poliqarp_segment_info info;
	poliqarp_interpretation_set set;
	poliqarp_interpretation_set_info sinfo;
	poliqarp_get_segment(&segment, &corpus_, pos);
	poliqarp_get_segment_info(&segment, &info);
	poliqarp_get_disambiguated_interpretations(&segment, &set);
	poliqarp_get_interpretation_set_info(&set, &sinfo);

	std::auto_ptr<Token> res(new Token());
	if (info.space_before) {
		res->set_wa(PwrNlp::Whitespace::Space);
	}
	res->set_orth_utf8(info.text);
	for (size_t i = 0; i < sinfo.size; i++) {
		poliqarp_interpretation interp;
		poliqarp_interpretation_info iinfo;
		poliqarp_get_interpretation(&set, &interp, i);
		poliqarp_get_interpretation_info(&interp, &iinfo);
		Tag tag = tagset_.parse_simple_tag(iinfo.tag);
		Lexeme lex = Lexeme(UnicodeString::fromUTF8(iinfo.base), tag);
		lex.set_disamb(interp.disamb);
		res->add_lexeme(lex);
	}
	return res.release();
}

boost::shared_ptr<Chunk> PoliqarpClient::get_next_document(bool get_metadata)
{
	poliqarp_match match;
	boost::shared_ptr<Chunk> chunk;
	if (next_match(match)) {
		chunk = boost::make_shared<Chunk>();
		size_t document_id = match.document;

		chunk->set_attribute("id", "ch" + boost::lexical_cast<std::string>(document_id));
		if(get_metadata) {
			std::map<std::string, std::string> meta = get_document_metadata(document_id);
			std::map<std::string, std::string>::iterator iter;
			for(iter = meta.begin();iter != meta.end();++iter)
				chunk->set_attribute(iter->first, iter->second);
		}

		chunk->append(get_token_range(match.start, match.end));
		while (next_match(match)) {
			if (match.document == document_id) {
				chunk->append(get_token_range(match.start, match.end));
			} else {
				buffer_pos_--;
				break;
			}
		}
	}
	return chunk;
}

std::map<std::string, std::string> PoliqarpClient::get_document_metadata(size_t document_id)
{
	std::map<std::string, std::string> ret;

	poliqarp_metadata_set document_metadata_set;
	poliqarp_get_metadata_set(&corpus_, document_id, &document_metadata_set);
	int metadata_set_size = poliqarp_metadata_count(&document_metadata_set);

	for(int i = 0;i < metadata_set_size;++i) {
		poliqarp_metadata meta; poliqarp_metadata_info meta_info;
		poliqarp_get_metadata(&document_metadata_set, i, &meta);
		poliqarp_get_metadata_info(&meta, &meta_info);

		ret[meta_info.key] = meta_info.value.text;
	}
	return ret;
}

Sentence::Ptr PoliqarpClient::get_token_range(size_t from, size_t to)
{
	Sentence::Ptr s = boost::make_shared<Sentence>();
	for (size_t j = from; j < to; j++) {
		s->append(get_token(j));
	}
	return s;
}

size_t PoliqarpClient::get_count_of_matches_so_far()
{
	return count_so_far_;
}

size_t PoliqarpClient::only_count_results()
{
	//countSoFar = 0;
	if (query_compiled_) {
		while (poliqarp_produce(&buffer_, 1000, &query_, &progress_, NULL, 0, 1000) &&
				poliqarp_get_match_buffer_info(&buffer_, &info_)==0 &&
				info_.used > 0) {
			count_so_far_ += info_.used;
			poliqarp_forget(&buffer_);
		}
	}
	return count_so_far_;
}

size_t PoliqarpClient::get_corpus_size() const
{
	return corpus_size_;
};

size_t PoliqarpClient::get_corpus_pos() const
{
	if (query_compiled_) {
		return query_.last_context.index;
	} else {
		return 0;
	}
};
}
