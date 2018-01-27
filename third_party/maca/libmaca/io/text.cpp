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

#include <libmaca/io/text.h>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/ref.hpp>
#include <fstream>
#include <iostream>

namespace Maca {

TextReader::TextReader(std::istream &is,
		const boost::shared_ptr<SentenceAnalyser>& sa,
		int bufsize /* = 1000*/)
	: Corpus2::BufferedSentenceReader(sa->tagset()), is_(is), sa_(sa)
{
	sa_->set_input_source(is, bufsize);
}

Corpus2::Sentence::Ptr TextReader::actual_next_sentence()
{
	return sa_->get_next_sentence();
}

boost::shared_ptr<Corpus2::TokenReader> PlainTextReader::create_file_reader(const std::string& filename, const std::string& config)
{
    boost::shared_ptr<SentenceAnalyser> sentenceAnalyser = SentenceAnalyser::create_from_named_config(config);

    boost::shared_ptr<std::ifstream> is_ = boost::make_shared<std::ifstream>();
    is_->open(filename.c_str(), std::ios_base::in);

    return boost::make_shared<PlainTextReader>(is_, sentenceAnalyser);
}

boost::shared_ptr<Corpus2::TokenReader> PlainTextReader::create_stream_reader(const std::string& config)
{
    boost::shared_ptr<SentenceAnalyser> sentenceAnalyser = SentenceAnalyser::create_from_named_config(config);

    return boost::make_shared<TextReader>(boost::ref(std::cin), sentenceAnalyser);
}

boost::shared_ptr<Corpus2::TokenReader> PlainTextReader::create_string_reader(const std::string& inputText, const std::string& config)
{
    boost::shared_ptr<std::stringstream> inputStream = boost::make_shared<std::stringstream>(inputText, std::stringstream::in);

    boost::shared_ptr<SentenceAnalyser> sentenceAnalyser = SentenceAnalyser::create_from_named_config(config);

    return boost::make_shared<PlainTextReader>(inputStream, sentenceAnalyser);
}

PlainTextReader::PlainTextReader(boost::shared_ptr<std::istream> inputStream, const boost::shared_ptr<SentenceAnalyser>& sa) : TextReader(boost::ref(*inputStream), sa), inputFileStream(inputStream){
}


PlainTextReader::~PlainTextReader(){
}

} /* end ns Maca */
