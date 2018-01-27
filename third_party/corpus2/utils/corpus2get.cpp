/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE and COPYING files for more details.
*/

#include <libcorpus2/util/settings.h>
#include <libcorpus2/tagsetparser.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/token.h>
#include <libcorpus2/sentence.h>

#include <libcorpus2/io/reader.h>
#include <libcorpus2/io/writer.h>

#include <boost/foreach.hpp>

#include <boost/program_options.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <fstream>
#include <iterator>

#ifdef HAVE_LIBEDIT
#include <histedit.h>
#endif

#ifdef HAVE_CONFIG_D_H
#include <libcorpus2/config_d.h>
#endif

// not checking for HAVE_VERSION, there is no reason it shouldn't be there
#include <libcorpus2/version.h>

int main(int argc, char** argv)
{
	std::string tagset_load, file;
	bool quiet = false, internals = false;
	bool parse = false, validate = false, sort = false;
	using boost::program_options::value;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("tagset,t", value(&tagset_load)->default_value("kipi"),
			 "tagset")
			("file", value(&file),
			 "file to load")
			("help,h", "Show help")
			("version", "print version string")
			;
	boost::program_options::variables_map vm;
	boost::program_options::positional_options_description p;
	p.add("file", -1);

	try {
		boost::program_options::store(
			boost::program_options::command_line_parser(argc, argv)
			.options(desc).positional(p).run(), vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << "\n";
		return 2;
	}
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		std::cout << "Available tagsets: ";
		std::cout << Corpus2::available_tagsets() << "\n";
		std::cout << "Available readers: ";
		std::cout << boost::algorithm::join(Corpus2::TokenReader::available_reader_types(), " ") << "\n";
		std::cout << "Available writers: ";
		std::cout << boost::algorithm::join(Corpus2::TokenWriter::available_writer_types(), " ") << "\n";
		return 1;
	}
	if (vm.count("version")) {
		std::cout << "corpus-read (libcorpus2) " << LIBCORPUS2_VERSION << "\n";
		return 0;
	}

	try {
		const Corpus2::Tagset& tagset = Corpus2::get_named_tagset(tagset_load);
		const Corpus2::TokenReader::TokenReaderPtr reader = Corpus2::TokenReader::create_path_reader(
			"xces", tagset, file);
		const Corpus2::TokenWriter::TokenWriterPtr writer = Corpus2::TokenWriter::create_stream_writer(
			"xces", std::cout, tagset);
		while (Corpus2::Sentence::Ptr sentence = reader->get_next_sentence()) {
			writer->write_sentence(*sentence);
		}
	} catch (PwrNlp::PwrNlpError& e) {
		std::cerr << "Error: " << e.info() << "\n";
	}
	return 0;
}
