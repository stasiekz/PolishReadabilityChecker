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


#include <libwccl/values/strset.h>
#include <libwccl/sentencecontext.h>
#include <libwccl/ops/functions/strset/tolower.h>
#include <libwccl/ops/functions/strset/toupper.h>
#include <libwccl/ops/functions/strset/affix.h>
#include <libwccl/ops/functions/constant.h>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(strset_functions)

struct StrSetFix
{
	StrSetFix()
		: sc(boost::make_shared<Corpus2::Sentence>()),
		  tagset(),
		  cx(sc, boost::make_shared<Variables>()),
		  strset(),
		  strset_expr()
	{
		strset.insert("alllower");
		strset.insert("Firstcapital");
		strset.insert("PascalCase");
		strset.insert("camelCase");
		strset.insert("some1325numbers");
		strset.insert("ALLUPPER");
		strset.insert("kIdSpEeChLoL");
		strset.insert("short");

		strset_expr.reset(new Constant<StrSet>(strset));
	}
	SentenceContext sc;
	Corpus2::Tagset tagset;
	FunExecContext cx;

	StrSet strset;
	boost::shared_ptr<Function<StrSet> > strset_expr;
};

BOOST_FIXTURE_TEST_CASE(lower, StrSetFix)
{
	StrSet lowerset;
	lowerset.insert("alllower");
	lowerset.insert("firstcapital");
	lowerset.insert("pascalcase");
	lowerset.insert("camelcase");
	lowerset.insert("some1325numbers");
	lowerset.insert("allupper");
	lowerset.insert("kidspeechlol");
	lowerset.insert("short");

	ToLower to_lower(strset_expr);

	BOOST_CHECK(lowerset.equals(*to_lower.apply(cx)));
}

BOOST_FIXTURE_TEST_CASE(upper, StrSetFix)
{
	StrSet upperset;
	upperset.insert("ALLLOWER");
	upperset.insert("FIRSTCAPITAL");
	upperset.insert("PASCALCASE");
	upperset.insert("CAMELCASE");
	upperset.insert("SOME1325NUMBERS");
	upperset.insert("ALLUPPER");
	upperset.insert("KIDSPEECHLOL");
	upperset.insert("SHORT");

	ToUpper to_upper(strset_expr);

	BOOST_CHECK(upperset.equals(*to_upper.apply(cx)));
}

BOOST_FIXTURE_TEST_CASE(prefix, StrSetFix)
{
    StrSet prefixset;
    prefixset.insert("alllowe");
    prefixset.insert("Firstca");
    prefixset.insert("PascalC");
    prefixset.insert("camelCa");
    prefixset.insert("some132");
    prefixset.insert("ALLUPPE");
    prefixset.insert("kIdSpEe");
    prefixset.insert("short");

    Affix prefix(strset_expr, 7);

	BOOST_CHECK(prefixset.equals(*prefix.apply(cx)));
}

BOOST_FIXTURE_TEST_CASE(suffix, StrSetFix)
{
    StrSet suffixset;
    suffixset.insert("lllower");
    suffixset.insert("capital");
    suffixset.insert("calCase");
    suffixset.insert("melCase");
    suffixset.insert("numbers");
    suffixset.insert("LLUPPER");
    suffixset.insert("EeChLoL");
    suffixset.insert("short");

    Affix suffix(strset_expr, -7);

	BOOST_CHECK(suffixset.equals(*suffix.apply(cx)));
}

BOOST_FIXTURE_TEST_CASE(affix_0, StrSetFix)
{
	Affix affix_0(strset_expr, 0);
	BOOST_CHECK(strset.equals(*affix_0.apply(cx)));
}

BOOST_FIXTURE_TEST_CASE(lower_locale, StrSetFix)
{
	//I'm not sure if I can guarantee this test will pass
	//on all locales? - ToLower uses default locale at the moment

	//I wanted to make sure switching around encoding of source file
	//won't affect the test, so I explicitly provide escaped UTF8 sequence

	StrSet upperset;
	upperset.insert(UnicodeString::fromUTF8(
		"za\xC5\xBB\xC3\x93\xC5\x81\xC4\x86g\xC4\x98\xC5\x9AL\xC4\x84ja\xC5\xB9\xC5\x83"
		"zA\xC5\xBC\xC3\xB3\xC5\x82\xC4\x87g\xC4\x99\xC5\x9Bl\xC4\x85ja\xC5\xBA\xC5\x84"));
	StrSet lowerset;
	lowerset.insert(UnicodeString::fromUTF8(
		"za\xC5\xBC\xC3\xB3\xC5\x82\xC4\x87g\xC4\x99\xC5\x9Bl\xC4\x85ja\xC5\xBA\xC5\x84"
		"za\xC5\xBC\xC3\xB3\xC5\x82\xC4\x87g\xC4\x99\xC5\x9Bl\xC4\x85ja\xC5\xBA\xC5\x84"));

	ToLower to_lower(boost::shared_ptr<Function<StrSet> >(
		new Constant<StrSet>(upperset)));

	BOOST_CHECK(lowerset.equals(*to_lower.apply(cx)));
}

BOOST_FIXTURE_TEST_CASE(upper_locale, StrSetFix)
{
	//I'm not sure if I can guarantee this test will pass
	//on all locales? - ToUpper uses default locale at the moment

	//I wanted to make sure switching around encoding of source file
	//won't affect the test, so I explicitly provide escaped UTF8 sequence

	StrSet upperset;
	upperset.insert(UnicodeString::fromUTF8(
		"ZA\xC5\xBB\xC3\x93\xC5\x81\xC4\x86G\xC4\x98\xC5\x9AL\xC4\x84JA\xC5\xB9\xC5\x83"
		"ZA\xC5\xBB\xC3\x93\xC5\x81\xC4\x86G\xC4\x98\xC5\x9AL\xC4\x84JA\xC5\xB9\xC5\x83"));
	StrSet lowerset;
	lowerset.insert(UnicodeString::fromUTF8(
		"za\xC5\xBC\xC3\xB3\xC5\x82\xC4\x87g\xC4\x99\xC5\x9Bl\xC4\x85ja\xC5\xBA\xC5\x84"
		"ZA\xC5\xBB\xC3\x93\xC5\x81\xC4\x86g\xC4\x98\xC5\x9AL\xC4\x84JA\xC5\xB9\xC5\x83"));

	ToUpper to_upper(boost::shared_ptr<Function<StrSet> >(
		new Constant<StrSet>(lowerset)));

	BOOST_CHECK(upperset.equals(*to_upper.apply(cx)));
}

BOOST_FIXTURE_TEST_CASE(lower_empty, StrSetFix)
{
	StrSet emptyset;

	ToLower to_lower(boost::shared_ptr<Function<StrSet> >(
		new Constant<StrSet>(emptyset)));

	BOOST_CHECK(emptyset.equals(*to_lower.apply(cx)));
}

BOOST_FIXTURE_TEST_CASE(upper_empty, StrSetFix)
{
	StrSet emptyset;

	ToUpper to_upper(boost::shared_ptr<Function<StrSet> >(
		new Constant<StrSet>(emptyset)));

	BOOST_CHECK(emptyset.equals(*to_upper.apply(cx)));
}

//------ to_string test cases -------

BOOST_FIXTURE_TEST_CASE(lower_to_string, StrSetFix)
{
	StrSet one_elem_set;
	one_elem_set.insert("YayaAy");
	ToLower to_lower(boost::shared_ptr<Function<StrSet> >(
			new Constant<StrSet>(one_elem_set)));
	std::string expected = "lower([\"YayaAy\"])";
	BOOST_CHECK_EQUAL(expected, to_lower.to_string(tagset));
}

BOOST_AUTO_TEST_CASE(lower_to_raw_string)
{
	StrSet one_elem_set;
	one_elem_set.insert("YayaAy");
	ToLower to_lower(boost::shared_ptr<Function<StrSet> >(
			new Constant<StrSet>(one_elem_set)));
	std::string expected = "lower([\"YayaAy\"])";
	BOOST_CHECK_EQUAL(expected, to_lower.to_raw_string());
}

BOOST_FIXTURE_TEST_CASE(upper_to_string, StrSetFix)
{
	StrSet one_elem_set;
	one_elem_set.insert("YayaAy");
	ToUpper to_upper(boost::shared_ptr<Function<StrSet> >(
			new Constant<StrSet>(one_elem_set)));
	std::string expected = "upper([\"YayaAy\"])";
	BOOST_CHECK_EQUAL(expected, to_upper.to_string(tagset));
}

BOOST_AUTO_TEST_CASE(upper_to_raw_string)
{
	StrSet one_elem_set;
	one_elem_set.insert("YayaAy");
	ToUpper to_upper(boost::shared_ptr<Function<StrSet> >(
			new Constant<StrSet>(one_elem_set)));
	std::string expected = "upper([\"YayaAy\"])";
	BOOST_CHECK_EQUAL(expected, to_upper.to_raw_string());
}

BOOST_AUTO_TEST_SUITE_END()
