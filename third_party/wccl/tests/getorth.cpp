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
#include <libwccl/ops/functions/strset/getorth.h>

#include "optest.h"

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(relative_position)

struct OrthPredFix: public Wccl::PositionFixture
{
	OrthPredFix()
		: s(boost::make_shared<Corpus2::Sentence>()),
		  sc(s),
		  tagset(),
		  cx(sc, boost::make_shared<Variables>()),
		  empty_set(),
		  first_orth(),
		  second_orth()
	{
		first_orth.insert("One");
		second_orth.insert("two");
		Corpus2::Token* the_token = new Corpus2::Token(
				*first_orth.contents().begin(),
				PwrNlp::Whitespace::ManySpaces);
		Corpus2::Tag t1(Corpus2::mask_t(0));
		Corpus2::Lexeme l1("aaa", t1);
		Corpus2::Lexeme l2("bbb", t1);
		the_token->add_lexeme(l1);
		the_token->add_lexeme(l2);
		s->append(the_token);
		Corpus2::Token* another_token = new Corpus2::Token(
				*second_orth.contents().begin(),
				PwrNlp::Whitespace::ManySpaces);
		Corpus2::Tag t2(Corpus2::mask_t(0));
		Corpus2::Lexeme l3("ccc", t2);
		Corpus2::Lexeme l4("ddd", t2);
		another_token->add_lexeme(l3);
		another_token->add_lexeme(l4);
		s->append(another_token);
	}

	boost::shared_ptr<Corpus2::Sentence> s;
	SentenceContext sc;
	Corpus2::Tagset tagset;

	FunExecContext cx;
	StrSet empty_set;
	StrSet first_orth;
	StrSet second_orth;

};

BOOST_FIXTURE_TEST_CASE(orth_nowhere, OrthPredFix)
{
	GetOrth orth(nowhere());
	BOOST_CHECK(orth.apply(cx)->equals(empty_set));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(empty_set));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(empty_set));
	sc.goto_start();
}

BOOST_FIXTURE_TEST_CASE(orth_begin, OrthPredFix)
{
	GetOrth orth(begin());
	BOOST_CHECK(orth.apply(cx)->equals(first_orth));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(first_orth));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(first_orth));
	sc.goto_start();
}

BOOST_FIXTURE_TEST_CASE(orth_end, OrthPredFix)
{
	GetOrth orth(end());
	BOOST_CHECK(orth.apply(cx)->equals(second_orth));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(second_orth));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(second_orth));
	sc.goto_start();
}

BOOST_FIXTURE_TEST_CASE(orth_zero, OrthPredFix)
{
	GetOrth orth(pos(0));
	BOOST_CHECK(orth.apply(cx)->equals(first_orth));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(second_orth));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(empty_set));
	sc.goto_start();
}

BOOST_FIXTURE_TEST_CASE(orth_one, OrthPredFix)
{
	GetOrth orth(pos(1));
	BOOST_CHECK(orth.apply(cx)->equals(second_orth));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(empty_set));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(empty_set));
	sc.goto_start();
}

BOOST_FIXTURE_TEST_CASE(orth_minus_one, OrthPredFix)
{
	GetOrth orth(pos(-1));
	BOOST_CHECK(orth.apply(cx)->equals(empty_set));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(first_orth));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(second_orth));
	sc.advance();
	BOOST_CHECK(orth.apply(cx)->equals(empty_set));
	sc.goto_start();
}
//------ to_string test cases -------

BOOST_FIXTURE_TEST_CASE(orth_to_string, OrthPredFix)
{
	GetOrth orth(begin());
	BOOST_CHECK_EQUAL("orth[begin]", orth.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(orth_to_raw_string, OrthPredFix)
{
	GetOrth orth(end());
	BOOST_CHECK_EQUAL("orth[end]", orth.to_string(tagset));
	GetOrth orth2(pos(-1));
	BOOST_CHECK_EQUAL("orth[-1]", orth2.to_string(tagset));
}
BOOST_AUTO_TEST_SUITE_END()
