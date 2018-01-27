/* This file is part of WCRFT
  Copyright (C) 2014 Radosław Warzocha, Adam Radziszewski.
  WCRFT is free software; you can redistribute and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option)
  any later version.

  WCRFT is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.

  See the LICENCE and COPYING files for more details
 */

#include <boost/make_shared.hpp>
#include <boost/ref.hpp>

#include <libmaca/io/text.h>
#include <libmaca/io/premorph.h>
#include <libmaca/util/sentenceanalyser.h>

#include "corpusio.h"

namespace Wcrft {

boost::shared_ptr<Corpus2::TokenReader> get_reader(
		const std::string& input_path, const std::string& input_format,
		const Corpus2::Tagset& tagset, const std::string& maca_config)
{
	if (input_path.empty()) {
		// read from stdin
		if (input_format.compare(WCRFT_PLAIN_TEXT_FORMAT) == 0 || input_format.compare(WCRFT_PLAIN_TEXT_FORMAT_ALT) == 0)
			return Maca::PlainTextReader::create_stream_reader(maca_config);
		else if (input_format.compare(WCRFT_PREMORPH_TEXT_FORMAT) == 0)
			return Maca::PremorphTextReader::create_stream_reader(maca_config);
		else
			return Corpus2::TokenReader::create_stream_reader(input_format, tagset, std::cin);
	}
	else {
		// read from file pointed to by input_path
		if (input_format.compare(WCRFT_PLAIN_TEXT_FORMAT) == 0 || input_format.compare(WCRFT_PLAIN_TEXT_FORMAT_ALT) == 0)
			return Maca::PlainTextReader::create_file_reader(input_path, maca_config);
		else if (input_format.compare(WCRFT_PREMORPH_TEXT_FORMAT) == 0)
			return Maca::PremorphTextReader::create_file_reader(input_path, maca_config);
		else
			return Corpus2::TokenReader::create_path_reader(input_format, tagset, input_path);
	}
}

boost::shared_ptr<Corpus2::TokenReader> get_reader(
		std::istream& input_stream, const std::string& input_format,
		const Corpus2::Tagset& tagset, const std::string& maca_config)
{
	typedef Maca::SentenceAnalyser SentAnalyser;
	if (input_format.compare(WCRFT_PLAIN_TEXT_FORMAT) == 0 || input_format.compare(WCRFT_PLAIN_TEXT_FORMAT_ALT) == 0) {
		boost::shared_ptr<SentAnalyser> sentenceAnalyser = SentAnalyser::create_from_named_config(maca_config);
		return boost::make_shared<Maca::TextReader>(boost::ref(input_stream), sentenceAnalyser);
	}
	else if (input_format.compare(WCRFT_PREMORPH_TEXT_FORMAT) == 0) {
		boost::shared_ptr<SentAnalyser> sentenceAnalyser = SentAnalyser::create_from_named_config(maca_config);
		return boost::make_shared<Maca::PremorphReader>(boost::ref(input_stream), sentenceAnalyser);
	}
	else
		return Corpus2::TokenReader::create_stream_reader(input_format, tagset, input_stream);
}

boost::shared_ptr<Corpus2::TokenWriter> get_writer(
		const std::string& output_path, const std::string& output_format,
		const Corpus2::Tagset& tagset)
{
	if (output_path.empty()) {
		// write to stdout
		return Corpus2::TokenWriter::create_stream_writer(output_format, std::cout, tagset);
	}
	else {
		// write to file pointed to by output_path
		return Corpus2::TokenWriter::create_path_writer(output_format, output_path, tagset);
	}
}

boost::shared_ptr<Corpus2::TokenWriter> get_writer(
		std::ostream& output_stream, const std::string& output_format,
		const Corpus2::Tagset& tagset)
{
	return Corpus2::TokenWriter::create_stream_writer(output_format, output_stream, tagset);
}

}
