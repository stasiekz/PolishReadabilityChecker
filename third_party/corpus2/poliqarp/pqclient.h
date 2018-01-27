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
#ifndef CORPUS2_POLIQARP_PQCLIENT_H
#define CORPUS2_POLIQARP_PQCLIENT_H

#include <map>
#include <boost/utility.hpp>

extern "C" {
	#define this this_
	#include <poliqarp.h>
	#undef this
	void async_notify_new_results(void* session);
}

#include <libcorpus2/chunk.h>

namespace Corpus2
{

class PoliqarpClient : boost::noncopyable
{
public:
	PoliqarpClient(const Tagset& tagset, const std::string path);
	~PoliqarpClient();
	void reload_corpus(const std::string& path);

	void compile_query(const std::string& q);
	void reset_query();
	void execute_query();

	int has_error();

	bool next_match(poliqarp_match& match);

	Token* get_next_focus_token();
        Sentence::Ptr get_next_match_sequence(bool getWholeSentence);
    /**
     * @arg get_metadata if set to @c true will retrieve available document
     * metadata from corpus and set them as attributes to the returned chunk.
     * Otherwise it will just set @c id attribute. Default: @c false
     * @note Retrieved metadata will be written when writing chunk in XML or
     * XCES format (but not CCL format).
     */
	boost::shared_ptr<Chunk> get_next_document(bool get_metadata = false);

	Token* get_token(size_t pos);
	Sentence::Ptr get_token_range(size_t from, size_t to);

	size_t get_count_of_matches_so_far();
	size_t only_count_results();
	size_t get_corpus_size() const;
	size_t get_corpus_pos() const;

private:
	std::map<std::string, std::string> get_document_metadata(size_t document_id);

	const Tagset& tagset_;
	boost::shared_ptr<Sentence> match_;
	boost::shared_ptr<Chunk> document_;
	size_t buffer_pos_;
	size_t count_so_far_;
	int err_;
	std::string last_error_;
	std::string last_query_;
	bool quiet_;
	char *corpusname_;
	char *querytext_;
	bool tags_context_;
	bool tags_match_;
	bool query_compiled_;
	size_t corpus_size_;
    size_t curr_chunk_doc_id_;
	struct poliqarp_corpus corpus_;
	struct poliqarp_query query_;
	struct poliqarp_match_buffer buffer_;
	struct poliqarp_match_buffer_info info_;
	progress_t progress_;
	void *exception_data_;
};

}

#endif /* CORPUS2_POLIQARP_PQCLIENT_H */
