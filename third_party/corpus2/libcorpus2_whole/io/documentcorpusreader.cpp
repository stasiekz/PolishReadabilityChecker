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
#include <fstream>
#include <boost/algorithm/string.hpp>

#include <libcorpus2/exception.h>
#include <libcorpus2_whole/io/documentcorpusreader.h>
#include <libcorpus2_whole/io/cclrelreader.h>

namespace Corpus2 {
namespace whole {

DocumentCorpusReader::DocumentCorpusReader(const Tagset& tagset) : tagset_(tagset)
{
	//
}

boost::shared_ptr<Corpus> DocumentCorpusReader::read(const std::string& corpus_file_path)
{
	std::string line;
	std::string ann_path, rel_path;
	boost::shared_ptr<CclRelReader> doc_reader;

	std::ifstream corpus_file(corpus_file_path.c_str());
	if (!corpus_file) {
		throw Corpus2Error(corpus_file_path + " file not found!");
	}

	boost::shared_ptr<Corpus> corpus = boost::make_shared<Corpus>(corpus_file_path);
	while(getline(corpus_file, line)) {
		// split line by semicolon
		std::vector<std::string> splitted_line;
		boost::split(splitted_line, line, boost::is_any_of(";"));

		if (splitted_line.empty()) {
			// maybe exception?
			continue;
		}
		else if (splitted_line.size() == 1) {
			throw Corpus2Error("DocumentReader requires both paths to relations and annotations");
		}

		ann_path = splitted_line[0];
		rel_path = splitted_line[1];

		doc_reader = boost::shared_ptr<CclRelReader>(
				new CclRelReader(this->tagset_, ann_path, rel_path));

		corpus->add_document(doc_reader->read());
	}

	return corpus;
}

} // whole ns
} // Corpus2 ns
