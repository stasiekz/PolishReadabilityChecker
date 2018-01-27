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

#ifndef LIBMACA_IO_PLAIN_H
#define LIBMACA_IO_PLAIN_H

#include <libcorpus2/io/reader.h>
#include <libmaca/util/sentenceanalyser.h>
#include <boost/shared_ptr.hpp>
#include <boost/ref.hpp>

namespace Maca {

/**
 * Class that wraps Maca SentenceAnalyser into Corpus2::TokenReader
 * interface. This allows to use Maca in plain text analysis mode wherever
 * a Corpus2 reader is expected. If the underlying Maca configuration used
 * is able to split input into paragraphs ("chunks"), the returned reader
 * object will also be ablo to return them by calling rdr.get_next_chunk.
 * Note that this behaviour is implemented in Corpus2::BufferedSentenceReader
 * and a paragraph is split when a sentence-initial token is marked as preceded
 * by many newline characters.
 *
 * NOTE: for a more convenient interface please use the below PlainTextReader
 * class, which provides convenience functions to create the reader from
 * given std::istream, a file name or a string. This version allows to create
 * the analyser directly by giving its config name.
 */
class TextReader : public Corpus2::BufferedSentenceReader
{
public:
	TextReader(std::istream& is,
			const boost::shared_ptr<SentenceAnalyser>& sa, int bufsize = 1000);

	std::istream& is() {
		return is_;
	}

protected:
	/// BufferedSentenceReader override
	Corpus2::Sentence::Ptr actual_next_sentence();

	std::istream& is_;

	boost::shared_ptr<SentenceAnalyser> sa_;
};

/**
 * Convenient class that allows to create a Maca analyser using given
 * Maca configuration name and input to be analysed (file name, input stream
 * or (discouraged unless you really want it) string with all the contents to
 * be analysed. The resulting analyser will wrapped as a Corpus2::TokenReader
 * object, which allows for simple usage wherever a standard Corpus2 reader
 * is expected.
 * If the given Maca config is splitting input into paragraphs ("chunks")
 * (it should be), then you can call the reader's get_next_chunk method
 * to obtain subsequent paragraphs (if it wasn't splitting, you would get
 * a huge paragraphs containing all input sentences).
 */
class PlainTextReader : public TextReader
{
public:
    PlainTextReader(boost::shared_ptr<std::istream> inputStream, const boost::shared_ptr<SentenceAnalyser>& sa);
    ~PlainTextReader();

    static boost::shared_ptr<Corpus2::TokenReader> create_file_reader(const std::string& filename, const std::string& config);
    static boost::shared_ptr<Corpus2::TokenReader> create_stream_reader(const std::string& config);
    static boost::shared_ptr<Corpus2::TokenReader> create_string_reader(const std::string& inputText, const std::string& config);

private:

    boost::shared_ptr<std::istream> inputFileStream;

};

} /* end ns Maca */

#endif // LIBMACA_IO_PLAIN_H
