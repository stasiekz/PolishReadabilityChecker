/*
    Copyright (C) 2011 Adam Wardyński, Tomasz Śniatowski, Paweł Kędzia,
    Adam Radziszewski, Bartosz Broda
    Part of the WCCL project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

/**
  *Tests for mwe.h
  *
  */
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "libmwereader/mwe.h"
#include <libcorpus2/io/reader.h>

BOOST_AUTO_TEST_SUITE( mwe_basic );


struct Fixture{
	Fixture()
	{
		fill_sentences();

	}
	virtual void fill_sentences()
	{

	#ifdef MWE_READER_TEST_DATA_DIR
		boost::filesystem::path data_dir(MWE_READER_TEST_DATA_DIR);
	#else
		boost::filesystem::path data_dir("./data");
	#endif
		boost::filesystem::path test_corpus = data_dir / "test1.kipi.xml";

		tset = Corpus2::get_named_tagset("kipi");
		boost::shared_ptr<Corpus2::TokenReader> reader = Corpus2::TokenReader::create_path_reader("kipi", tset, test_corpus);




	}

	Corpus2::Tagset tset;
};

BOOST_FIXTURE_TEST_CASE( fix_no_gap , Fixture)
{


}
BOOST_FIXTURE_TEST_CASE( flex_no_gap , Fixture)
{

}
BOOST_AUTO_TEST_SUITE_END()
