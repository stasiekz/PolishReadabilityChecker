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

#ifndef LIBMWEREADER_MWEREADER_H
#define LIBMWEREADER_MWEREADER_H

#include <libcorpus2/io/reader.h>

#include "mwe.h"

namespace Corpus2 {


class MWEReader: public TokenReader
{
public:
	/**
	  * \param filename corpus filename (MWE file is given in options)
	  */
	MWEReader(const Tagset& tagset, const std::string & filename);
	MWEReader(const Tagset &tagset, const std::string & filename, TokenReaderPtr reader);

	~MWEReader();
	
	/// Allows reusage of the reader for multiple files. It is needed for it stores huge index of MWEs
	void setFile(const std::string & filename);

	/// retrieves whole sentence, finds MWEs, and return tokens
	Token* get_next_token();

	/// the prefered mode for this reader
	Sentence::Ptr get_next_sentence();

	/**
	  * retrieves chunk with inner reader and then searches for MWEs within
	  * sentences.
	  */
	boost::shared_ptr<Chunk> get_next_chunk();
	/**
	  * setting an "inner:..." option is equal to an immediate creation of an inner reader.
	  * If a filename set in ctor is not valid, setting the "inner" option results in error.
	  *
	 **/
	void set_option(const std::string& option);

	/**
	 * Option inspector. Should echo the option if it is set, return
	 * an empty string otheriwse, and "unknown" if the option is invalid.
	 */
	std::string get_option(const std::string& option) const;

	/**
	 * Check if the reader is valid, should throw if not. Called after
	 * all set_options during factory reader creation.
	 */
	virtual void validate();

	static bool registered;

protected:
	Sentence::Ptr process_sentence(Corpus2::Sentence::Ptr sentence);

private:
	void load_mwes(const std::string& filename);

	Wccl::SentenceContext clone_sentence_add_mwe(Wccl::SentenceContext sentence,
										  int head, const std::set<int>& all,
										  const std::string &new_base);
	std::string get_new_orth_utf8(Corpus2::Sentence::Ptr sentence,
								  const std::set<int>& all);
	
	/// resets inner reader and all state bar MWE index
	void reset();

	MWEIndex mwe_index_;
	/// ptr to inner reader doing the real work of reading a corpus
	TokenReaderPtr inner_reader_;
	/// path for inner reader
	std::string inner_filename_;
	/// type of inner reader
	std::string inner_reader_type;
	/// inner reader option
	size_t token_index;
	/// contains last processed sentence
	Sentence::Ptr currentSentence;
	/// contains last processed chunk
	boost::shared_ptr<Chunk> currentChunk;
	/// quantity of loaded mwes files
	size_t mwes_counter;
};

} // ns Corpus2

#endif // LIBMWEREADER_MWEREADER_H
