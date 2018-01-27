/*
    Copyright (C) 2011 Adam Wardyński, Tomasz Śniatowski, Paweł Kędzia,
    Adam Radziszewski, Bartosz Broda
    Part of the WCCL project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include "mwereader.h"
#include "mweparser.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/unordered_set.hpp>


namespace Corpus2{

bool MWEReader::registered = TokenReader::register_path_reader<MWEReader>(
	"mwereader","inner,mwefile"); // TODO more help?

	MWEReader::MWEReader(const Tagset &tagset, const std::string &filename)
		: TokenReader(tagset), inner_filename_(filename)
	{
		mwes_counter=0;
	}

	MWEReader::MWEReader(const Tagset &tagset, const std::string &filename, TokenReaderPtr reader)
		: TokenReader(tagset), inner_filename_(filename)
	{
		mwes_counter=0;
		inner_reader_ = reader;
	}
	
	void MWEReader::setFile(const std::string &filename)
	{
		inner_filename_ = filename;
		reset();
	}

	MWEReader::~MWEReader()
	{
		// TODO implementataion
	}

	Token* MWEReader::get_next_token()
	{
		if(currentSentence->empty())
			currentSentence=get_next_sentence();

		std::vector<Token*> tokens = currentSentence->tokens();
		if(token_index<tokens.size())
		{
			return tokens.at(token_index++)->clone();
		}
		else
		{

			currentSentence=get_next_sentence();


			if(currentSentence==NULL)
			{
				return NULL;
			}
			tokens = currentSentence->tokens();
			token_index=0;
			return tokens.at(token_index++)->clone();
		}
	}

	Sentence::Ptr MWEReader::get_next_sentence()
	{
		currentSentence = inner_reader_->get_next_sentence();
		if(currentSentence==0)
			return currentSentence;
		return process_sentence(currentSentence);
	}

	Sentence::Ptr MWEReader::process_sentence(Corpus2::Sentence::Ptr sentence)
	{
		boost::unordered_set<std::string> available_bases;
		for (unsigned i = 0; i < sentence->size(); ++i)
			for (unsigned j = 0; j < sentence->at(i)->lexemes().size(); ++j)
				if (sentence->at(i)->lexemes()[j].is_disamb())
					available_bases.insert(sentence->at(i)->lexemes()[j].lemma_utf8());
		
		
		
		Wccl::SentenceContext sc(sentence);
		for (sc.goto_start(); sc.is_current_inside(); sc.advance())
		{
			Corpus2::Token *pToken = sc.current();
			std::vector<Lexeme>& lexemes = pToken->lexemes();
			if(lexemes.size() == 0)
				continue;

			BOOST_FOREACH (const Lexeme& lex, lexemes){
				if(lex.is_disamb()){
					std::string base = lex.lemma_utf8();
					const MWEIndex::luvec& potential = mwe_index_.get_potential_lu(base);
					BOOST_FOREACH (LexicalUnit::Ptr pLU, potential)
					{
						bool ok = true;
						BOOST_FOREACH (const std::string & base, pLU->get_potential_bases())
						{
							if (available_bases.find(base) == available_bases.end())
							{
								ok = false;
								break;
							}
						}
						
						if (ok)
						{
							std::set<int> positions;
							int head;
							bool is_here = pLU->IsHere(sc, positions, head);
							if(is_here)
								sc = clone_sentence_add_mwe(sc, head, positions, pLU->get_base());
						}
					}
				}
			}
		}
		return sc.get_sentence_ptr();
	}

	Wccl::SentenceContext MWEReader::clone_sentence_add_mwe(Wccl::SentenceContext sentence,
										  int head, const std::set<int>& all,
										  const std::string &new_base)
	{
		std::string new_orth = get_new_orth_utf8(sentence.get_sentence_ptr(), all);
		Sentence::Ptr new_sentence = boost::make_shared<Sentence>();
		
		Wccl::SentenceContext new_context(new_sentence);
		new_context.set_position(sentence.get_position());
		
		std::vector<Token*> &tokens = sentence.get_sentence_ptr()->tokens();
		
		for (int i = 0; i < (int)tokens.size(); i++)
		{
			if(i == head)
			{
				Corpus2::Token * t = tokens[i]->clone();
				t->set_orth_utf8(new_orth);
				
				BOOST_FOREACH (Lexeme& lex, t->lexemes())
					if(lex.is_disamb())
						lex.set_lemma_utf8(new_base);
				
				new_sentence->append(t);
			}
			else if( all.find(i) == all.end())
				new_sentence->append(tokens[i]->clone());
			
			else if (i < sentence.get_position())
				new_context.recede();
		}
		return new_context;
	}

	std::string MWEReader::get_new_orth_utf8(Corpus2::Sentence::Ptr sentence,
								  const std::set<int>& all)
	{
		std::string new_orth;
		std::vector<Token*> &tokens = sentence->tokens();
		BOOST_FOREACH (const int &pos, all){
			Token* tok = tokens [pos];
			new_orth += tok->orth_utf8() + " ";
		}
		new_orth.erase(new_orth.size()-1, 1);

		return new_orth;
	}

	boost::shared_ptr<Chunk> MWEReader::get_next_chunk()
	{
		currentChunk=inner_reader_->get_next_chunk();
		if(currentChunk == NULL)
			return currentChunk;

		 boost::shared_ptr<Chunk> new_chunk = boost::make_shared<Chunk>();

		 BOOST_FOREACH (Corpus2::Sentence::Ptr sentence, currentChunk->sentences())
			 new_chunk->append( process_sentence(sentence) );


		return new_chunk;
	}

	void MWEReader::set_option(const std::string& option)
	{
		if(boost::algorithm::starts_with(option, "inner:")) {
			inner_reader_type = option.substr(6);
			reset();
		}
		if(boost::algorithm::starts_with(option, "mwefile:")) {
			std::string mwefile = option.substr(8);
			boost::algorithm::trim(mwefile);
			if(boost::filesystem::exists(mwefile)){
				load_mwes(mwefile);
			}
			else
				throw std::runtime_error("File "+ mwefile + " does not exists");
		}
		if(boost::algorithm::starts_with(option, "mwefile-list:")) {
			std::string mwefile = option.substr(13);
			size_t found=mwefile.find(" ",0);
			while(found!=std::string::npos)
			{
				std::string file = mwefile.substr(0,found);
				if(boost::filesystem::exists(file))
					load_mwes(file);
				else
					throw std::runtime_error("File "+ mwefile +" does not exists");
				mwefile=mwefile.substr(found+1);
				found=mwefile.find(" ",0);
			}
			if(boost::filesystem::exists(mwefile))
				load_mwes(mwefile);
			else
				throw std::runtime_error("File "+ mwefile +" does not exists");
		}


		// TODO more MWE stuff
	}

	void MWEReader::validate()
	{
		
		if(inner_reader_ == NULL)
			throw Corpus2Error("Inner reader not initialised.");
		if(mwes_counter==0)
			throw Corpus2Error("MWE files were not loaded");
		// TODO MWE stuff
	}

	std::string MWEReader::get_option(const std::string& option) const
	{
		if(boost::algorithm::starts_with(option, "inner:")
			&& inner_reader_ != NULL)
			return option;
		// TODO options for MWE
		return inner_reader_->get_option(option);
	}

	void MWEReader::load_mwes(const std::string &filename)
	{
		MWEParser parser(mwe_index_);
		parser.parse_file(filename);
		mwes_counter++;
		if(parser.get_tagset().name() != tagset().name())
			throw std::runtime_error( "Tagset in mwe file does not match reader tagset!" );
	}
	
	void MWEReader::reset()
	{
		inner_reader_ = create_path_reader(inner_reader_type, this->tagset(),
										inner_filename_);
		token_index=0;
		currentSentence = boost::make_shared<Sentence>();
	}


}// ns Corpus2
