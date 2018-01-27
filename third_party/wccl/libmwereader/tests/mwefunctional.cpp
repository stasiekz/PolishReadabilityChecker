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

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "libmwereader/mwe.h"
#include "libmwereader/mweparser.h"
#include "libmwereader/mwereader.h"
#include <libcorpus2/tagsetmanager.h>

BOOST_AUTO_TEST_SUITE( mwe_functional );

struct Fixture{
	Fixture()
	{
		fill_sentences();

	}
	virtual void fill_sentences()
	{

	#ifdef MWE_READER_TEST_DATA_DIR
		data_dir = boost::filesystem::path(MWE_READER_TEST_DATA_DIR);
		data_dir /= "data";

	#else
		boost::filesystem::path data_dir("data");
	#endif
		test_corpus = data_dir / "test1.kipi.xml";		

	}
	boost::filesystem::path test_corpus, data_dir;

};




BOOST_FIXTURE_TEST_CASE( preferred_lexeme, Fixture)
{
	BOOST_MESSAGE("test: finding preferred lexeme");
	const Corpus2::Tagset& tset = Corpus2::get_named_tagset("kipi");
	Corpus2::MWEReader mwr(tset, test_corpus.string());
	mwr.set_option("inner:xces");
	mwr.set_option("mwefile:"+ (data_dir / "fix_mwe.xml").string());

	Corpus2::Sentence::Ptr s1 = mwr.get_next_sentence();	
	Corpus2::Token* mwu = s1->operator[](4);
	BOOST_CHECK(mwu->get_preferred_lexeme(tset).lemma_utf8() != "");


}

BOOST_FIXTURE_TEST_CASE( lexeme_no_white_spaces, Fixture)
{
	BOOST_MESSAGE("=====================\ntest: no white space after or before");

	const Corpus2::Tagset& tset = Corpus2::get_named_tagset("kipi");
	Corpus2::MWEReader mwr(tset, test_corpus.string());
	mwr.set_option("inner:xces");

	mwr.set_option("mwefile:"+ (data_dir / "fix_mwe.xml").string());
	Corpus2::Sentence::Ptr s1 = mwr.get_next_sentence();
	Corpus2::Token* mwu = s1->operator[](4);
	std::string a = mwu->orth_utf8();
	BOOST_CHECK(a != "dobrej woli ");
	BOOST_CHECK(a != " dobrej woli");
}
BOOST_FIXTURE_TEST_CASE( fix_no_gap , Fixture)
{
	BOOST_MESSAGE("=====================\ntest: finding fixed mwe");
	const Corpus2::Tagset& tset = Corpus2::get_named_tagset("kipi");
	Corpus2::MWEReader mwr(tset, test_corpus.string());
	mwr.set_option("inner:xces");

	mwr.set_option("mwefile:"+ (data_dir / "fix_mwe.xml").string());
	Corpus2::Sentence::Ptr s1 = mwr.get_next_sentence();	
	Corpus2::Token* mwu = s1->operator[](4);
	BOOST_CHECK(mwu->orth_utf8() == "dobrej woli");
	BOOST_CHECK(mwu->get_preferred_lexeme(tset).lemma_utf8() == "dobra wola");

}

BOOST_FIXTURE_TEST_CASE( flex_no_gap , Fixture)
{
	BOOST_MESSAGE("=====================\ntest: finding flex mwe");
	const Corpus2::Tagset& tset = Corpus2::get_named_tagset("kipi");
	Corpus2::MWEReader mwr(tset, test_corpus.string());
	mwr.set_option("inner:xces");

	mwr.set_option("mwefile:"+ (data_dir / "flex_mwe.xml").string());
	mwr.get_next_sentence();
	Corpus2::Sentence::Ptr s2 = mwr.get_next_sentence();
	Corpus2::Sentence::Ptr s3 = mwr.get_next_sentence();
	Corpus2::Token* mwu = s2->operator[](14);
	BOOST_CHECK(mwu->orth_utf8() == "dzień dobry");
	BOOST_CHECK(mwu->get_preferred_lexeme(tset).lemma_utf8() == "dzień dobry");
	Corpus2::Token* mwu2 = s3->operator[](14);
	BOOST_CHECK(mwu2->orth_utf8() == "dobry dzień");
	BOOST_CHECK(mwu2->get_preferred_lexeme(tset).lemma_utf8() == "dzień dobry");
}

BOOST_FIXTURE_TEST_CASE( fix_gap , Fixture)
{
	BOOST_MESSAGE("=====================\ntest: finding fix_gap mwe");
	const Corpus2::Tagset& tset = Corpus2::get_named_tagset("kipi");
	Corpus2::MWEReader mwr(tset, test_corpus.string());
	mwr.set_option("inner:xces");

	mwr.set_option("mwefile:"+ (data_dir / "fix_gap_mwe.xml").string());
	mwr.get_next_sentence();
	mwr.get_next_sentence();
	mwr.get_next_sentence();
	Corpus2::Sentence::Ptr s4 = mwr.get_next_sentence();
	Corpus2::Token* mwu = s4->operator[](27);

	BOOST_CHECK(mwu->orth_utf8() == "instrumentów muzycznych");
	BOOST_CHECK(mwu->get_preferred_lexeme(tset).lemma_utf8() == "instrument muzyczny");
}
BOOST_FIXTURE_TEST_CASE( flex_gap , Fixture)
{
	BOOST_MESSAGE("=====================\ntest: finding flex_gap mwe");
	const Corpus2::Tagset& tset = Corpus2::get_named_tagset("kipi");
	Corpus2::MWEReader mwr(tset, test_corpus.string());
	mwr.set_option("inner:xces");

	mwr.set_option("mwefile:"+ (data_dir / "flex_gap_mwe.xml").string());
	mwr.get_next_sentence();
	mwr.get_next_sentence();
	mwr.get_next_sentence();
	mwr.get_next_sentence();
	Corpus2::Sentence::Ptr s5 = mwr.get_next_sentence();
	Corpus2::Sentence::Ptr s6 = mwr.get_next_sentence();
	Corpus2::Token* mwu = s5->operator[](27);
	BOOST_CHECK(mwu->orth_utf8() == "pracy naukowej");
	BOOST_CHECK(mwu->get_preferred_lexeme(tset).lemma_utf8() == "praca naukowa");
	Corpus2::Token* mwu2 = s6->operator[](29);
	BOOST_CHECK(mwu2->orth_utf8() == "naukowej pracy");
	BOOST_CHECK(mwu2->get_preferred_lexeme(tset).lemma_utf8() == "praca naukowa");
}
//czy head jest w odpowiednm miejscu jesli rzeczownik jest tam w srodku
BOOST_FIXTURE_TEST_CASE( flex_gap_noun , Fixture)
{
	BOOST_MESSAGE("=====================\ntest: finding flex_gap_noun mwe");
	const Corpus2::Tagset& tset = Corpus2::get_named_tagset("kipi");
	Corpus2::MWEReader mwr(tset, test_corpus.string());
	mwr.set_option("inner:xces");

	mwr.set_option("mwefile:"+ (data_dir / "flex_gap_mwe.xml").string());
	mwr.get_next_sentence();
	mwr.get_next_sentence();
	mwr.get_next_sentence();
	mwr.get_next_sentence();
	mwr.get_next_sentence();
	mwr.get_next_sentence();
	Corpus2::Sentence::Ptr s7 = mwr.get_next_sentence();
	Corpus2::Token* mwu = s7->operator[](27);
	BOOST_CHECK(mwu->orth_utf8() == "pracy naukowej");
	BOOST_CHECK(mwu->get_preferred_lexeme(tset).lemma_utf8() == "praca naukowa");
	Corpus2::Sentence::Ptr s8 = mwr.get_next_sentence();
	Corpus2::Token* mwu2 = s8->operator[](31);
	std::string a = mwu2->orth_utf8();
	BOOST_MESSAGE("+++++"+a);
	BOOST_CHECK(mwu2->orth_utf8() == "naukowej pracy");
	BOOST_CHECK(mwu2->get_preferred_lexeme(tset).lemma_utf8() == "praca naukowa");

}

BOOST_FIXTURE_TEST_CASE( get_next_chunk, Fixture)
{
	const Corpus2::Tagset& tset = Corpus2::get_named_tagset("kipi");
	Corpus2::MWEReader mwr1(tset, test_corpus.string());
	mwr1.set_option("inner:xces");
	mwr1.set_option("mwefile:"+ (data_dir / "fix_mwe.xml").string());

	mwr1.get_next_chunk();
	mwr1.get_next_chunk();
	mwr1.get_next_chunk();
	mwr1.get_next_chunk();
	mwr1.get_next_chunk();
	mwr1.get_next_chunk();
	mwr1.get_next_chunk();
	BOOST_CHECK(mwr1.get_next_chunk()!=NULL);
	BOOST_CHECK(mwr1.get_next_chunk()==NULL);
}

BOOST_FIXTURE_TEST_CASE( get_next_sentence, Fixture)
{
	const Corpus2::Tagset& tset = Corpus2::get_named_tagset("kipi");
	Corpus2::MWEReader mwr1(tset, test_corpus.string());
	mwr1.set_option("inner:xces");
	mwr1.set_option("mwefile:"+ (data_dir / "fix_mwe.xml").string());

	mwr1.get_next_sentence();
	mwr1.get_next_sentence();
	mwr1.get_next_sentence();
	mwr1.get_next_sentence();
	mwr1.get_next_sentence();
	mwr1.get_next_sentence();
	mwr1.get_next_sentence();
	BOOST_CHECK(mwr1.get_next_sentence()!=NULL);
	BOOST_CHECK(mwr1.get_next_sentence()==NULL);
}

BOOST_FIXTURE_TEST_CASE( flex_no_gap_new , Fixture)
{
	BOOST_MESSAGE("=====================\ntest: finding flex mwe");
	const Corpus2::Tagset& tset = Corpus2::get_named_tagset("kipi");
	Corpus2::MWEReader mwr(tset, test_corpus.string());
	mwr.set_option("inner:xces");

	mwr.set_option("mwefile:"+ (data_dir / "flex_mwe.xml").string());
	Corpus2::Token* mwu;
	for(int i=0; i<120 ; i++)
	{
		switch(i)
		{
			case 26:
				mwu = mwr.get_next_token();
				BOOST_CHECK(mwu->orth_utf8() == "dzień dobry");
				BOOST_CHECK(mwu->get_preferred_lexeme(tset).lemma_utf8() == "dzień dobry");
				break;
			case 48:
				mwu = mwr.get_next_token();
				BOOST_CHECK(mwu->orth_utf8() == "dobry dzień");
				BOOST_CHECK(mwu->get_preferred_lexeme(tset).lemma_utf8() == "dzień dobry");
				break;
			default:
				mwr.get_next_token();
		}

	}
}





//Check NULL



BOOST_AUTO_TEST_SUITE_END()
