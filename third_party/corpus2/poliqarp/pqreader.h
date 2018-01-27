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
#ifndef LIBSORPUS2_PQREADER_H
#define LIBCORPUS2_PQREADER_H

#include <libcorpus2/io/reader.h>
#include <deque>
#include <boost/scoped_ptr.hpp>

/*
extern "C" {
void pwrnlp_plugin_init();
}
*/

namespace Corpus2 {

// fwd decl
class PoliqarpClient;

class PoliqarpReader: public TokenReader
{
public:
	PoliqarpReader(const Tagset& tagset, const std::string& filename);

	enum PQ_MODE {
		PQ_TOKENS,
		PQ_SENTENCES,
		PQ_CHUNKS,
		PQ_MANUAL
	};

	~PoliqarpReader();

	void set_query(const std::string& query);

	void execute();

	Token* get_next_token();

	Sentence::Ptr get_next_sentence();

	boost::shared_ptr<Chunk> get_next_chunk();

        void setGetWholeSentence(bool getWholeSentence_);

	void set_option(const std::string& option);

	std::string get_option(const std::string& option) const;

	static bool registered;

protected:
	void ensure_more();

	boost::scoped_ptr<PoliqarpClient> pq_;

        bool executed_;

	PQ_MODE mode_;

        bool getWholeSentence;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_PQREADER_H
