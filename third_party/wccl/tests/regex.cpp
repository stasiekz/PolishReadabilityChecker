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
#include <libwccl/ops/functions/bool/predicates/regex.h>
#include <libwccl/values/bool.h>
#include <libwccl/values/strset.h>
#include <libwccl/sentencecontext.h>
#include <unicode/unistr.h>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(regexing)

struct RegexFix
{
	RegexFix()
		: sc(boost::make_shared<Corpus2::Sentence>()),
		  tagset(),
		  cx(sc, boost::make_shared<Variables>())
	{
	}
	SentenceContext sc;
	Corpus2::Tagset tagset;
	FunExecContext cx;
};

BOOST_FIXTURE_TEST_CASE(positive_sanity_check, RegexFix)
{
	StrSet sanity;
	sanity.insert("word");
	boost::shared_ptr<Function<StrSet> > sanity_expr(new Constant<StrSet>(sanity));
	Regex r(sanity_expr, "word");
	BOOST_CHECK(r.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(negative_sanity_check, RegexFix)
{
	StrSet sanity;
	sanity.insert("word");
	boost::shared_ptr<Function<StrSet> > sanity_expr(new Constant<StrSet>(sanity));
	Regex r(sanity_expr, "Word");
	BOOST_CHECK(!r.apply(cx)->get_value());
}

//TODO: need more regex tests...

//------------ To string ----------

BOOST_FIXTURE_TEST_CASE(regex_tostring, RegexFix)
{
	StrSet sanity;
	sanity.insert("word");
	boost::shared_ptr<Function<StrSet> > sanity_expr(new Constant<StrSet>(sanity));
	Regex r(sanity_expr, "Word");
	BOOST_CHECK_EQUAL("regex([\"word\"], \"Word\")", r.to_string(tagset));
}

BOOST_AUTO_TEST_CASE(regex_to_raw_string)
{
	StrSet sanity;
	sanity.insert("word");
	boost::shared_ptr<Function<StrSet> > sanity_expr(new Constant<StrSet>(sanity));
	Regex r(sanity_expr, "Word");
	BOOST_CHECK_EQUAL("regex([\"word\"], \"Word\")", r.to_raw_string());
}

BOOST_AUTO_TEST_SUITE_END()
