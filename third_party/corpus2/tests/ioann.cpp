/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Lesser Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.

    See the LICENSE.CORPUS2, POLIQARP, COPYING.LESSER and COPYING files for more details.
*/

#include <boost/test/unit_test.hpp>
#include <set>
#include <boost/foreach.hpp>
#include <libpwrutils/bitset.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/io/cclreader.h>
#include <libcorpus2/io/writer.h>
#include <libcorpus2/ann/annotatedsentence.h>

namespace {
static char swiatopoglad[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">\n"
"<cesAna xmlns:xlink=\"http://www.w3.org/1999/xlink\" version=\"1.0\" type=\"lex disamb\">\n"
"<chunkList>\n"
"<chunk id=\"ch51\" type=\"tok\">\n"
"<chunk type=\"s\">\n"
"<tok>\n"
"<orth>Uważam</orth>\n"
"<lex disamb=\"1\"><base>uważać</base><ctag>fin:sg:pri:imperf</ctag></lex>\n"
"</tok>\n"
"<ns/>\n"
"<tok>\n"
"<orth>,</orth>\n"
"<lex disamb=\"1\"><base>,</base><ctag>interp</ctag></lex>\n"
"</tok>\n"
"<tok>\n"
"<orth>że</orth>\n"
"<lex disamb=\"1\"><base>że</base><ctag>conj</ctag></lex>\n"
"</tok>\n"
"<tok>\n"
"<orth>światopogląd</orth>\n"
"<lex><base>światopogląd</base><ctag>subst:sg:acc:m3</ctag></lex>\n"
"<lex disamb=\"1\"><base>światopogląd</base><ctag>subst:sg:nom:m3</ctag></lex>\n"
"</tok>\n"
"</chunk>\n"
"</chunk>\n"
"</chunkList>\n"
"</cesAna>\n"
;

static char swiatopoglad_ann[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<chunkList>\n"
"<chunk id=\"ch51\" type=\"tok\">\n"
"<sentence>\n"
"<tok>\n"
"<ann chan=\"cute\">1</ann>\n"
"<ann chan=\"meh\">1</ann>\n"
"<orth>Uważam</orth>\n"
"<lex disamb=\"1\"><base>uważać</base><ctag>fin:sg:pri:imperf</ctag></lex>\n"
"</tok>\n"
"<ns/>\n"
"<tok>\n"
"<ann chan=\"cute\">1</ann>\n"
"<ann chan=\"meh\">2</ann>\n"
"<orth>,</orth>\n"
"<lex disamb=\"1\"><base>,</base><ctag>interp</ctag></lex>\n"
"</tok>\n"
"<tok>\n"
"<ann chan=\"meh\" head=\"1\">1</ann>\n"
"<orth>że</orth>\n"
"<lex disamb=\"1\"><base>że</base><ctag>conj</ctag></lex>\n"
"</tok>\n"
"<tok>\n"
"<orth>światopogląd</orth>\n"
"<lex><base>światopogląd</base><ctag>subst:sg:acc:m3</ctag></lex>\n"
"<lex disamb=\"1\"><base>światopogląd</base><ctag>subst:sg:nom:m3</ctag></lex>\n"
"<ann chan=\"cute\">2</ann>\n"
"</tok>\n"
"</sentence>\n"
"</chunk>\n"
"</chunkList>\n"
;
}

BOOST_AUTO_TEST_SUITE( ioann )

BOOST_AUTO_TEST_CASE( iobase )
{
	const Corpus2::Tagset& tagset = Corpus2::get_named_tagset("kipi");
	std::stringstream ssin;
	ssin << swiatopoglad_ann;
	Corpus2::CclReader xr(tagset, ssin);
	boost::shared_ptr<Corpus2::Chunk> chunk = xr.get_next_chunk();
	BOOST_REQUIRE(chunk);
	std::stringstream ss;
	boost::shared_ptr<Corpus2::TokenWriter> w(Corpus2::TokenWriter::create_stream_writer("xces,flat", ss, tagset));
	//boost::shared_ptr<Corpus2::TokenWriter> wann(Corpus2::TokenWriter::create("ccl", std::cerr, tagset));
	//wann->write_chunk(*chunk);
	//wann->finish();
	w->write_chunk(*chunk);
	w->finish();
	BOOST_CHECK_EQUAL(ss.str(), swiatopoglad);
	BOOST_REQUIRE(!chunk->sentences().empty());
	boost::shared_ptr<Corpus2::AnnotatedSentence> as;
	as = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(chunk->sentences()[0]);
	BOOST_REQUIRE(as);

	BOOST_REQUIRE(as->has_channel("cute"));
	as->get_channel("cute").make_iob_from_segments();
	BOOST_CHECK_EQUAL(as->get_channel("cute").dump_iob(), "BIOB");
	BOOST_CHECK_EQUAL(as->get_channel("cute").dump_segments(), "1102");
	BOOST_CHECK_EQUAL(as->get_channel("cute").dump_heads(), "    ");
	Corpus2::Sentence::Ptr cute = Corpus2::create_view(as, "cute");
	BOOST_REQUIRE_EQUAL(cute->size(), 3);
	BOOST_CHECK_EQUAL(cute->tokens()[0]->orth_utf8(), "Uważam,");
	BOOST_CHECK_EQUAL(cute->tokens()[1]->orth_utf8(), "że");
	BOOST_CHECK_EQUAL(cute->tokens()[2]->orth_utf8(), "światopogląd");

	BOOST_REQUIRE(as->has_channel("meh"));
	as->get_channel("meh").make_iob_from_segments();
	BOOST_CHECK_EQUAL(as->get_channel("meh").dump_iob(), "BBBO");
	Corpus2::Sentence::Ptr meh = Corpus2::create_view(as, "meh");
	BOOST_REQUIRE_EQUAL(meh->size(), 3);
	BOOST_CHECK_EQUAL(meh->tokens()[0]->orth_utf8(), ",");
	BOOST_CHECK_EQUAL(meh->tokens()[1]->orth_utf8(), "Uważam że");
	BOOST_CHECK_EQUAL(meh->tokens()[2]->orth_utf8(), "światopogląd");

	//std::cerr << as->annotation_info();
}

BOOST_AUTO_TEST_SUITE_END();
