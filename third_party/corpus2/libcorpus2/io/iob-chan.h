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

#ifndef LIBSORPUS2_IO_IOB_CHAN_H
#define LIBCORPUS2_IO_IOB_CHAN_H

#include <libcorpus2/io/reader.h>
#include <libcorpus2/io/writer.h>
#include <boost/scoped_ptr.hpp>

namespace Corpus2 {

/**
 * Simple writer class to output token in RFTagger-compatible corpora form.
 *
 * One token per line, token line consists of the orth, followed by the
 * tag, followed by newline (one tag per token only). Each sentence
 * is followed by a blank line.
 *
 * The first lexeme is used. No-lexeme tokens trigger a warning unless
 * nowarn is passed.
 */
class IobChanWriter : public TokenWriter
{
public:
	IobChanWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params);

	void write_token(const Token& t);

	void write_sentence(const Sentence& s);

	void write_chunk(const Chunk &p);

	void write_tag(const Tag& tag);

	static bool registered;

private:
	bool warn_on_no_lexemes_;

	bool force_;
};

class IobChanReader : public BufferedSentenceReader
{
public:
	IobChanReader(const Tagset& tagset, std::istream& is);

	IobChanReader(const Tagset& tagset, const std::string& filename);

	std::istream& is() {
		return *is_;
	}

	void set_option(const std::string& option);

	std::string get_option(const std::string& option) const;

	static bool registered;

protected:
	/// BufferedSentenceReader override
	Sentence::Ptr actual_next_sentence();

	std::istream* is_;
	boost::scoped_ptr<std::istream> is_owned_;

	/// Whether to mark all incoming tags as disambiguated
	bool disamb_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_IOB_CHAN_H
