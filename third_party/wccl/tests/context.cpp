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
#include <boost/bind.hpp>

#include <libwccl/variables.h>
#include <libwccl/sentencecontext.h>

#include <iostream>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(context_position)

BOOST_AUTO_TEST_CASE(contex_init)
{
	SentenceContext sc(boost::make_shared<Corpus2::Sentence>());
	BOOST_CHECK_EQUAL(sc.size(), 0);
	BOOST_CHECK_EQUAL(sc.get_position(), 0);
}

struct Fpos
{
	Fpos()
		: sc(boost::make_shared<Corpus2::Sentence>())
	{
		using namespace Corpus2;
		using namespace PwrNlp::Whitespace;
		sc.get_sentence().append(
				new Token(UnicodeString::fromUTF8("t1"), Space));
		sc.get_sentence().append(
				new Token(UnicodeString::fromUTF8("t2"), Space));
		sc.get_sentence().append(
				new Token(UnicodeString::fromUTF8("t3"), Space));
	}

	SentenceContext sc;
};

BOOST_FIXTURE_TEST_CASE(scadvance, Fpos)
{
	BOOST_CHECK(sc.is_current_inside());
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_position(), 1);
	BOOST_CHECK(sc.is_current_inside());
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_position(), 2);
	BOOST_CHECK(sc.is_current_inside());
	sc.advance();
	BOOST_CHECK(!sc.is_current_inside());
	sc.goto_start();
	BOOST_CHECK(sc.is_current_inside());
	BOOST_CHECK_EQUAL(sc.get_position(), 0);
}

BOOST_FIXTURE_TEST_CASE(pos0, Fpos)
{
	Position p(0);
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 0);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), 0);
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 1);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), 0);
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 2);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), 0);
	sc.advance();
	sc.advance();
	BOOST_CHECK(!sc.is_inside(sc.get_abs_position(p)));
}

BOOST_FIXTURE_TEST_CASE(pos1, Fpos)
{
	Position p(1);
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 1);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), 1);
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 2);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), 1);
	sc.advance();
	BOOST_CHECK(!sc.is_inside(sc.get_abs_position(p)));
}

BOOST_FIXTURE_TEST_CASE(posneg1, Fpos)
{
	Position p(-1);
	BOOST_CHECK(!sc.is_inside(sc.get_abs_position(p)));
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 0);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), -1);
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 1);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), -1);
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 2);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), -1);
	sc.advance();	BOOST_CHECK(!sc.is_inside(sc.get_abs_position(p)));
}

BOOST_FIXTURE_TEST_CASE(pos_begin, Fpos)
{
	Position p(Position::Begin);
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 0);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), 0);
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 0);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), -1);
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 0);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), -2);
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 0);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), -3);
}

BOOST_FIXTURE_TEST_CASE(pos_end, Fpos)
{
	Position p(Position::End);
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 2);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), 2);
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 2);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), 1);
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 2);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), 0);
	sc.advance();
	BOOST_CHECK_EQUAL(sc.get_abs_position(p), 2);
	BOOST_CHECK_EQUAL(sc.get_rel_position(p), -1);
}

BOOST_FIXTURE_TEST_CASE(pos_no, Fpos)
{
	Position p(Position::Nowhere);
	BOOST_CHECK(!sc.is_inside(sc.get_abs_position(p)));
	sc.advance();
	BOOST_CHECK(!sc.is_inside(sc.get_abs_position(p)));
	sc.advance();
	BOOST_CHECK(!sc.is_inside(sc.get_abs_position(p)));
}


BOOST_AUTO_TEST_SUITE_END()

