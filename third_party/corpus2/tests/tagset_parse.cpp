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
#include <libcorpus2/tagsetparser.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/util/settings.h>
#include <iostream>

BOOST_AUTO_TEST_SUITE( tagset_parse );

Corpus2::Tagset parse(const char* s)
{
	std::stringstream ss;
	ss << s;
	return Corpus2::TagsetParser::load_ini(ss);
}

#define PRE "[ATTR]\n"
#define POSA "[POS]\n ign\n"

BOOST_AUTO_TEST_CASE( empty )
{
	BOOST_CHECK_THROW(
		parse(""), Corpus2::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( minimal )
{
	Corpus2::Tagset t;
	try {
		t = parse(PRE POSA);
	} catch (Corpus2::TagsetParseError& e) {
		BOOST_FAIL(e.info());
	}
	BOOST_CHECK_EQUAL(t.pos_count(), 1);
}
BOOST_AUTO_TEST_CASE( minimal_nonewline )
{
	Corpus2::Tagset t;
	try {
		t = parse(PRE "[POS]\n ign");
	} catch (Corpus2::TagsetParseError& e) {
		BOOST_FAIL(e.info());
	}
	BOOST_CHECK_EQUAL(t.pos_count(), 1);
}

BOOST_AUTO_TEST_CASE( dupe_val )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a a " POSA), Corpus2::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( dupe_val2 )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\nB c d\n C x a" POSA), Corpus2::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( dupe_sym )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\nB c d\n a x z" POSA), Corpus2::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( dupe_sym2 )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\nB c d" POSA "A B"), Corpus2::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( dupe_attr )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\nB c d\n C x z" POSA), Corpus2::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( dupe_pos )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\n" POSA "P1 A\n P1\n"), Corpus2::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( bad_pos_attr )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\n" POSA "P1 A\n P2 ZZ\n"), Corpus2::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( bad_value_name )
{
	BOOST_CHECK_THROW(
		parse(PRE "@@ a b\n" POSA "P1 A\n P2"), Corpus2::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( size1 )
{
	Corpus2::Tagset t = parse(PRE POSA);
	BOOST_CHECK_EQUAL(t.size(), 1);
	BOOST_CHECK_EQUAL(t.size_extra(), 1);
}

BOOST_AUTO_TEST_CASE( size2 )
{
	Corpus2::Tagset t = parse(PRE POSA " POS2\n");
	BOOST_CHECK_EQUAL(t.size(), 2);
	BOOST_CHECK_EQUAL(t.size_extra(), 2);
}

BOOST_AUTO_TEST_CASE( size3 )
{
	Corpus2::Tagset t = parse(PRE "A a b\n" POSA "POS2 A");
	BOOST_CHECK_EQUAL(t.size(), 3);
	BOOST_CHECK_EQUAL(t.size_extra(), 6);
}

BOOST_AUTO_TEST_CASE( size6 )
{
	Corpus2::Tagset t = parse(PRE "A a b \n B c d\n" POSA "POS2 A\n POS3 [A]\n");
	BOOST_CHECK_EQUAL(t.size(), 6);
	BOOST_CHECK_EQUAL(t.size_extra(), 27);
}

BOOST_AUTO_TEST_CASE( load_named )
{
	PwrNlp::ConfigPathSetter ps(Corpus2::Path::Instance(),
		LIBCORPUS2_TEST_DATA_DIR);
	BOOST_CHECK_NO_THROW(
		try {
			Corpus2::get_named_tagset("test");
		} catch (Corpus2::Corpus2Error& e) {
			std::cerr << e.info();
			throw;
		}
	);
	BOOST_CHECK_THROW(
		Corpus2::get_named_tagset("__nonexistant_9867s8t"),
		Corpus2::FileNotFound);
}

BOOST_AUTO_TEST_SUITE_END();
