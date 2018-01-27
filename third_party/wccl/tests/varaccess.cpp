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

#include <boost/foreach.hpp>

#include <iostream>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(varaccess);

struct VAfx
{
	boost::shared_ptr<Variables> v;
	VAfx() {
		Variables v2;
		v2.put("a", new Bool(true));
		v2.put("b", new Bool(true));
		v2.put("c", new Bool(true));
		v2.put("bb", new Bool(true));
		v2.put("aa", new Position(1));
		v2.put("aaa", new Position(2));
		v.reset(v2.clone());
	}
};


BOOST_FIXTURE_TEST_CASE(access, VAfx)
{
	std::vector<std::string> vnames;
	vnames.push_back("a");
	vnames.push_back("b");
	vnames.push_back("c");
	vnames.push_back("bb");
	BOOST_FOREACH (const std::string vn, vnames) {
		VariableAccessor<Bool> a1 = v->create_accessor<Bool>(vn);
		BOOST_CHECK(v->get_fast(a1) == v->get<Bool>(vn));
		v->set("a", Bool(false));
		BOOST_CHECK(v->get_fast(a1) == v->get<Bool>(vn));
		v->put("a", Bool(true));
		BOOST_CHECK(v->get_fast(a1) == v->get<Bool>(vn));
	}
}

BOOST_FIXTURE_TEST_CASE(badaccess, VAfx)
{
	BOOST_CHECK_THROW(v->create_accessor<Bool>("asd"), InvalidVariableName);
	BOOST_CHECK_THROW(v->create_accessor<Bool>("aaa"), VariableTypeMismatch);
}


BOOST_AUTO_TEST_SUITE_END()

