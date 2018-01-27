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
#include <libcorpus2/ann/annotatedsentence.h>
#include <libwccl/ops/functions/constant.h>

#include <libwccl/ops/functions/bool/predicates/ann.h>
#include <libwccl/ops/functions/bool/predicates/annsub.h>
#include <libwccl/values/match.h>
#include <libwccl/values/tokenmatch.h>

#include "optest.h"

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(ann_ann_sub)

struct AnnSubFix : public Wccl::PositionFixture
{
	AnnSubFix()
		: Wccl::PositionFixture(3),
		as(boost::make_shared<Corpus2::AnnotatedSentence>()),
		sc(as),
		cx(sc, boost::make_shared<Variables>())
	{
		as->append(new Corpus2::Token(UnicodeString::fromUTF8("t1"), PwrNlp::Whitespace::Newline));
		as->append(new Corpus2::Token(UnicodeString::fromUTF8("t2"), PwrNlp::Whitespace::Newline));
		as->append(new Corpus2::Token(UnicodeString::fromUTF8("t3"), PwrNlp::Whitespace::Newline));
		as->append(new Corpus2::Token(UnicodeString::fromUTF8("t4"), PwrNlp::Whitespace::Newline));
		as->append(new Corpus2::Token(UnicodeString::fromUTF8("t5"), PwrNlp::Whitespace::Newline));
		as->create_channel("ch1");
		as->get_channel("ch1").set_segment_at(2, 1);
		as->get_channel("ch1").set_segment_at(3, 1);
		as->get_channel("ch1").set_head_at(3, true);
	}

	boost::shared_ptr<Corpus2::AnnotatedSentence> as;
	SentenceContext sc;
	Corpus2::Tagset tagset;
	FunExecContext cx;
};

BOOST_FIXTURE_TEST_CASE(ann_not, AnnSubFix)
{
	boost::shared_ptr< Constant<Match> > m0, m1;
	m0.reset(new Constant<Match>(Match(TokenMatch(pos_value(0).get_value()))));
	m1.reset(new Constant<Match>(Match(TokenMatch(pos_value(1).get_value()))));
	Ann a(m0, m1, "ch1");
	boost::shared_ptr<const Bool> rv = a.apply(cx);
	BOOST_REQUIRE(rv);
	BOOST_CHECK(!*rv);
	AnnSub as(m0, m1, "ch1");
	rv = a.apply(cx);
	BOOST_REQUIRE(rv);
	BOOST_CHECK(!*rv);
}

BOOST_FIXTURE_TEST_CASE(ann_yes, AnnSubFix)
{
	boost::shared_ptr< Constant<Match> > m0, m1;
	m0.reset(new Constant<Match>(Match(TokenMatch(pos_value(2).get_value()))));
	m1.reset(new Constant<Match>(Match(TokenMatch(pos_value(3).get_value()))));
	Ann a(m0, m1, "ch1");
	boost::shared_ptr<const Bool> rv = a.apply(cx);
	BOOST_REQUIRE(rv);
	BOOST_CHECK(*rv);
	AnnSub as(m0, m1, "ch1");
	rv = as.apply(cx);
	BOOST_REQUIRE(rv);
	BOOST_CHECK(*rv);
}

BOOST_FIXTURE_TEST_CASE(ann_sub, AnnSubFix)
{
	boost::shared_ptr< Constant<Match> > m0, m1;
	m0.reset(new Constant<Match>(Match(TokenMatch(pos_value(2).get_value()))));
	m1.reset(new Constant<Match>(Match(TokenMatch(pos_value(2).get_value()))));
	Ann a(m0, m1, "ch1");
	boost::shared_ptr<const Bool> rv = a.apply(cx);
	BOOST_REQUIRE(rv);
	BOOST_CHECK(!*rv);
	AnnSub as(m0, m1, "ch1");
	rv = as.apply(cx);
	BOOST_REQUIRE(rv);
	BOOST_CHECK(*rv);
}


//------ to_string test cases -------

BOOST_FIXTURE_TEST_CASE(ann_to_string, AnnSubFix)
{
	boost::shared_ptr< Constant<Match> > m0, m1;
	m0.reset(new Constant<Match>(Match(TokenMatch(pos_value(0).get_value()))));
	m1.reset(new Constant<Match>(Match(TokenMatch(pos_value(1).get_value()))));
	Ann ann(m0, m1, "ch");
	BOOST_CHECK_EQUAL("ann(TOK[0], TOK[1], \"ch\")", ann.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(ann_to_string_one, AnnSubFix)
{
	boost::shared_ptr< Constant<Match> > m0, m1;
	m0.reset(new Constant<Match>(Match(TokenMatch(pos_value(0).get_value()))));
	m1.reset(new Constant<Match>(Match(TokenMatch(pos_value(1).get_value()))));
	Ann ann(m0, "ch");
	BOOST_CHECK_EQUAL("ann(TOK[0], \"ch\")", ann.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(annsub_to_string, AnnSubFix)
{
	boost::shared_ptr< Constant<Match> > m0, m1;
	m0.reset(new Constant<Match>(Match(TokenMatch(pos_value(0).get_value()))));
	m1.reset(new Constant<Match>(Match(TokenMatch(pos_value(1).get_value()))));
	AnnSub ann(m0, m1, "ch");
	BOOST_CHECK_EQUAL("annsub(TOK[0], TOK[1], \"ch\")", ann.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(annsub_to_string_one, AnnSubFix)
{
	boost::shared_ptr< Constant<Match> > m0, m1;
	m0.reset(new Constant<Match>(Match(TokenMatch(pos_value(0).get_value()))));
	m1.reset(new Constant<Match>(Match(TokenMatch(pos_value(1).get_value()))));
	AnnSub ann(m0, "ch");
	BOOST_CHECK_EQUAL("annsub(TOK[0], \"ch\")", ann.to_string(tagset));
}

BOOST_AUTO_TEST_SUITE_END()
