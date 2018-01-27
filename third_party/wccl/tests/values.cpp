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
#include <libwccl/values/tset.h>
#include <libwccl/values/strset.h>

#include <iostream>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(values)

BOOST_AUTO_TEST_CASE(boolz)
{
	Bool b;
	BOOST_CHECK_EQUAL(b.get_value(), false);
	Value& v = b;
	BOOST_CHECK_EQUAL(v.get_type_name(), Bool::type_name);
}

BOOST_AUTO_TEST_CASE(tsetz)
{
	TSet t;
	BOOST_CHECK(t.get_value().is_null());
	Value& v = t;
	BOOST_CHECK_EQUAL(v.get_type_name(), TSet::type_name);
}

BOOST_AUTO_TEST_CASE(positionz)
{
	Position p;
	BOOST_CHECK(p.get_value() == Position::Nowhere);
	Value& v = p;
	BOOST_CHECK_EQUAL(v.get_type_name(), Position::type_name);
}

BOOST_AUTO_TEST_CASE(strset_ops)
{
	StrSet s1, s2;
	BOOST_CHECK(s1.equals(s2));
	BOOST_CHECK(s1.is_subset_of(s2));
	BOOST_CHECK(s2.is_subset_of(s1));
	BOOST_CHECK(!s1.intersects(s2));
	s1.insert_utf8("aaa");
	BOOST_CHECK(!s1.equals(s2));
	BOOST_CHECK(!s1.is_subset_of(s2));
	BOOST_CHECK(s2.is_subset_of(s1));
	BOOST_CHECK(!s1.intersects(s2));
	s2.insert_utf8("bbb");
	BOOST_CHECK(!s1.equals(s2));
	BOOST_CHECK(!s1.is_subset_of(s2));
	BOOST_CHECK(!s2.is_subset_of(s1));
	BOOST_CHECK(!s1.intersects(s2));
	s2.insert_utf8("aaa");
	BOOST_CHECK(!s1.equals(s2));
	BOOST_CHECK(s1.is_subset_of(s2));
	BOOST_CHECK(!s2.is_subset_of(s1));
	BOOST_CHECK(s1.intersects(s2));
	s1.insert_utf8("bbb");
	BOOST_CHECK(s1.equals(s2));
	BOOST_CHECK(s1.is_subset_of(s2));
	BOOST_CHECK(s2.is_subset_of(s1));
	BOOST_CHECK(s1.intersects(s2));
}

BOOST_AUTO_TEST_CASE(tset_ops)
{
	TSet s1, s2;
	const Corpus2::Tagset& tagset = Corpus2::get_named_tagset("kipi");
	Corpus2::Tag subst_tag = tagset.parse_simple_tag("subst:sg:nom:f");
	Corpus2::Tag adj_tag = tagset.parse_simple_tag("adj:pl:acc:m3:pos");

	BOOST_CHECK(s1.equals(s2));
	BOOST_CHECK(s1.is_subset_of(s2));
	BOOST_CHECK(s2.is_subset_of(s1));
	BOOST_CHECK(!s1.intersects(s2));
	BOOST_CHECK_EQUAL(0, s1.categories_count(tagset));
	BOOST_CHECK_EQUAL(0, s1.matching_categories(subst_tag));
	BOOST_CHECK_EQUAL(0, s1.matching_categories(adj_tag));
	s1.insert_symbol(tagset, "subst");
	BOOST_CHECK_EQUAL(1, s1.categories_count(tagset));
	BOOST_CHECK_EQUAL(1, s1.matching_categories(subst_tag));
	BOOST_CHECK_EQUAL(0, s1.matching_categories(adj_tag));
	BOOST_CHECK(!s1.equals(s2));
	BOOST_CHECK(!s1.is_subset_of(s2));
	BOOST_CHECK(s2.is_subset_of(s1));
	BOOST_CHECK(!s1.intersects(s2));
	s2.insert_symbol(tagset, "pl");
	BOOST_CHECK_EQUAL(1, s2.categories_count(tagset));
	BOOST_CHECK_EQUAL(0, s2.matching_categories(subst_tag));
	BOOST_CHECK_EQUAL(1, s2.matching_categories(adj_tag));
	BOOST_CHECK(!s1.equals(s2));
	BOOST_CHECK(!s1.is_subset_of(s2));
	BOOST_CHECK(!s2.is_subset_of(s1));
	BOOST_CHECK(!s1.intersects(s2));
	s2.insert_symbol(tagset, "subst");
	BOOST_CHECK_EQUAL(2, s2.categories_count(tagset));
	BOOST_CHECK_EQUAL(1, s2.matching_categories(subst_tag));
	BOOST_CHECK_EQUAL(1, s2.matching_categories(adj_tag));
	BOOST_CHECK(!s1.equals(s2));
	BOOST_CHECK(s1.is_subset_of(s2));
	BOOST_CHECK(!s2.is_subset_of(s1));
	BOOST_CHECK(s1.intersects(s2));
	s1.insert_symbol(tagset, "pl");
	BOOST_CHECK_EQUAL(2, s1.categories_count(tagset));
	BOOST_CHECK_EQUAL(1, s1.matching_categories(subst_tag));
	BOOST_CHECK_EQUAL(1, s1.matching_categories(adj_tag));
	BOOST_CHECK(s1.equals(s2));
	BOOST_CHECK(s1.is_subset_of(s2));
	BOOST_CHECK(s2.is_subset_of(s1));
	BOOST_CHECK(s1.intersects(s2));
	s1.insert_symbol(tagset, "sg");
	BOOST_CHECK_EQUAL(2, s1.categories_count(tagset));
	BOOST_CHECK_EQUAL(2, s1.matching_categories(subst_tag));
	BOOST_CHECK_EQUAL(1, s1.matching_categories(adj_tag));
	s1.insert_symbol(tagset, "f");
	BOOST_CHECK_EQUAL(3, s1.categories_count(tagset));
	BOOST_CHECK_EQUAL(3, s1.matching_categories(subst_tag));
	BOOST_CHECK_EQUAL(1, s1.matching_categories(adj_tag));
	s1.insert_symbol(tagset, "adj");
	BOOST_CHECK_EQUAL(3, s1.categories_count(tagset));
	BOOST_CHECK_EQUAL(3, s1.categories_count(tagset));
	BOOST_CHECK_EQUAL(2, s1.matching_categories(adj_tag));

}

BOOST_AUTO_TEST_CASE(position_ops)
{
	boost::shared_ptr<Corpus2::Sentence> s(boost::make_shared<Corpus2::Sentence>());
	SentenceContext sc(s);
	Corpus2::Token* a_token = new Corpus2::Token("ZZ", PwrNlp::Whitespace::ManySpaces);
	Corpus2::Tag t1(Corpus2::mask_t(0));
	Corpus2::Lexeme l1("aaa", t1);
	Corpus2::Lexeme l2("bbb", t1);
	a_token->add_lexeme(l1);
	a_token->add_lexeme(l2);
	s->append(a_token);
	s->append(a_token->clone());
	Position begin(Position::Begin);
	Position end(Position::End);
	Position nowhere(Position::Nowhere);
	Position zero(0);
	Position one(1);
	Position minus_one(-1);
	Position minus_two(-2);

	std::vector<Position> v;
	v.push_back(begin);
	v.push_back(end);
	v.push_back(nowhere);
	v.push_back(zero);
	v.push_back(one);
	v.push_back(minus_one);
	v.push_back(minus_two);
	
	for(size_t i = 0; i < v.size(); ++i) {
		for(size_t j = 0; j < v.size(); ++j) {
			BOOST_CHECK_EQUAL(i == j, v[i].equals(v[j]));
			BOOST_CHECK_EQUAL(i == j, v[j].equals(v[i]));
			if(i >= 2 && j >= 2) { //nowhere, zero, one, minus_one, minus_two
				BOOST_CHECK_EQUAL(i == j, v[j].equals(v[i], sc));
				BOOST_CHECK_EQUAL(i == j, v[j].equals(v[i], sc));
				sc.advance();
				BOOST_CHECK_EQUAL(i == j, v[j].equals(v[i], sc));
				BOOST_CHECK_EQUAL(i == j, v[j].equals(v[i], sc));
				sc.advance();
				BOOST_CHECK_EQUAL(i == j, v[j].equals(v[i], sc));
				BOOST_CHECK_EQUAL(i == j, v[j].equals(v[i], sc));
				sc.goto_start();
			}
		}
	}
	sc.goto_start();
	BOOST_CHECK(begin.equals(zero, sc));
	BOOST_CHECK(zero.equals(begin, sc));
	BOOST_CHECK(one.equals(end, sc));
	BOOST_CHECK(end.equals(one, sc));
	sc.advance();
	BOOST_CHECK(begin.equals(minus_one, sc));
	BOOST_CHECK(minus_one.equals(begin, sc));
	BOOST_CHECK(zero.equals(end, sc));
	BOOST_CHECK(end.equals(zero, sc));
	sc.advance();
	BOOST_CHECK(begin.equals(minus_two, sc));
	BOOST_CHECK(minus_two.equals(begin, sc));
	BOOST_CHECK(minus_one.equals(end, sc));
	BOOST_CHECK(end.equals(minus_one, sc));
}

BOOST_AUTO_TEST_CASE(bool_safe_bool)
{
	Bool b(false);
	BOOST_CHECK(!b);
	if (b) {
		BOOST_ERROR("if (b) on Bool(false)");
	}
	b.set_value(true);
	if (!b) {
		BOOST_ERROR("if (!b) on Bool(true)");
	}
	if (b) {
	} else {
		BOOST_ERROR("if (b) else reached on Bool(true)");
	}
	BOOST_CHECK(b);
}

BOOST_AUTO_TEST_SUITE_END()
