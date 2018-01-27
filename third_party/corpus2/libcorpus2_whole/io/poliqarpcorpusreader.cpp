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
#include <libcorpus2_whole/io/poliqarpcorpusreader.h>

namespace Corpus2 {
namespace whole{

PoliqarpCorpusReader::PoliqarpCorpusReader(const Tagset& tagset)
	: tagset_(tagset)
{
	//
}

boost::shared_ptr<Corpus> PoliqarpCorpusReader::read(const std::string& corpus_file)
{
	boost::shared_ptr<Document> doc;
	boost::shared_ptr<Corpus> corpus = boost::make_shared<Corpus>(corpus_file);

	this->pq_doc_reader_ = boost::shared_ptr<PoliqarpDocumentReader>(
			new PoliqarpDocumentReader(tagset_, corpus_file));

	while (1) {
		if (!(doc = this->pq_doc_reader_->read())) {
			break;
		}
		corpus->add_document(doc);
	}

	return corpus;
}

} // whole ns
} // Corpus2 ns
