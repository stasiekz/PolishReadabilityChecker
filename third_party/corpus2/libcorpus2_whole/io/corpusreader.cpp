/*
	Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski, Paweł Kędzia
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
#include <libcorpus2_whole/io/corpusreader.h>
#include <libcorpus2_whole/io/documentcorpusreader.h>

#ifdef WITH_POLIQARP
#include <libcorpus2_whole/io/poliqarpcorpusreader.h>
#endif


namespace Corpus2 {
namespace whole{

CorpusReader::CorpusReader(const Tagset& tagset, const std::string& corpus_type)
	: corpus_type_(corpus_type), tagset_(tagset)
{
	//
}

CorpusReader::~CorpusReader()
{
}

boost::shared_ptr<Corpus> CorpusReader::read(const std::string& corpus_file_path)
{
	try {
		boost::shared_ptr<CorpusReaderI> reader = this->get_corpus_reader_by_type();
		return reader->read(corpus_file_path);
	} catch(...) {
		throw;
	}
}

//
boost::shared_ptr<CorpusReaderI> CorpusReader::get_corpus_reader_by_type()
{
	if (corpus_type_ == "document") {
		return boost::shared_ptr<DocumentCorpusReader>(
				new DocumentCorpusReader(tagset_));
#ifdef WITH_POLIQARP
	} else if (corpus_type_ == "poliqarp") {
		return boost::shared_ptr<PoliqarpCorpusReader>(
				new PoliqarpCorpusReader(tagset_));
#endif
	}
	throw Corpus2Error(corpus_type_ + " is an unknown reader type!");
}

} // whole ns
} // Corpus2 ns
