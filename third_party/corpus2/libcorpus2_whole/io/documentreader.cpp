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
#include <boost/algorithm/string.hpp>
#include <libcorpus2_whole/io/documentreader.h>
#include <libcorpus2_whole/io/cclrelreader.h>

#ifdef WITH_POLIQARP
#include <libcorpus2_whole/io/poliqarpdocumentreader.h>
#endif


namespace Corpus2 {
namespace whole{

DocumentReader::DocumentReader(const Tagset& tagset, const std::string& corpus_type, const std::string& corpus_file_path, const std::string& corpus_reader)
	: corpus_type_(corpus_type), tagset_(tagset), corpus_path_(corpus_file_path)
{
	if (corpus_type_ == "document") {
		corpus_file.open(corpus_file_path.c_str());
#ifdef WITH_POLIQARP
	} else if (corpus_type_ == "poliqarp") {
		reader = boost::shared_ptr<PoliqarpDocumentReader>(
					new PoliqarpDocumentReader(tagset_, corpus_path_, corpus_reader));
#endif
	} else {
		throw Corpus2Error(corpus_type_ + " is an unknown reader type!");
	}
}

DocumentReader::DocumentReader(const Tagset& tagset, const std::string& corpus_type, const std::string& corpus_file_path)
	: corpus_type_(corpus_type), tagset_(tagset), corpus_path_(corpus_file_path)
{
	if (corpus_type_ == "document") {
		corpus_file.open(corpus_file_path.c_str());
#ifdef WITH_POLIQARP
	} else if (corpus_type_ == "poliqarp") {
		reader = boost::shared_ptr<PoliqarpDocumentReader>(
					new PoliqarpDocumentReader(tagset_, corpus_path_, "poliqarp"));
#endif
	} else {
		throw Corpus2Error(corpus_type_ + " is an unknown reader type!");
	}
}

boost::shared_ptr<Document> DocumentReader::read()
{
	std::string line;
#ifdef WITH_POLIQARP
	if (corpus_type_ == "poliqarp") {
		return this->reader->read();
	}
#endif
	if (corpus_type_ == "document") {
		if (std::getline(corpus_file, line)) {
			return get_cclrel_reader(line)->read();
		} else {
			return boost::make_shared<Document>("End");
		}
	}
	throw Corpus2Error(corpus_type_ + " is an unknown reader type!");
}

boost::shared_ptr<Document> DocumentReader::read_with_auto_id()
{
	std::string line;
#ifdef WITH_POLIQARP
	if (corpus_type_ == "poliqarp") {
		return this->reader->read();
	}
#endif
	if (corpus_type_ == "document") {
		if (std::getline(corpus_file, line)) {
			boost::shared_ptr<CclRelReader> cclrel_reader = get_cclrel_reader(line);
			cclrel_reader->get_option("autogen_sent_id");
			cclrel_reader->set_option("autogen_chunk_id");
			return cclrel_reader->read();
		} else {
			return boost::make_shared<Document>("End");
		}
	}
	throw Corpus2Error(corpus_type_ + " is an unknown reader type!");
}


boost::shared_ptr<CclRelReader> DocumentReader::get_cclrel_reader(std::string& line)
{
	std::string ann_path, rel_path;

	// split line by semicolon
	std::vector<std::string> splitted_line;
	boost::split(splitted_line, line, boost::is_any_of(";"));

	if (splitted_line.empty()) {
		throw Corpus2Error("Empty line in corpus file!");
	}
	else if (splitted_line.size() == 1) {
		throw Corpus2Error("CclRelReader requires both paths to relations and annotations");
	}

	ann_path = splitted_line[0];
	rel_path = splitted_line[1];

	return boost::shared_ptr<CclRelReader>(
			new CclRelReader(tagset_, ann_path, rel_path));
}


} // whole ns
} // Corpus2 ns
