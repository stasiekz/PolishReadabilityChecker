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

    See the LICENSE.POLIQARP and COPYING files for more details.
*/

#include "pqclient.h"
#include <libcorpus2/tagsetmanager.h>
#include <iostream>
#include <libcorpus2/io/writer.h>

int main(int argc, char** argv)
{
	std::cerr << "C2PQTEST\n";
	if (argc < 3) return 2;
	const Corpus2::Tagset& tagset = Corpus2::get_named_tagset("kipi");
	std::cerr << argv[1] << "\n";
	std::cerr << argv[2] << "\n";
	Corpus2::PoliqarpClient pqc(tagset, argv[1]);
	pqc.compile_query(argv[2]);
	pqc.execute_query();
	boost::shared_ptr<Corpus2::TokenWriter> writer;
	writer = Corpus2::TokenWriter::create_stream_writer("plain", std::cout, tagset);
	//while (Corpus2::Token* t = pqc.get_next_focus_token()) {
	//	writer->write_token_dispose(t);
	//}
	while (Corpus2::Sentence::Ptr s = pqc.get_next_match_sequence(false)) {
		writer->write_sentence(*s);
	}
}
