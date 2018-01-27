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
#include <libwccl/ops/functions/bool/predicates/and.h>
#include <libwccl/ops/functions/bool/predicates/or.h>
#include <libwccl/ops/functions/bool/predicates/nor.h>
#include <libwccl/values/bool.h>
#include <libwccl/sentencecontext.h>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(logical_predicates)

struct PredFix
{
	PredFix()
		: sc(boost::make_shared<Corpus2::Sentence>()),
		  tagset(),
		  true_value(true),
		  false_value(false),
		  true_constant(new Constant<Bool>(true_value)),
		  false_constant(new Constant<Bool>(false_value)),
		  cx(sc, boost::make_shared<Variables>())
	{
	}
	SentenceContext sc;
	Corpus2::Tagset tagset;

	Bool true_value;
	Bool false_value;
	LogicalPredicate::BoolFunctionPtr true_constant;
	LogicalPredicate::BoolFunctionPtr false_constant;

	FunExecContext cx;
};

BOOST_FIXTURE_TEST_CASE(predicate_constants, PredFix)
{
	BOOST_CHECK_EQUAL(true, Predicate::True(cx)->get_value());
	BOOST_CHECK_EQUAL(false, Predicate::False(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(and_1arg, PredFix)
{
	boost::shared_ptr<And::BoolFunctionPtrVector> v(new And::BoolFunctionPtrVector());
	v->push_back(true_constant);
	And pred_and(v);
	BOOST_CHECK_EQUAL(true, pred_and.apply(cx)->get_value());
	v->clear();
	v->push_back(false_constant);
	BOOST_CHECK_EQUAL(false, pred_and.apply(cx)->get_value());
}


BOOST_FIXTURE_TEST_CASE(and_2arg, PredFix)
{
	for(int arg1 = 0; arg1 < 2; ++arg1) {
		for(int arg2 = 0; arg2 < 2; ++arg2) {
			boost::shared_ptr<And::BoolFunctionPtrVector> v(new And::BoolFunctionPtrVector());
			v->push_back(arg1 != 0 ? true_constant : false_constant);
			v->push_back(arg2 != 0 ? true_constant : false_constant);
			And pred_and(v);
			BOOST_CHECK_EQUAL((arg1 != 0) && (arg2 != 0), pred_and.apply(cx)->get_value());
		}
	}
}

BOOST_FIXTURE_TEST_CASE(and_3arg, PredFix)
{
	for(int arg1 = 0; arg1 < 2; ++arg1) {
		for(int arg2 = 0; arg2 < 2; ++arg2) {
			for(int arg3 = 0; arg3 < 2; ++arg3) {
				boost::shared_ptr<And::BoolFunctionPtrVector> v(new And::BoolFunctionPtrVector());
				v->push_back(arg1 != 0 ? true_constant : false_constant);
				v->push_back(arg2 != 0 ? true_constant : false_constant);
				v->push_back(arg3 != 0 ? true_constant : false_constant);
				And pred_and(v);
				BOOST_CHECK_EQUAL((arg1 != 0) && (arg2 != 0) && (arg3 != 0), pred_and.apply(cx)->get_value());
			}
		}
	}
}

BOOST_FIXTURE_TEST_CASE(or_1arg, PredFix)
{
    boost::shared_ptr<Or::BoolFunctionPtrVector> v(new Or::BoolFunctionPtrVector());
    v->push_back(true_constant);
    Or pred_or(v);
	BOOST_CHECK_EQUAL(true, pred_or.apply(cx)->get_value());
    v->clear();
    v->push_back(false_constant);
	BOOST_CHECK_EQUAL(false, pred_or.apply(cx)->get_value());
}


BOOST_FIXTURE_TEST_CASE(or_2arg, PredFix)
{
    for(int arg1 = 0; arg1 < 2; ++arg1) {
        for(int arg2 = 0; arg2 < 2; ++arg2) {
            boost::shared_ptr<Or::BoolFunctionPtrVector> v(new Or::BoolFunctionPtrVector());
            v->push_back(arg1 != 0 ? true_constant : false_constant);
            v->push_back(arg2 != 0 ? true_constant : false_constant);
            Or pred_or(v);
			BOOST_CHECK_EQUAL((arg1 != 0) || (arg2 != 0), pred_or.apply(cx)->get_value());
        }
    }
}

BOOST_FIXTURE_TEST_CASE(or_3arg, PredFix)
{
    for(int arg1 = 0; arg1 < 2; ++arg1) {
        for(int arg2 = 0; arg2 < 2; ++arg2) {
            for(int arg3 = 0; arg3 < 2; ++arg3) {
                boost::shared_ptr<Or::BoolFunctionPtrVector> v(new Or::BoolFunctionPtrVector());
                v->push_back(arg1 != 0 ? true_constant : false_constant);
                v->push_back(arg2 != 0 ? true_constant : false_constant);
                v->push_back(arg3 != 0 ? true_constant : false_constant);
                Or pred_or(v);
				BOOST_CHECK_EQUAL((arg1 != 0) || (arg2 != 0) || (arg3 != 0), pred_or.apply(cx)->get_value());
            }
        }
    }
}

BOOST_FIXTURE_TEST_CASE(nor_1arg, PredFix)
{
    boost::shared_ptr<Nor::BoolFunctionPtrVector> v(new Nor::BoolFunctionPtrVector());
    v->push_back(true_constant);
    Nor pred_nor(v);
	BOOST_CHECK_EQUAL(false, pred_nor.apply(cx)->get_value());
    v->clear();
    v->push_back(false_constant);
	BOOST_CHECK_EQUAL(true, pred_nor.apply(cx)->get_value());
}


BOOST_FIXTURE_TEST_CASE(nor_2arg, PredFix)
{
    for(int arg1 = 0; arg1 < 2; ++arg1) {
        for(int arg2 = 0; arg2 < 2; ++arg2) {
            boost::shared_ptr<Nor::BoolFunctionPtrVector> v(new Nor::BoolFunctionPtrVector());
            v->push_back(arg1 != 0 ? true_constant : false_constant);
            v->push_back(arg2 != 0 ? true_constant : false_constant);
            Nor pred_nor(v);
			BOOST_CHECK_EQUAL(!((arg1 != 0) || (arg2 != 0)), pred_nor.apply(cx)->get_value());
        }
    }
}

BOOST_FIXTURE_TEST_CASE(nor_3arg, PredFix)
{
    for(int arg1 = 0; arg1 < 2; ++arg1) {
        for(int arg2 = 0; arg2 < 2; ++arg2) {
            for(int arg3 = 0; arg3 < 2; ++arg3) {
                boost::shared_ptr<Nor::BoolFunctionPtrVector> v(new Nor::BoolFunctionPtrVector());
                v->push_back(arg1 != 0 ? true_constant : false_constant);
                v->push_back(arg2 != 0 ? true_constant : false_constant);
                v->push_back(arg3 != 0 ? true_constant : false_constant);
                Nor pred_nor(v);
				BOOST_CHECK_EQUAL(!((arg1 != 0) || (arg2 != 0) || (arg3 != 0)), pred_nor.apply(cx)->get_value());
            }
        }
    }
}


//------ to_string test cases -------

BOOST_FIXTURE_TEST_CASE(and_to_string, PredFix)
{
	boost::shared_ptr<And::BoolFunctionPtrVector> v(new And::BoolFunctionPtrVector());
	v->push_back(true_constant);
	boost::shared_ptr<Function<Bool> > pred_and(new And(v));
	BOOST_CHECK_EQUAL("and(True)", pred_and->to_string(tagset));
	v->push_back(false_constant);
	BOOST_CHECK_EQUAL("and(True, False)", pred_and->to_string(tagset));
	v->push_back(true_constant);
	BOOST_CHECK_EQUAL("and(True, False, True)", pred_and->to_string(tagset));

	boost::shared_ptr<And::BoolFunctionPtrVector> v2(new And::BoolFunctionPtrVector());
	v2->push_back(false_constant);
	v2->push_back(pred_and);
	And another_and(v2);
	BOOST_CHECK_EQUAL("and(False, and(True, False, True))", another_and.to_string(tagset));
	v2->push_back(false_constant);
	BOOST_CHECK_EQUAL("and(False, and(True, False, True), False)", another_and.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(and_to_raw_string, PredFix)
{
	boost::shared_ptr<And::BoolFunctionPtrVector> v(new And::BoolFunctionPtrVector());
	v->push_back(false_constant);
	boost::shared_ptr<Function<Bool> > pred_and(new And(v));
	BOOST_CHECK_EQUAL("and(False)", pred_and->to_raw_string());
	v->push_back(true_constant);
	BOOST_CHECK_EQUAL("and(False, True)", pred_and->to_raw_string());
	v->push_back(true_constant);
	BOOST_CHECK_EQUAL("and(False, True, True)", pred_and->to_raw_string());

	boost::shared_ptr<And::BoolFunctionPtrVector> v2(new And::BoolFunctionPtrVector());
	v2->push_back(true_constant);
	v2->push_back(pred_and);
	And another_and(v2);
	BOOST_CHECK_EQUAL("and(True, and(False, True, True))", another_and.to_raw_string());
}

BOOST_FIXTURE_TEST_CASE(or_to_string, PredFix)
{
    boost::shared_ptr<Or::BoolFunctionPtrVector> v(new Or::BoolFunctionPtrVector());
    v->push_back(true_constant);
    boost::shared_ptr<Function<Bool> > pred_or(new Or(v));
	BOOST_CHECK_EQUAL("or(True)", pred_or->to_string(tagset));
    v->push_back(false_constant);
	BOOST_CHECK_EQUAL("or(True, False)", pred_or->to_string(tagset));
    v->push_back(true_constant);
	BOOST_CHECK_EQUAL("or(True, False, True)", pred_or->to_string(tagset));

    boost::shared_ptr<Or::BoolFunctionPtrVector> v2(new Or::BoolFunctionPtrVector());
    v2->push_back(pred_or);
	v2->push_back(false_constant);
    Or another_or(v2);
	BOOST_CHECK_EQUAL("or(or(True, False, True), False)", another_or.to_string(tagset));
    v2->push_back(true_constant);
	BOOST_CHECK_EQUAL("or(or(True, False, True), False, True)", another_or.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(or_to_raw_string, PredFix)
{
    boost::shared_ptr<Or::BoolFunctionPtrVector> v(new Or::BoolFunctionPtrVector());
    v->push_back(false_constant);
    boost::shared_ptr<Function<Bool> > pred_or(new Or(v));
	BOOST_CHECK_EQUAL("or(False)", pred_or->to_raw_string());
    v->push_back(true_constant);
	BOOST_CHECK_EQUAL("or(False, True)", pred_or->to_raw_string());
    v->push_back(true_constant);
	BOOST_CHECK_EQUAL("or(False, True, True)", pred_or->to_raw_string());

    boost::shared_ptr<Or::BoolFunctionPtrVector> v2(new Or::BoolFunctionPtrVector());
    v2->push_back(true_constant);
    v2->push_back(pred_or);
    Or another_or(v2);
	BOOST_CHECK_EQUAL("or(True, or(False, True, True))", another_or.to_raw_string());
}

BOOST_FIXTURE_TEST_CASE(nor_to_string, PredFix)
{
    boost::shared_ptr<Nor::BoolFunctionPtrVector> v(new Nor::BoolFunctionPtrVector());
    v->push_back(true_constant);
    boost::shared_ptr<Function<Bool> > pred_nor(new Nor(v));
	BOOST_CHECK_EQUAL("not(True)", pred_nor->to_string(tagset));
    v->push_back(false_constant);
	BOOST_CHECK_EQUAL("not(True, False)", pred_nor->to_string(tagset));
    v->push_back(true_constant);
	BOOST_CHECK_EQUAL("not(True, False, True)", pred_nor->to_string(tagset));

    boost::shared_ptr<Or::BoolFunctionPtrVector> v2(new Nor::BoolFunctionPtrVector());
    v2->push_back(pred_nor);
    v2->push_back(false_constant);
    Nor another_nor(v2);
	BOOST_CHECK_EQUAL("not(not(True, False, True), False)", another_nor.to_string(tagset));
    v2->push_back(true_constant);
	BOOST_CHECK_EQUAL("not(not(True, False, True), False, True)", another_nor.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(nor_to_raw_string, PredFix)
{
    boost::shared_ptr<Or::BoolFunctionPtrVector> v(new Nor::BoolFunctionPtrVector());
    v->push_back(false_constant);
    boost::shared_ptr<Function<Bool> > pred_nor(new Nor(v));
	BOOST_CHECK_EQUAL("not(False)", pred_nor->to_raw_string());
    v->push_back(true_constant);
	BOOST_CHECK_EQUAL("not(False, True)", pred_nor->to_raw_string());
    v->push_back(true_constant);
	BOOST_CHECK_EQUAL("not(False, True, True)", pred_nor->to_raw_string());

    boost::shared_ptr<Nor::BoolFunctionPtrVector> v2(new Nor::BoolFunctionPtrVector());
    v2->push_back(true_constant);
    v2->push_back(pred_nor);
    Nor another_nor(v2);
	BOOST_CHECK_EQUAL("not(True, not(False, True, True))", another_nor.to_raw_string());
}

BOOST_AUTO_TEST_SUITE_END()
