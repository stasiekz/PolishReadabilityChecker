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
#include <boost/shared_ptr.hpp>
#include <libcorpus2/sentence.h>

#include <libwccl/ops/functions/constant.h>
#include <libwccl/ops/functions/position/relativeposition.h>

#include "optest.h"

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(relative_position)

struct PosPredFix : public Wccl::PositionFixture
{
	PosPredFix()
		: Wccl::PositionFixture(2),
		  s(boost::make_shared<Corpus2::Sentence>()),
		  sc(s),
		  tagset(),
		  cx(sc, boost::make_shared<Variables>())
	{
		Corpus2::Token* the_token = new Corpus2::Token("ZZ", PwrNlp::Whitespace::ManySpaces);
		Corpus2::Tag t1(Corpus2::mask_t(0));
		Corpus2::Lexeme l1("aaa", t1);
		Corpus2::Lexeme l2("bbb", t1);
		the_token->add_lexeme(l1);
		the_token->add_lexeme(l2);
		s->append(the_token);
		s->append(the_token->clone());
	}

	boost::shared_ptr<Corpus2::Sentence> s;
	SentenceContext sc;
	Corpus2::Tagset tagset;

	FunExecContext cx;

};

BOOST_FIXTURE_TEST_CASE(rel_nowhere, PosPredFix)
{
	for(int offset = -2; offset < 3; offset++)
	{
		RelativePosition relpos(nowhere(), offset);
		BOOST_CHECK_EQUAL(
			Position::Nowhere,
			relpos.apply(cx)->get_value());
		sc.advance();
		BOOST_CHECK_EQUAL(
			Position::Nowhere,
			relpos.apply(cx)->get_value());
		sc.advance();
		BOOST_CHECK_EQUAL(
			Position::Nowhere,
			relpos.apply(cx)->get_value());
		sc.goto_start();
	}
}

BOOST_FIXTURE_TEST_CASE(rel_begin, PosPredFix)
{
	for(int offset = -2; offset < 3; offset++)
	{
		RelativePosition relpos(begin(), offset);
		BOOST_CHECK_EQUAL(
			offset,
			cx.sentence_context().get_abs_position(*relpos.apply(cx)));
		sc.advance();
		BOOST_CHECK_EQUAL(
			offset,
			cx.sentence_context().get_abs_position(*relpos.apply(cx)));
		sc.advance();
		BOOST_CHECK_EQUAL(
			offset,
			cx.sentence_context().get_abs_position(*relpos.apply(cx)));
		sc.goto_start();
	}
}

BOOST_FIXTURE_TEST_CASE(rel_end, PosPredFix)
{
	for(int i = -2; i < 3; i++)
	{
		RelativePosition relpos(end(), i);
		BOOST_CHECK_EQUAL(
			sc.size() - 1 + i,
			cx.sentence_context().get_abs_position(*relpos.apply(cx)));
		sc.advance();
		BOOST_CHECK_EQUAL(
			sc.size() - 1 + i,
			cx.sentence_context().get_abs_position(*relpos.apply(cx)));
		sc.advance();
		BOOST_CHECK_EQUAL(
			sc.size() - 1 + i,
			cx.sentence_context().get_abs_position(*relpos.apply(cx)));
		sc.goto_start();
	}
}

BOOST_FIXTURE_TEST_CASE(rel_normal_pos, PosPredFix)
{
	for(int offset = -2; offset < 3; offset++)
	{
		for(int ppos = -2; ppos < 3; ppos++)
		{
			RelativePosition relpos(pos(ppos), offset);
			BOOST_CHECK_EQUAL(
				offset + ppos,
				relpos.apply(cx)->get_value());
			sc.advance();
			BOOST_CHECK_EQUAL(
				offset + ppos,
				relpos.apply(cx)->get_value());
			sc.advance();
			BOOST_CHECK_EQUAL(
				offset + ppos,
				relpos.apply(cx)->get_value());
			sc.goto_start();
		}
	}
}

//------ to_string test cases -------

BOOST_FIXTURE_TEST_CASE(relpos_to_string, PosPredFix)
{
	RelativePosition relpos(begin(), 4);
	BOOST_CHECK_EQUAL("begin + 4", relpos.to_string(tagset));
	RelativePosition relpos_min1(pos(-1), -1);
	BOOST_CHECK_EQUAL("-1 - 1", relpos_min1.to_string(tagset));
	RelativePosition relpos_zero(end(), 0);
	BOOST_CHECK_EQUAL("end + 0", relpos_zero.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(relpos_to_raw_string, PosPredFix)
{
	RelativePosition relpos(nowhere(), 42);
	BOOST_CHECK_EQUAL("nowhere + 42", relpos.to_raw_string());
	RelativePosition relpos_min1(pos(1), -1);
	BOOST_CHECK_EQUAL("1 - 1", relpos_min1.to_raw_string());
	RelativePosition relpos_zero(end(), 0);
	BOOST_CHECK_EQUAL("end + 0", relpos_zero.to_raw_string());
}
BOOST_AUTO_TEST_SUITE_END()
