/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBMACA_IO_PREMORPH_H
#define LIBMACA_IO_PREMORPH_H

#include <libcorpus2/token.h>
#include <libcorpus2/io/reader.h>
#include <libmaca/morph/morphanalyser.h>

#include <libtoki/tokenizer/tokenizer.h>

#include <boost/scoped_ptr.hpp>

namespace Maca {

class PremorphProcessorImpl;

class SentenceAnalyser;

/**
 * Extracts pieces of text from pre_morph-like XML data, feeds them through
 * the given morphological analyser and sends it to the output stream.
 * NOTE: this implementation does not actually care about the input being
 * valid pre_morph or XCES. It just replaces PCDATA strings with the analyser
 * output (sentences and tokens), while retaining the XML structure.
 * By default the processor outputs recognised sentence boundaries. This may be
 * switched off by set_mark_sentences(false).
 *
 */
class PremorphProcessor
{
public:
	PremorphProcessor(std::ostream& os,
			const boost::shared_ptr<Toki::Tokenizer>& tok,
			const boost::shared_ptr<Maca::MorphAnalyser>& ma);

	PremorphProcessor(std::ostream& os,
			const boost::shared_ptr<SentenceAnalyser>& sa);

	~PremorphProcessor();

	void parse_file(const std::string& filename);

	void parse_stream(std::istream& is);

	void set_stats(bool v);

	/** Sets whether sentence boundaries should be marked in output or not
	  * (default: always mark).
	  */
	void set_mark_sentences(bool shall_mark_sents);

protected:
	boost::scoped_ptr<PremorphProcessorImpl> impl_;

	size_t tokens_;

	size_t sentences_;
};

class PremorphReaderImpl;

/**
 * A convenient TokenReader interface for analysers. Allows to read pre_morph
 * XCES files, have their paragraphs analysed with the given analyser and get
 * output mimicking a regular ordinary TokenReader for already analysed
 * corpora. This allows to use Maca in premorph analysis mode wherever
 * a Corpus2 reader is expected. Premorph files contain division into
 * paragraphs ("chunks") and subsequent paragraphs may be obtained by calling
 * rdr.get_next_chunk.
 *
 * NOTE: for a more convenient interface please use the
 * PremorphTextReader class, which provides convenience functions to create the
 * reader from given std::istream, a file name or a string.
 */
class PremorphReader : public Corpus2::BufferedChunkReader
{
public:
	PremorphReader(std::istream& is,
			const boost::shared_ptr<Toki::Tokenizer>& tok,
			const boost::shared_ptr<Maca::MorphAnalyser>& ma);

	PremorphReader(std::istream& is,
			const boost::shared_ptr<SentenceAnalyser>& sa);

	~PremorphReader();

	std::istream& is() {
		return is_;
	}

protected:
	std::istream& is_;

	void ensure_more();

	boost::scoped_ptr<PremorphReaderImpl> impl_;
};

/**
 * Convenient class that allows to create a Maca analyser for premorph files
 * using given Maca configuration name and input to be analysed (file name or
 * input stream).
 * The resulting analyser will wrapped as a Corpus2::TokenReader object, which
 * allows for simple usage wherever a standard Corpus2 reader is expected.
 * Premorph files contain division into paragraphs and this will be preserved.
 * To get paragraphs, use the reader's get_next_chunk method.
 */
class PremorphTextReader : public PremorphReader{

public:
    PremorphTextReader(boost::shared_ptr<std::ifstream> inputStream, const boost::shared_ptr<SentenceAnalyser>& sentenceAnalyser);
    ~PremorphTextReader();

    static boost::shared_ptr<Corpus2::TokenReader> create_file_reader(const std::string& filename, const std::string& config);
    static boost::shared_ptr<Corpus2::TokenReader> create_stream_reader(const std::string& config);

private:

    boost::shared_ptr<std::ifstream> inputFileStream;

};


} /* end ns Maca */

#endif // LIBMACA_IO_PREMORPH_H
