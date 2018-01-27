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

#ifndef LIBCORPUS2_IO_RFT_H
#define LIBCORPUS2_IO_RFT_H

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
class RftWriter : public TokenWriter
{
public:
	RftWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params);

	void write_token(const Token& t);

	void write_sentence(const Sentence& s);

	void write_chunk(const Chunk &p);

	void write_tag(const Tag& tag);

	static bool registered;

private:
	bool warn_on_no_lexemes_;

	/// Whether using TiMBL/MBT variant (slightly different than RFT per se).
	bool mbt_dialect_;

	/// Dialect flag: keep colons in tags, do not replace with dots
	bool colon_;

	/// Dialect flag: keep missing optional attributes as empty, do not
	/// replace with attribute name dummy
	bool opt_;

	/// Dialect flag: output all lexemes, not just the preferred one
	bool alltags_;

	/// Dialect flag: use non-utf8 encoding
	std::string encoding_;
};

class RftReader : public BufferedSentenceReader
{
public:
	RftReader(const Tagset& tagset, std::istream& is, bool disamb = false,
		bool mbt_dialect = false); // TODO move to some sort of params

	RftReader(const Tagset& tagset, const std::string& filename, bool disamb = false,
		bool mbt_dialect = false); // TODO move to some sort of params

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

	/// Whether using TiMBL/MBT variant (slightly different than RFT per se).
	bool mbt_dialect_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_RFT_H
