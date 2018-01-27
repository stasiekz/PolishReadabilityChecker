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
#include "pqreader.h"
#include "pqclient.h"

/*
extern "C" {
void pwrnlp_plugin_init()
{
	std::cerr << "PQINIT\n";
	Corpus2::TokenReader::register_path_reader<Corpus2::PoliqarpReader>(
		"poliqarp","token,chunk,sentence");
}
}
*/

namespace Corpus2 {

bool PoliqarpReader::registered = TokenReader::register_path_reader<PoliqarpReader>(
	"poliqarp","token,chunk,sentence");


PoliqarpReader::PoliqarpReader(const Tagset &tagset, const std::string &filename)
	: TokenReader(tagset), pq_(new PoliqarpClient(tagset, filename)),
        executed_(false), mode_(PQ_SENTENCES), getWholeSentence(false)
{
	pq_->compile_query("[]+ within s");
}

PoliqarpReader::~PoliqarpReader()
{
}

void PoliqarpReader::set_query(const std::string &query)
{
	pq_->compile_query(query);
	mode_ = PQ_MANUAL;
        executed_ = false;
}

void PoliqarpReader::execute()
{
	pq_->execute_query();
	executed_ = true;
}

Token* PoliqarpReader::get_next_token()
{
	if (!executed_) execute();
	return pq_->get_next_focus_token();
}

Sentence::Ptr PoliqarpReader::get_next_sentence()
{
        if (!executed_) execute();
        return pq_->get_next_match_sequence(getWholeSentence);
}

boost::shared_ptr<Chunk> PoliqarpReader::get_next_chunk()
{
	if (!executed_) execute();
	return pq_->get_next_document();
}

void PoliqarpReader::set_option(const std::string &option)
{
	if (option == "chunk") {
		pq_->compile_query("[]+ within p");
		mode_ = PQ_CHUNKS;
		executed_ = false;
	} else if (option == "token") {
		pq_->compile_query("[]+ within s");
		mode_ = PQ_SENTENCES;
		executed_ = false;
	} else if (option == "token") {
		pq_->compile_query("[]");
		mode_ = PQ_TOKENS;
		executed_ = false;
        } else if (option == "getWholeSentence") {
                setGetWholeSentence(true);
        }else {
		TokenReader::set_option(option);
	}
}

void PoliqarpReader::setGetWholeSentence(bool getWholeSentence_)
{
    getWholeSentence = getWholeSentence_;
}

std::string PoliqarpReader::get_option(const std::string& option) const
{
	return TokenReader::get_option(option);
}

} /* end ns Corpus2 */
