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
#include <libcorpus2_whole/io/poliqarpdocumentreader.h>
#include <boost/scoped_ptr.hpp>

namespace Corpus2 {
namespace whole {

PoliqarpDocumentReader::PoliqarpDocumentReader(const Tagset& tagset, const std::string& corpus_path, const std::string& corpus_reader)
	: DocumentReaderI("poliqarp")
{
	this->pqr_ = Corpus2::TokenReader::create_path_reader(corpus_reader, tagset, corpus_path);
}

PoliqarpDocumentReader::PoliqarpDocumentReader(const Tagset& tagset, const std::string& corpus_path)
	: DocumentReaderI("poliqarp")
{

	this->pqr_ = Corpus2::TokenReader::create_path_reader("poliqarp", tagset, corpus_path);
}

boost::shared_ptr<Document> PoliqarpDocumentReader::read()
{
	boost::shared_ptr<Document> document;
	boost::shared_ptr<Chunk> chunk = this->pqr_->get_next_chunk();

	if (chunk) {
		document = boost::make_shared<Document>();
		document->add_paragraph(chunk);
	}
	else
	{
		document = boost::make_shared<Document>("End");
	}
	return document;
}


} // whole ns
} // Corpus2 ns
