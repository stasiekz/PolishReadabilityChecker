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
#include <libwccl/values/position.h>

#include <iostream>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(variables)

BOOST_AUTO_TEST_CASE(v_basic)
{
	Variables v;
	BOOST_CHECK_EQUAL(v.size<Value>(), 0);
	BOOST_CHECK_EQUAL(v.size<Bool>(), 0);
	v.put("a", new Bool(true));
	BOOST_CHECK_EQUAL(v.size<Value>(), 1);
	BOOST_CHECK_EQUAL(v.size<Bool>(), 1);
	boost::shared_ptr<Bool> b = v.get<Bool>("b");
	BOOST_CHECK(!b);
	b = v.get<Bool>("a");
	BOOST_REQUIRE(b);
	BOOST_CHECK_EQUAL(b->get_value(), true);
	v.put("a", new Bool(false));
	BOOST_CHECK_EQUAL(v.size<Value>(), 1);
	BOOST_CHECK_EQUAL(v.size<Bool>(), 1);
	b = v.get<Bool>("a");
	BOOST_REQUIRE(b);
	BOOST_CHECK_EQUAL(b->get_value(), false);
	v.del_variable<Position>("a");
	BOOST_CHECK(v.get<Bool>("a"));
	v.del_variable<Bool>("a");
	BOOST_CHECK(!v.get<Bool>("a"));
}

BOOST_AUTO_TEST_CASE(v_del_any)
{
	Variables v;
	v.put("a", new Bool(true));
	v.put("b", new Bool(true));
	v.del_variable<Bool>("a");
	BOOST_CHECK(!v.get<Value>("a"));
	v.put("a", new Bool(true));
	v.del_variable<Value>("a");
	BOOST_CHECK(!v.get<Value>("a"));
	BOOST_CHECK(v.get<Value>("b"));
}

BOOST_AUTO_TEST_CASE(v_put_any)
{
	Variables v;
	boost::shared_ptr<Value> a;
	a.reset(new Bool(true));
	v.put_any("aaa", a);
	BOOST_CHECK(v.get<Bool>("aaa"));
	BOOST_CHECK(v.get<Bool>("aaa").get() == a.get());
}

struct Vfix
{
	Variables v;
	Vfix() {
		v.put("b1", Bool(false));
		v.put("b2", new Bool(true));
		v.put("p1", boost::shared_ptr<Position>(new Position(1)));
		v.put("p2", boost::make_shared<Position>(2));
		v.put_any("p3", boost::shared_ptr<Value>(new Position(3)));
	}
};

BOOST_FIXTURE_TEST_CASE(get, Vfix)
{
	BOOST_CHECK(v.get<Bool>("b1"));
	BOOST_CHECK(v.get<Value>("b1"));
	BOOST_CHECK(!v.get<Position>("b1"));
	BOOST_CHECK(v.get<Bool>("b2"));
	BOOST_CHECK(v.get<Position>("p1"));
	BOOST_CHECK(v.get<Position>("p2"));
	BOOST_CHECK(v.get<Position>("p3"));
	BOOST_CHECK(v.get<Value>("p3"));
	BOOST_CHECK(!v.get<Bool>("p3"));
	BOOST_CHECK_THROW(v.get_or_throw<Bool>("b9"), InvalidVariableName);
	BOOST_CHECK_THROW(v.get_or_throw<Position>("b1"), VariableTypeMismatch);
	BOOST_CHECK_EQUAL(v.get_or_throw<Bool>("b2"), v.get<Bool>("b2"));
	BOOST_CHECK_EQUAL(v.get_or_throw<Position>("p2"), v.get<Position>("p2"));
}

BOOST_FIXTURE_TEST_CASE(get_put, Vfix)
{
	BOOST_CHECK(!v.get<Bool>("b3"));
	BOOST_CHECK(v.get_put<Bool>("b1"));
	BOOST_CHECK(v.get_put<Bool>("b3"));
	BOOST_CHECK(v.get<Bool>("b3"));
}

BOOST_FIXTURE_TEST_CASE(get_put_bad, Vfix)
{
	BOOST_CHECK_THROW(v.get_put<Bool>("p1"), VariableTypeMismatch);
}

BOOST_FIXTURE_TEST_CASE(put, Vfix)
{
	boost::shared_ptr<Bool> b1 = v.get<Bool>("b1");
	BOOST_CHECK_EQUAL(b1->get_value(), false);
	v.put("b1", Bool(true));
	BOOST_CHECK_EQUAL(b1->get_value(), false);
	boost::shared_ptr<Bool> b2 = v.get<Bool>("b1");
	BOOST_CHECK(b1.get() != b2.get());
	BOOST_CHECK_EQUAL(b2->get_value(), true);
}

BOOST_FIXTURE_TEST_CASE(put_bad, Vfix)
{
	BOOST_CHECK_THROW(v.put("p1", Bool(true)), VariableTypeMismatch);
}

BOOST_FIXTURE_TEST_CASE(set, Vfix)
{
	boost::shared_ptr<Bool> b1 = v.get<Bool>("b1");
	BOOST_CHECK_EQUAL(b1->get_value(), false);
	v.set("b1", Bool(true));
	BOOST_CHECK_EQUAL(b1->get_value(), true);
}

BOOST_FIXTURE_TEST_CASE(set_new, Vfix)
{
	BOOST_CHECK(!v.get<Bool>("b3"));
	v.set("b3", Bool(false));
	BOOST_CHECK(v.get<Bool>("b3"));
}

BOOST_FIXTURE_TEST_CASE(set_bad, Vfix)
{
	BOOST_CHECK_THROW(v.set("p1", Bool(true)), VariableTypeMismatch);
}

BOOST_FIXTURE_TEST_CASE(del_variable, Vfix)
{
	v.del_variable<Bool>("b1");
	BOOST_CHECK(!v.get<Bool>("b1"));
	v.del_variable<Bool>("p2");
	BOOST_CHECK(v.get<Value>("p2"));
	v.del_any("p2");
	BOOST_CHECK(!v.get<Value>("p2"));
}

BOOST_FIXTURE_TEST_CASE(get_all, Vfix)
{
	std::set<std::string> names;
	BOOST_FOREACH (const detail::Vmap<Value>::map_t::value_type& a, v.get_all<Bool>()) {
		names.insert(a.first);
	}
	std::set<std::string> expected;
	expected.insert("b1");
	expected.insert("b2");
	BOOST_CHECK_EQUAL_COLLECTIONS(names.begin(), names.end(), expected.begin(), expected.end());
	names.clear();
	BOOST_FOREACH (const detail::Vmap<Value>::map_t::value_type& a, v.get_all<Value>()) {
		names.insert(a.first);
	}
	expected.insert("p1");
	expected.insert("p2");
	expected.insert("p3");
	BOOST_CHECK_EQUAL_COLLECTIONS(names.begin(), names.end(), expected.begin(), expected.end());
}

BOOST_FIXTURE_TEST_CASE(reset_values, Vfix)
{
	v.reset_values();
	BOOST_CHECK_EQUAL(v.get_value<Bool>("b1"), false);
	BOOST_CHECK_EQUAL(v.get_value<Bool>("b2"), false);
	BOOST_CHECK_EQUAL(v.get_value<Position>("p1"), Position::Nowhere);
	BOOST_CHECK_EQUAL(v.get_value<Position>("p2"), Position::Nowhere);
	BOOST_CHECK_EQUAL(v.get_value<Position>("p3"), Position::Nowhere);
}

BOOST_FIXTURE_TEST_CASE(clone, Vfix)
{
	boost::shared_ptr<Variables> copy(v.clone());
	BOOST_CHECK_EQUAL(v.size<Value>(), copy->size<Value>());
	BOOST_CHECK_EQUAL(v.size<Bool>(), copy->size<Bool>());
	BOOST_CHECK_EQUAL(v.size<Position>(), copy->size<Position>());
	BOOST_FOREACH (const detail::Vmap<Value>::map_t::value_type& a, v.get_all<Value>()) {
		boost::shared_ptr<Value> orig = a.second;
		std::string name = a.first;
		boost::shared_ptr<Value> other = copy->get<Value>(name);
		BOOST_REQUIRE(other);
		BOOST_CHECK(orig != other);
		BOOST_CHECK_EQUAL(orig->to_raw_string(), other->to_raw_string());
	}
	BOOST_FOREACH (const detail::Vmap<Bool>::map_t::value_type& a, v.get_all<Bool>()) {
		boost::shared_ptr<Bool> orig = a.second;
		std::string name = a.first;
		boost::shared_ptr<Bool> other = copy->get<Bool>(name);
		BOOST_REQUIRE(other);
		BOOST_CHECK(orig != other);
		BOOST_CHECK_EQUAL(orig->get_value(), other->get_value());
	}
	BOOST_FOREACH (const detail::Vmap<Position>::map_t::value_type& a, v.get_all<Position>()) {
		boost::shared_ptr<Position> orig = a.second;
		std::string name = a.first;
		boost::shared_ptr<Position> other = copy->get<Position>(name);
		BOOST_REQUIRE(other);
		BOOST_CHECK(orig != other);
		BOOST_CHECK_EQUAL(orig->get_value(), other->get_value());
	}
}

BOOST_AUTO_TEST_SUITE_END()

