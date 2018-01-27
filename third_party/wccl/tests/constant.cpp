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

#include <libwccl/ops/functions/constant.h>
#include <libwccl/variables.h>
#include <libwccl/values/bool.h>
#include <libwccl/sentencecontext.h>


using namespace Wccl;

BOOST_AUTO_TEST_SUITE(constant)

struct BoolFix
{
	BoolFix()
		: sc(boost::make_shared<Corpus2::Sentence>()),
		  cx(sc, boost::make_shared<Variables>()),
		  tagset(),
		  true_value(true),
		  false_value(false),
		  true_constant(true_value),
		  false_constant(false_value)
	{
	}
	SentenceContext sc;
	FunExecContext cx;
	Corpus2::Tagset tagset;

	Bool true_value;
	Bool false_value;
	Constant<Bool> true_constant;
	Constant<Bool> false_constant;
};

BOOST_FIXTURE_TEST_CASE(bool_apply, BoolFix)
{
	BOOST_CHECK_EQUAL(true, true_value.get_value());
	BOOST_CHECK_EQUAL(true, true_constant.apply(cx)->get_value());
	BOOST_CHECK_EQUAL(false, false_value.get_value());
	BOOST_CHECK_EQUAL(false, false_constant.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(bool_to_string, BoolFix)
{
	BOOST_CHECK_EQUAL("True", true_value.to_string(tagset));
	BOOST_CHECK_EQUAL(true_value.to_string(tagset), true_constant.to_string(tagset));
	BOOST_CHECK_EQUAL("False", false_value.to_string(tagset));
	BOOST_CHECK_EQUAL(false_value.to_string(tagset), false_constant.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(bool_to_raw_string, BoolFix)
{
	BOOST_CHECK_EQUAL("True", true_value.to_raw_string());
	BOOST_CHECK_EQUAL(true_value.to_raw_string(), true_constant.to_raw_string());
	BOOST_CHECK_EQUAL("False", false_value.to_raw_string());
	BOOST_CHECK_EQUAL(false_value.to_raw_string(), false_constant.to_raw_string());
}

BOOST_AUTO_TEST_SUITE_END()
