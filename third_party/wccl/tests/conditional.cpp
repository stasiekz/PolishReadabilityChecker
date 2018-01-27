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
#include <libwccl/ops/functions/conditional.h>

#include <libwccl/values/bool.h>
#include <libwccl/values/tset.h>
#include <libwccl/values/strset.h>

#include <libwccl/sentencecontext.h>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(conditional_predicates)

struct CondFix
{
	CondFix()
		: sc(boost::make_shared<Corpus2::Sentence>()),
		  cx(sc, boost::make_shared<Variables>()),
		  tagset(),
		  true_value(true),
		  false_value(false),
		  true_constant(new Constant<Bool>(true_value)),
		  false_constant(new Constant<Bool>(false_value))
	{		
	}
	SentenceContext sc;
	FunExecContext cx;
	Corpus2::Tagset tagset;

	Bool true_value;
	Bool false_value;
	Conditional<StrSet>::BoolFunctionPtr true_constant;
	Conditional<StrSet>::BoolFunctionPtr false_constant;
};

struct CondFixStrSet : public CondFix
{
	CondFixStrSet()
	: CondFix(),
		iftrue_strset(),
		iffalse_strset(),
		empty_strset(),
		one_elem_strset()
	{
		iftrue_strset.insert("I");
		iftrue_strset.insert("am");
		iftrue_strset.insert("True");
		iftrue_strset_expr = Conditional<StrSet>::ArgFunctionPtr(new Constant<StrSet>(iftrue_strset));
		iffalse_strset.insert("I");
		iffalse_strset.insert("am");
		iffalse_strset.insert("so");
		iffalse_strset.insert("False");
		iffalse_strset_expr = Conditional<StrSet>::ArgFunctionPtr(new Constant<StrSet>(iffalse_strset));
		one_elem_strset.insert("oNe");
		one_elem_strset_expr = Conditional<StrSet>::ArgFunctionPtr(new Constant<StrSet>(one_elem_strset));
		empty_strset_expr = Conditional<StrSet>::ArgFunctionPtr(new Constant<StrSet>(empty_strset));
	}

	StrSet iftrue_strset;
	StrSet iffalse_strset;
	StrSet empty_strset;
	StrSet one_elem_strset;
	Conditional<StrSet>::ArgFunctionPtr iftrue_strset_expr;
	Conditional<StrSet>::ArgFunctionPtr iffalse_strset_expr;
	Conditional<StrSet>::ArgFunctionPtr empty_strset_expr;
	Conditional<StrSet>::ArgFunctionPtr one_elem_strset_expr;
};

BOOST_FIXTURE_TEST_CASE(true_condition_strset, CondFixStrSet)
{
	BOOST_CHECK(!iftrue_strset.equals(iffalse_strset));

	boost::shared_ptr<Conditional<StrSet> > cond(new Conditional<StrSet>(
		true_constant,
		iftrue_strset_expr,
		iffalse_strset_expr));

	BOOST_CHECK(iftrue_strset.equals(*(cond->apply(cx))));
}

BOOST_FIXTURE_TEST_CASE(false_condition_strset, CondFixStrSet)
{
	boost::shared_ptr<Conditional<StrSet> > cond(new Conditional<StrSet>(
			false_constant,
			iftrue_strset_expr,
			iffalse_strset_expr));

	BOOST_CHECK(iffalse_strset.equals(*(cond->apply(cx))));
}

BOOST_FIXTURE_TEST_CASE(true_condition_op_strset, CondFixStrSet)
{
	boost::shared_ptr<ConditionalOp<StrSet> > cond(new ConditionalOp<StrSet>(
		true_constant,
		iftrue_strset_expr));

	BOOST_CHECK(iftrue_strset.equals(*(cond->apply(cx))));
}

BOOST_FIXTURE_TEST_CASE(false_condition_op_strset, CondFixStrSet)
{
	boost::shared_ptr<ConditionalOp<StrSet> > cond(new ConditionalOp<StrSet>(
			false_constant,
			iftrue_strset_expr));

	BOOST_CHECK(empty_strset.equals(*(cond->apply(cx))));
}

//------ to_string test cases -------

BOOST_FIXTURE_TEST_CASE(cond_to_string, CondFixStrSet)
{
	boost::shared_ptr<Conditional<StrSet> > cond(new Conditional<StrSet>(
		true_constant,
		empty_strset_expr,
		one_elem_strset_expr));
	std::string expected = "if(True, [], [\"oNe\"])";
	BOOST_CHECK_EQUAL(expected, cond->to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(cond_to_string_raw, CondFixStrSet)
{
	boost::shared_ptr<Conditional<StrSet> > cond(new Conditional<StrSet>(
		true_constant,
		one_elem_strset_expr));
	std::string expected = "if(True, [\"oNe\"], [])";
	BOOST_CHECK_EQUAL(expected, cond->to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(cond_op_to_string, CondFixStrSet)
{
	boost::shared_ptr<ConditionalOp<StrSet> > cond(new ConditionalOp<StrSet>(
		true_constant,
		empty_strset_expr));
	std::string expected = "? [] ? True";
	BOOST_CHECK_EQUAL(expected, cond->to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(cond_op_to_string_raw, CondFixStrSet)
{
	boost::shared_ptr<ConditionalOp<StrSet> > cond(new ConditionalOp<StrSet>(
		false_constant,
		one_elem_strset_expr));
	std::string expected = "? [\"oNe\"] ? False";
	BOOST_CHECK_EQUAL(expected, cond->to_string(tagset));
}

BOOST_AUTO_TEST_SUITE_END()
