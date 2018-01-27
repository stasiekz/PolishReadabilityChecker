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
#include <libcorpus2/io/xcesreader.h>
#include <libcorpus2/io/fastxces.h>
#include <libcorpus2/io/writer.h>

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

static char swiatopoglad_noid[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">\n"
"<cesAna xmlns:xlink=\"http://www.w3.org/1999/xlink\" version=\"1.0\" type=\"lex disamb\">\n"
"<chunkList>\n"
"<chunk id=\"ch1\">\n"
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

static char swiatopoglad_broken[] =
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
"<tok>\n"
"<orth>Uważam</orth>\n"
"<lex disamb=\"1\"><base>uważać</base><ctag>fin:sg:pri:imperf</ctag></lex>\n"
"</tok>\n"
"<chunk id=\"ch51\" type=\"tok\">\n"
"<chunk type=\"s\">\n"
"<tok>\n"
"<orth>Uważam</orth>\n"
"<lex disamb=\"1\"><base>uważać</base><ctag>fin:sg:pri:imperf</ctag></lex>\n"
"</tok>\n"
"</chunk>\n"
"</chunk>\n"
"</chunkList>\n"
"</cesAna>\n"
;
}
BOOST_AUTO_TEST_SUITE( io )

BOOST_AUTO_TEST_CASE( iobase )
{
	const Corpus2::Tagset& tagset = Corpus2::get_named_tagset("kipi");
	std::stringstream ssin;
	ssin << swiatopoglad;
	Corpus2::XcesReader xr(tagset, ssin);
	boost::shared_ptr<Corpus2::Chunk> chunk = xr.get_next_chunk();
	std::stringstream ss;
	boost::shared_ptr<Corpus2::TokenWriter> w(Corpus2::TokenWriter::create_stream_writer("xces,flat", ss, tagset));
	w->write_chunk(*chunk);
	w->finish();
	BOOST_CHECK_EQUAL(ss.str(), swiatopoglad);
}


BOOST_AUTO_TEST_CASE( fast )
{
	const Corpus2::Tagset& tagset = Corpus2::get_named_tagset("kipi");
	std::stringstream ssin;
	ssin << swiatopoglad;
	Corpus2::FastXcesReader xr(tagset, ssin);
	boost::shared_ptr<Corpus2::Chunk> chunk = xr.get_next_chunk();
	std::stringstream ss;
	boost::shared_ptr<Corpus2::TokenWriter> w(Corpus2::TokenWriter::create_stream_writer("xces,flat", ss, tagset));
	w->write_chunk(*chunk);
	w->finish();
	BOOST_CHECK_EQUAL(ss.str(), swiatopoglad_noid);
}

BOOST_AUTO_TEST_CASE( io_oo )
{
	const Corpus2::Tagset& tagset = Corpus2::get_named_tagset("kipi");
	std::stringstream ssin;
	ssin << swiatopoglad_broken;
	Corpus2::XcesReader xr(tagset, ssin);
	xr.set_option("no_warn_inconsistent");
	boost::shared_ptr<Corpus2::Chunk> chunk = xr.get_next_chunk();
	std::stringstream ss;
	boost::shared_ptr<Corpus2::TokenWriter> w(Corpus2::TokenWriter::create_stream_writer("xces,flat", ss, tagset));
	w->write_chunk(*chunk);
	w->finish();
	BOOST_CHECK_EQUAL(ss.str(), swiatopoglad);
	chunk = xr.get_next_chunk();
	BOOST_CHECK(chunk);
	chunk = xr.get_next_chunk();
	BOOST_CHECK(chunk);
}

BOOST_AUTO_TEST_CASE( create_reader )
{
	const Corpus2::Tagset& tagset = Corpus2::get_named_tagset("kipi");
	boost::shared_ptr<Corpus2::TokenReader> r;
	std::stringstream ss;
	ss << swiatopoglad;
	r = Corpus2::TokenReader::create_stream_reader("xces,disamb_only,sh", tagset, ss);
	boost::shared_ptr<Corpus2::XcesReader> xr;
	xr = boost::dynamic_pointer_cast<Corpus2::XcesReader>(r);
	BOOST_REQUIRE(xr);
	BOOST_CHECK_EQUAL(xr->get_option("disamb_only"), "disamb_only");
	BOOST_CHECK_EQUAL(xr->get_option("sh"), "sh");
	r = Corpus2::TokenReader::create_stream_reader("xces,disamb_only,strict", tagset, ss);
	xr = boost::dynamic_pointer_cast<Corpus2::XcesReader>(r);
	BOOST_REQUIRE(xr);
	BOOST_CHECK_EQUAL(xr->get_option("disamb_only"), "disamb_only");
	BOOST_CHECK_EQUAL(xr->get_option("sh"), "");
	BOOST_CHECK_EQUAL(xr->get_option("strict"), "strict");
}



BOOST_AUTO_TEST_SUITE_END();
