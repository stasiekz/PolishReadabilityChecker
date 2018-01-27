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
#include <libwccl/ops/functions/strset/getlemmas.h>

#include "optest.h"

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(get_lemmas)

		struct LemmasPredFix : public Wccl::PositionFixture
{
	LemmasPredFix()
		: Wccl::PositionFixture(),
		  s(boost::make_shared<Corpus2::Sentence>()),
		  sc(s),
		  tagset(),
		  cx(sc, boost::make_shared<Variables>()),
		  empty_set(),
		  first_lemmas(),
		  second_lemmas()
	{
		first_lemmas.insert("aaa");
		first_lemmas.insert("bbb");
		second_lemmas.insert("ccc");
		second_lemmas.insert("ddd");
		Corpus2::Token* the_token = new Corpus2::Token(
				"One",
				PwrNlp::Whitespace::ManySpaces);
		Corpus2::Tag t1(Corpus2::mask_t(0));
		Corpus2::Lexeme l1("aaa", t1);
		Corpus2::Lexeme l2("bbb", t1);
		the_token->add_lexeme(l1);
		the_token->add_lexeme(l2);
		s->append(the_token);
		Corpus2::Token* another_token = new Corpus2::Token(
				"Two",
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
	StrSet first_lemmas;
	StrSet second_lemmas;

};

BOOST_FIXTURE_TEST_CASE(lemmas_nowhere, LemmasPredFix)
{
	GetLemmas lemmas(nowhere());
	BOOST_CHECK(lemmas.apply(cx)->equals(empty_set));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(empty_set));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(empty_set));
}

BOOST_FIXTURE_TEST_CASE(lemmas_begin, LemmasPredFix)
{
	GetLemmas lemmas(begin());
	BOOST_CHECK(lemmas.apply(cx)->equals(first_lemmas));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(first_lemmas));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(first_lemmas));
}

BOOST_FIXTURE_TEST_CASE(lemmas_end, LemmasPredFix)
{
	GetLemmas lemmas(end());
	BOOST_CHECK(lemmas.apply(cx)->equals(second_lemmas));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(second_lemmas));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(second_lemmas));
}

BOOST_FIXTURE_TEST_CASE(lemmas_zero, LemmasPredFix)
{
	GetLemmas lemmas(pos(0));
	BOOST_CHECK(lemmas.apply(cx)->equals(first_lemmas));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(second_lemmas));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(empty_set));
}

BOOST_FIXTURE_TEST_CASE(lemmas_one, LemmasPredFix)
{
	GetLemmas lemmas(pos(1));
	BOOST_CHECK(lemmas.apply(cx)->equals(second_lemmas));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(empty_set));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(empty_set));
}

BOOST_FIXTURE_TEST_CASE(lemmas_minus_one, LemmasPredFix)
{
	GetLemmas lemmas(pos(-1));
	BOOST_CHECK(lemmas.apply(cx)->equals(empty_set));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(first_lemmas));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(second_lemmas));
	sc.advance();
	BOOST_CHECK(lemmas.apply(cx)->equals(empty_set));
}
//------ to_string test cases -------

BOOST_FIXTURE_TEST_CASE(lemmas_to_string, LemmasPredFix)
{
	GetLemmas lemmas(begin());
	BOOST_CHECK_EQUAL("base[begin]", lemmas.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(lemmas_to_raw_string, LemmasPredFix)
{
	GetLemmas lemmas(end());
	BOOST_CHECK_EQUAL("base[end]", lemmas.to_string(tagset));
	GetLemmas lemmas2(pos(-1));
	BOOST_CHECK_EQUAL("base[-1]", lemmas2.to_string(tagset));
}
BOOST_AUTO_TEST_SUITE_END()
