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
#include <libcorpus2/tagsetmanager.h>

#include <libwccl/sentencecontext.h>
#include <libwccl/variables.h>

#include <libwccl/values/match.h>

#include <libwccl/variables.h>

#include <iostream>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(match_value)

BOOST_AUTO_TEST_CASE(empty)
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> ptr;
	Wccl::Match m;
	BOOST_CHECK(m.empty());
	BOOST_CHECK_EQUAL(m.first_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m.last_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m.to_raw_string(), "MATCH()");
}

BOOST_AUTO_TEST_CASE(token)
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> ptr;
	Wccl::TokenMatch m(1);
	BOOST_CHECK(!m.empty());
	BOOST_CHECK_EQUAL(m.first_token(ptr), 1);
	BOOST_CHECK_EQUAL(m.last_token(ptr), 1);
	BOOST_CHECK_EQUAL(m.to_raw_string(), "TOK[1]");
}

BOOST_AUTO_TEST_CASE(vector1)
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> ptr;
	Wccl::MatchVector m;
	BOOST_CHECK(m.empty());
	BOOST_CHECK_EQUAL(m.first_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m.last_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m.to_raw_string(), "MATCH()");
	m.append(boost::make_shared<Wccl::MatchVector>());
	BOOST_CHECK(m.empty());
	BOOST_CHECK_EQUAL(m.first_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m.last_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m.to_raw_string(), "MATCH(MATCH())");
	m.append(boost::make_shared<Wccl::MatchVector>());
	BOOST_CHECK(m.empty());
	BOOST_CHECK_EQUAL(m.first_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m.last_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m.to_raw_string(), "MATCH(MATCH(),MATCH())");
	Wccl::MatchVector m2;
	m2.append(boost::make_shared<Wccl::MatchVector>(m));
	BOOST_CHECK(m2.empty());
	BOOST_CHECK_EQUAL(m2.first_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m2.last_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m2.to_raw_string(), "MATCH(MATCH(MATCH(),MATCH()))");
	m2.append(boost::make_shared<Wccl::MatchVector>());
	BOOST_CHECK(m2.empty());
	BOOST_CHECK_EQUAL(m2.first_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m2.last_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m2.to_raw_string(), "MATCH(MATCH(MATCH(),MATCH()),MATCH())");
}

BOOST_AUTO_TEST_CASE(matchvector_first_last)
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> ptr;
	Wccl::MatchVector m;
	m.append(boost::make_shared<TokenMatch>(5));
	BOOST_CHECK_EQUAL(m.to_raw_string(), "MATCH(TOK[5])");
	BOOST_CHECK_EQUAL(m.first_token(ptr), 5);
	BOOST_CHECK_EQUAL(m.last_token(ptr), 5);
	m.append(boost::make_shared<TokenMatch>(6));
	BOOST_CHECK_EQUAL(m.first_token(ptr), 5);
	BOOST_CHECK_EQUAL(m.last_token(ptr), 6);
	BOOST_CHECK_EQUAL(m.to_raw_string(), "MATCH(TOK[5],TOK[6])");
	m.append(boost::make_shared<TokenMatch>(4));
	BOOST_CHECK_EQUAL(m.first_token(ptr), 4);
	BOOST_CHECK_EQUAL(m.last_token(ptr), 6);
	BOOST_CHECK_EQUAL(m.to_raw_string(), "MATCH(TOK[5],TOK[6],TOK[4])");
	Wccl::MatchVector m2;
	m2.append(boost::make_shared<TokenMatch>(5));
	boost::shared_ptr<Wccl::MatchVector> m1 = boost::make_shared<Wccl::MatchVector>(m);
	m2.append(m1);
	BOOST_CHECK_EQUAL(m2.first_token(ptr), 4);
	BOOST_CHECK_EQUAL(m2.last_token(ptr), 6);
	BOOST_CHECK_EQUAL(m2.to_raw_string(), "MATCH(TOK[5],MATCH(TOK[5],TOK[6],TOK[4]))");
	m2.append(boost::make_shared<TokenMatch>(2));
	BOOST_CHECK_EQUAL(m2.first_token(ptr), 2);
	BOOST_CHECK_EQUAL(m2.last_token(ptr), 6);
	m2.append(boost::make_shared<TokenMatch>(7));
	BOOST_CHECK_EQUAL(m2.first_token(ptr), 2);
	BOOST_CHECK_EQUAL(m2.last_token(ptr), 7);
	m1->append(boost::make_shared<TokenMatch>(1));
	BOOST_CHECK_EQUAL(m2.first_token(ptr), 1);
	BOOST_CHECK_EQUAL(m2.last_token(ptr), 7);
	Wccl::MatchVector m3;
	BOOST_CHECK_EQUAL(m3.to_raw_string(), "MATCH()");
	BOOST_CHECK_EQUAL(m3.first_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m3.last_token(ptr), Wccl::Position::Nowhere);
	m3.append(boost::make_shared<MatchVector>());
	BOOST_CHECK_EQUAL(m3.to_raw_string(), "MATCH(MATCH())");
	BOOST_CHECK_EQUAL(m3.first_token(ptr), Wccl::Position::Nowhere);
	BOOST_CHECK_EQUAL(m3.last_token(ptr), Wccl::Position::Nowhere);
	m3.append(boost::make_shared<TokenMatch>(1));
	BOOST_CHECK_EQUAL(m3.to_raw_string(), "MATCH(MATCH(),TOK[1])");
	BOOST_CHECK_EQUAL(m3.first_token(ptr), 1);
	BOOST_CHECK_EQUAL(m3.last_token(ptr), 1);
}

BOOST_AUTO_TEST_CASE(varmatch)
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> ptr;
	Wccl::Variables v;
	v.put<Wccl::Match>("a", new Wccl::Match(boost::shared_ptr<MatchData>(new Wccl::TokenMatch(1))));
	BOOST_CHECK_EQUAL(v.get<Wccl::Match>("a")->first_token(ptr), 1);
	BOOST_CHECK(v.get_put<Wccl::Match>("b")->empty());
	BOOST_CHECK_EQUAL(v.get_put<Wccl::Match>("b")->first_token(ptr),
		Wccl::Position::Nowhere);
}

BOOST_AUTO_TEST_SUITE_END()
