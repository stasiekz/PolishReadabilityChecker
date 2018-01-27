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
#include <libcorpus2/tagsetmanager.h>

#include <libwccl/ops/functions/constant.h>
#include <libwccl/ops/functions/tset/getsymbols.h>
#include <libwccl/ops/functions/tset/getsymbolsinrange.h>

#include "optest.h"

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(get_symbols_in_range_op)

struct SymbolsInRangeFix : public Wccl::PositionFixture
{
	SymbolsInRangeFix()
		: Wccl::PositionFixture(2),
		  s(boost::make_shared<Corpus2::Sentence>()),
		  sc(s),
		  tagset(Corpus2::get_named_tagset("kipi")),
		  cx(sc, boost::make_shared<Variables>())
	{
		Corpus2::Token* the_token = new Corpus2::Token(
				"One",
				PwrNlp::Whitespace::ManySpaces);
		Corpus2::Lexeme l1("aaa", tagset.parse_simple_tag("subst:sg:nom:m1"));
		Corpus2::Lexeme l2("aaa", tagset.parse_simple_tag("subst:sg:nom:m2"));
		the_token->add_lexeme(l1);
		the_token->add_lexeme(l2);
		s->append(the_token);
		Corpus2::Token* another_token = new Corpus2::Token(
				"Two",
				PwrNlp::Whitespace::ManySpaces);
		Corpus2::Lexeme l3("aaa", tagset.parse_simple_tag("subst:pl:dat:f"));
		Corpus2::Lexeme l4("aaa", tagset.parse_simple_tag("prep:nom:wok"));
		Corpus2::Lexeme l5("aaa", tagset.parse_simple_tag("adja"));
		another_token->add_lexeme(l3);
		another_token->add_lexeme(l4);
		another_token->add_lexeme(l5);
		s->append(another_token);
		gnd = tagset.parse_symbol("gnd");
		nmb = tagset.parse_symbol("nmb");
		vcl = tagset.parse_symbol("vcl");
	}

	boost::shared_ptr<Corpus2::Sentence> s;
	SentenceContext sc;
	const Corpus2::Tagset& tagset;

	FunExecContext cx;

	TSet empty;
	Corpus2::Tag gnd;
	Corpus2::Tag nmb;
	Corpus2::Tag vcl;
	Corpus2::Tag tagpos;
};

BOOST_FIXTURE_TEST_CASE(range_nowhere, SymbolsInRangeFix)
{
	for(int i = 0; i < 3; i++)
	{
		GetSymbolsInRange range(gnd, nowhere(), pos(0));
		BOOST_CHECK(range.apply(cx)->equals(empty));
		GetSymbolsInRange r2(gnd, pos(0), nowhere());
		BOOST_CHECK(r2.apply(cx)->equals(empty));
		GetSymbolsInRange r3(gnd, nowhere(), nowhere());
		BOOST_CHECK(r3.apply(cx)->equals(empty));
		sc.advance();
	}
}

BOOST_FIXTURE_TEST_CASE(range_outside, SymbolsInRangeFix)
{
	GetSymbolsInRange range(gnd, pos(-2), pos(-1));
	BOOST_CHECK(range.apply(cx)->equals(empty));
	sc.advance();
	GetSymbolsInRange r2(gnd, pos(1), pos(2));
	BOOST_CHECK(r2.apply(cx)->equals(empty));
}

BOOST_FIXTURE_TEST_CASE(range_valid_including_trimmed, SymbolsInRangeFix)
{
	GetSymbolsInRange range(gnd, pos(-2), pos(0));
	BOOST_CHECK_EQUAL("{m1,m2}", range.apply(cx)->to_string(tagset));
	GetSymbolsInRange r2(gnd, pos(-1), pos(0));
	BOOST_CHECK_EQUAL("{m1,m2}", r2.apply(cx)->to_string(tagset));
	GetSymbolsInRange r3(gnd, pos(1), pos(2));
	BOOST_CHECK_EQUAL("{f}", r3.apply(cx)->to_string(tagset));
	GetSymbolsInRange r4(gnd, pos(0), pos(2));
	BOOST_CHECK_EQUAL("{m1,m2,f}", r4.apply(cx)->to_string(tagset));
	sc.advance();
	BOOST_CHECK_EQUAL("{m1,m2,f}", range.apply(cx)->to_string(tagset));
	BOOST_CHECK_EQUAL("{m1,m2,f}", r2.apply(cx)->to_string(tagset));
	BOOST_CHECK_EQUAL("{}", r3.apply(cx)->to_string(tagset));
	BOOST_CHECK_EQUAL("{f}", r4.apply(cx)->to_string(tagset));
	sc.advance();
	BOOST_CHECK_EQUAL("{m1,m2,f}", range.apply(cx)->to_string(tagset));
	BOOST_CHECK_EQUAL("{f}", r2.apply(cx)->to_string(tagset));
	BOOST_CHECK_EQUAL("{}", r3.apply(cx)->to_string(tagset));
	BOOST_CHECK_EQUAL("{}", r4.apply(cx)->to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(range_same_token, SymbolsInRangeFix)
{
	GetSymbolsInRange range(gnd, pos(0), pos(0));
	BOOST_CHECK_EQUAL("{m1,m2}", range.apply(cx)->to_string(tagset));
	sc.advance();
	BOOST_CHECK_EQUAL("{f}", range.apply(cx)->to_string(tagset));
	sc.advance();
	BOOST_CHECK_EQUAL("{}", range.apply(cx)->to_string(tagset));
}


BOOST_FIXTURE_TEST_CASE(range_to_string, SymbolsInRangeFix)
{
	GetSymbolsInRange range(gnd, pos(0), pos(2));
	BOOST_CHECK_EQUAL("range(gnd, 0, 2)", range.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(range_to_raw_string, SymbolsInRangeFix)
{
	GetSymbolsInRange range(gnd, pos(0), pos(2));
	std::string expected = "range(" + gnd.raw_dump() + ", 0, 2)";
	BOOST_CHECK_EQUAL(expected, range.to_raw_string());
}

BOOST_AUTO_TEST_SUITE_END()
