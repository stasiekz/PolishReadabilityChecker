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

#ifndef LIBCORPUS2_WHOLE_DOCUMENTREADER_H
#define LIBCORPUS2_WHOLE_DOCUMENTREADER_H
#include <fstream>
#include <string>
#include <libcorpus2_whole/corpus.h>
#include <libcorpus2_whole/io/reader_i.h>
#include <libcorpus2_whole/io/cclrelreader.h>

namespace Corpus2 {
namespace whole {

class DocumentReader
{
public:
	DocumentReader(const Tagset& tagset, const std::string& corpus_type, const std::string& corpus_file_path, const std::string& corpus_reader);
	DocumentReader(const Tagset& tagset, const std::string& corpus_type, const std::string& corpus_file_path);
	boost::shared_ptr<Document> read();
	boost::shared_ptr<Document> read_with_auto_id();

private:

	boost::shared_ptr<CclRelReader> get_cclrel_reader(std::string& line);
	boost::shared_ptr<DocumentReaderI> reader;
        std::ifstream corpus_file;
private:
	const std::string corpus_type_;
	const Tagset& tagset_;
	const std::string corpus_path_;


};

} // whole ns
} // Corpus2 ns

#endif // LIBCORPUS2_WHOLE_DOCUMENTREADER_H
