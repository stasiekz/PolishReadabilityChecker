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

#include <libwccl/ops/functions/vargetter.h>
#include <libwccl/values/bool.h>
#include <libwccl/values/tset.h>
#include <libwccl/values/strset.h>
#include <libwccl/ops/functions/constant.h>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(vargetter)

struct VarGetFix
{
	VarGetFix()
		: sc(boost::make_shared<Corpus2::Sentence>()),
		  cx(sc, boost::make_shared<Variables>()),
		  tagset(),
		  true_value(true),
		  false_value(false),
		  true_constant(true_value),
		  false_constant(false_value)
	{
		cx.variables()->put("True_bool", true_value);
		cx.variables()->put("False_bool", false_value);
	}
	SentenceContext sc;
	FunExecContext cx;
	Corpus2::Tagset tagset;

	Bool true_value;
	Bool false_value;
	Constant<Bool> true_constant;
	Constant<Bool> false_constant;
};

BOOST_FIXTURE_TEST_CASE(bool_apply, VarGetFix)
{
	VariableAccessor<Bool> acc_t = cx.variables()->create_accessor<Bool>("True_bool");
	VarGetter<Bool> var_getter_t(acc_t);
	BOOST_CHECK_EQUAL(true, var_getter_t.apply(cx)->get_value());
	VariableAccessor<Bool> acc_f = cx.variables()->create_accessor<Bool>("False_bool");
	VarGetter<Bool> var_getter_f(acc_f);
	BOOST_CHECK_EQUAL(false, var_getter_f.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(bool_apply_acc_destruct, VarGetFix)
{
	boost::shared_ptr<VarGetter<Bool> > var_getter;
	{
		VariableAccessor<Bool> acc_t = cx.variables()->create_accessor<Bool>("True_bool");
		var_getter.reset(new VarGetter<Bool>(acc_t));
	}
	BOOST_CHECK_EQUAL(true, var_getter->apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(bool_mod, VarGetFix)
{
	VariableAccessor<Bool> acc_t = cx.variables()->create_accessor<Bool>("True_bool");
	VarGetter<Bool> var_getter_t(acc_t);
	BOOST_CHECK_EQUAL(true, var_getter_t.apply(cx)->get_value());
	cx.variables()->get<Bool>("True_bool")->set_value(false);
	BOOST_CHECK_EQUAL(false, var_getter_t.apply(cx)->get_value());
	cx.variables()->put("True_bool", Bool(true));
	BOOST_CHECK_EQUAL(true, var_getter_t.apply(cx)->get_value());
	cx.variables()->set("True_bool", false_value);
	BOOST_CHECK_EQUAL(false, var_getter_t.apply(cx)->get_value());
	cx.variables()->set("True_bool", Bool(true));
	BOOST_CHECK_EQUAL(true, var_getter_t.apply(cx)->get_value());
}

//----  To String -----

BOOST_FIXTURE_TEST_CASE(bool_varget_to_string, VarGetFix)
{
	VariableAccessor<Bool> acc_t = cx.variables()->create_accessor<Bool>("True_bool");
	VarGetter<Bool> var_getter_t(acc_t);
	BOOST_CHECK_EQUAL(Bool::var_repr("True_bool"), var_getter_t.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(bool_varget_to_raw_string, VarGetFix)
{
	VariableAccessor<Bool> acc_t = cx.variables()->create_accessor<Bool>("True_bool");
	VarGetter<Bool> var_getter_t(acc_t);
	BOOST_CHECK_EQUAL(Bool::var_repr("True_bool"), var_getter_t.to_raw_string());
}

BOOST_AUTO_TEST_SUITE_END()
