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
#include <libwccl/ops/functions/tset/getwordclass.h>

#include "optest.h"

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(get_symbols_op)

struct SymbolsFix : public Wccl::PositionFixture
{
	SymbolsFix()
		: Wccl::PositionFixture(),
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
		tag_pos.set_pos(Corpus2::filled_mask);
	}

	boost::shared_ptr<Corpus2::Sentence> s;
	SentenceContext sc;
	const Corpus2::Tagset& tagset;

	FunExecContext cx;
	TSet empty;
	Corpus2::Tag gnd;
	Corpus2::Tag nmb;
	Corpus2::Tag vcl;
	Corpus2::Tag tag_pos;
};

BOOST_FIXTURE_TEST_CASE(symbols_nowhere, SymbolsFix)
{
	GetSymbols symbols(gnd, nowhere());
	BOOST_CHECK(symbols.apply(cx)->equals(empty));
	GetSymbols s2(nmb, nowhere());
	BOOST_CHECK(s2.apply(cx)->equals(empty));
	GetSymbols s3(tag_pos, nowhere());
	BOOST_CHECK(s3.apply(cx)->equals(empty));
}

BOOST_FIXTURE_TEST_CASE(symbols_outside, SymbolsFix)
{
	GetSymbols symbols(gnd, pos(-1));
	BOOST_CHECK(symbols.apply(cx)->equals(empty));
}

BOOST_FIXTURE_TEST_CASE(get_gnd, SymbolsFix)
{
	GetSymbols symbols(gnd, pos(0));
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{m1,m2}");
	sc.advance();
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{f}");
	sc.advance();
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{}");
}

BOOST_FIXTURE_TEST_CASE(get_nmb, SymbolsFix)
{
	GetSymbols symbols(nmb, pos(0));
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{sg}");
	sc.advance();
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{pl}");
	sc.advance();
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{}");
}

BOOST_FIXTURE_TEST_CASE(get_vcl, SymbolsFix)
{
	GetSymbols symbols(vcl, pos(0));
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{}");
	sc.advance();
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{wok}");
	sc.advance();
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{}");
}

BOOST_FIXTURE_TEST_CASE(get_tag_pos, SymbolsFix)
{
	GetSymbols symbols(tag_pos, pos(0));
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{subst}");
	sc.advance();
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{adja,prep,subst}");
	sc.advance();
	BOOST_CHECK_EQUAL(symbols.apply(cx)->to_string(tagset), "{}");
}

BOOST_FIXTURE_TEST_CASE(get_symbols_to_string, SymbolsFix)
{
	GetSymbols symbols(nmb, pos(0));
	BOOST_CHECK_EQUAL("nmb[0]", symbols.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(get_symbols_to_raw_string, SymbolsFix)
{
	GetSymbols symbols(nmb, pos(0));
	std::string expected = nmb.raw_dump() + "[0]";
	BOOST_CHECK_EQUAL(expected, symbols.to_raw_string());
}

//
//  ---------- GetWordClass cases ----------
//

BOOST_FIXTURE_TEST_CASE(get_word_class, SymbolsFix)
{
	GetWordClass wclass(pos(0));
	BOOST_CHECK_EQUAL(wclass.apply(cx)->to_string(tagset), "{subst}");
	sc.advance();
	BOOST_CHECK_EQUAL(wclass.apply(cx)->to_string(tagset), "{adja,prep,subst}");
	sc.advance();
	BOOST_CHECK_EQUAL(wclass.apply(cx)->to_string(tagset), "{}");
}

BOOST_FIXTURE_TEST_CASE(get_word_class_to_string, SymbolsFix)
{
	GetWordClass wclass(pos(0));
	BOOST_CHECK_EQUAL("class[0]", wclass.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(get_word_class_to_raw_string, SymbolsFix)
{
	GetWordClass wclass(pos(0));
	BOOST_CHECK_EQUAL("class[0]", wclass.to_raw_string());
}

BOOST_AUTO_TEST_SUITE_END()
