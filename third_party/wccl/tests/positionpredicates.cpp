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
#include <libwccl/ops/functions/bool/predicates/isinside.h>
#include <libwccl/ops/functions/bool/predicates/isoutside.h>

#include "optest.h"

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(position_predicates)

struct PosPredFix : public Wccl::PositionFixture
{
	PosPredFix()
		: Wccl::PositionFixture(),
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

BOOST_FIXTURE_TEST_CASE(is_inside_1, PosPredFix)
{
	IsInside is_inside(pos(1));
	BOOST_CHECK(is_inside.apply(cx)->get_value());
	sc.advance();
	BOOST_CHECK(!is_inside.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(is_inside_minus1, PosPredFix)
{
	IsInside is_inside(pos(-1));
	BOOST_CHECK(!is_inside.apply(cx)->get_value());
	sc.advance();
	BOOST_CHECK(is_inside.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(is_inside_nowhere, PosPredFix)
{
	IsInside is_inside(nowhere());
	BOOST_CHECK(!is_inside.apply(cx)->get_value());
	sc.advance();
	BOOST_CHECK(!is_inside.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(is_inside_begin, PosPredFix)
{
	IsInside is_inside(begin());
	BOOST_CHECK(is_inside.apply(cx)->get_value());
	sc.advance();
	BOOST_CHECK(is_inside.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(is_inside_end, PosPredFix)
{
	IsInside is_inside(end());
	BOOST_CHECK(is_inside.apply(cx)->get_value());
	sc.advance();
	BOOST_CHECK(is_inside.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(is_outside_1, PosPredFix)
{
	IsOutside is_outside(pos(1));
	BOOST_CHECK(!is_outside.apply(cx)->get_value());
	sc.advance();
	BOOST_CHECK(is_outside.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(is_outside_minus1, PosPredFix)
{
	IsOutside is_outside(pos(-1));
	BOOST_CHECK(is_outside.apply(cx)->get_value());
	sc.advance();
	BOOST_CHECK(!is_outside.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(is_outside_nowhere, PosPredFix)
{
	IsOutside is_outside(nowhere());
	BOOST_CHECK(is_outside.apply(cx)->get_value());
	sc.advance();
	BOOST_CHECK(is_outside.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(is_outside_begin, PosPredFix)
{
	IsOutside is_outside(begin());
	BOOST_CHECK(!is_outside.apply(cx)->get_value());
	sc.advance();
	BOOST_CHECK(!is_outside.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(is_outside_end, PosPredFix)
{
	IsOutside is_outside(end());
	BOOST_CHECK(!is_outside.apply(cx)->get_value());
	sc.advance();
	BOOST_CHECK(!is_outside.apply(cx)->get_value());
}
//------ to_string test cases -------

BOOST_FIXTURE_TEST_CASE(is_inside_to_string, PosPredFix)
{
	IsInside is_inside(end());
	BOOST_CHECK_EQUAL("inside(end)", is_inside.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(is_inside_to_raw_string, PosPredFix)
{
	IsInside is_inside(end());
	BOOST_CHECK_EQUAL("inside(end)", is_inside.to_raw_string());
}

BOOST_FIXTURE_TEST_CASE(is_outside_to_string, PosPredFix)
{
	IsOutside is_outside(begin());
	BOOST_CHECK_EQUAL("outside(begin)", is_outside.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(is_outside_to_raw_string, PosPredFix)
{
	IsOutside is_outside(nowhere());
	BOOST_CHECK_EQUAL("outside(nowhere)", is_outside.to_raw_string());
}
BOOST_AUTO_TEST_SUITE_END()
