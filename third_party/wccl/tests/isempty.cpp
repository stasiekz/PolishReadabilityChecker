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

#include <libwccl/ops/functions/bool/predicates/isempty.h>
#include <libwccl/sentencecontext.h>
#include <libwccl/values/match.h>
#include <libwccl/values/strset.h>
#include <libwccl/values/tset.h>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(isstuffempty)

struct IsEmptyFix
{
	IsEmptyFix()
		: sc(boost::make_shared<Corpus2::AnnotatedSentence>()),
		  tagset(),
		  cx(sc, boost::make_shared<Variables>())
	{
	}
	SentenceContext sc;
	Corpus2::Tagset tagset;
	FunExecContext cx;
};

BOOST_FIXTURE_TEST_CASE(empty_strset, IsEmptyFix)
{
	StrSet empty_set;
	boost::shared_ptr<Function<StrSet> > empty_set_expr(new Constant<StrSet>(empty_set));
	IsEmpty<StrSet> e(empty_set_expr);
	BOOST_CHECK(e.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(full_strset, IsEmptyFix)
{
	StrSet full_set;
	full_set.insert("word");
	boost::shared_ptr<Function<StrSet> > full_set_expr(new Constant<StrSet>(full_set));
	IsEmpty<StrSet> e(full_set_expr);
	BOOST_CHECK(!e.apply(cx)->get_value());
}


BOOST_FIXTURE_TEST_CASE(empty_tset, IsEmptyFix)
{
	TSet empty_set;
	boost::shared_ptr<Function<TSet> > empty_set_expr(new Constant<TSet>(empty_set));
	IsEmpty<TSet> e(empty_set_expr);
	BOOST_CHECK(e.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(full_tset, IsEmptyFix)
{
	TSet full_set(Corpus2::Tag(1));
	boost::shared_ptr<Function<TSet> > full_set_expr(new Constant<TSet>(full_set));
	IsEmpty<TSet> e(full_set_expr);
	BOOST_CHECK(!e.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(default_match, IsEmptyFix)
{
	Match default_match;
	boost::shared_ptr<Function<Match> > default_match_expr(new Constant<Match>(default_match));
	IsEmpty<Match> e(default_match_expr);
	BOOST_CHECK(e.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(token_match, IsEmptyFix)
{
	Match token_match(TokenMatch(0));
	boost::shared_ptr<Function<Match> > match_expr(new Constant<Match>(token_match));
	IsEmpty<Match> e(match_expr);
	BOOST_CHECK(!e.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(ann_match, IsEmptyFix)
{
	Match ann_match(AnnotationMatch(0, "foo"));
	boost::shared_ptr<Function<Match> > match_expr(new Constant<Match>(ann_match));
	IsEmpty<Match> e(match_expr);
	BOOST_CHECK(!e.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(empty_matchvector, IsEmptyFix)
{
	MatchVector v;
	Match v_match(v);
	boost::shared_ptr<Function<Match> > match_expr(new Constant<Match>(v_match));
	IsEmpty<Match> e(match_expr);
	BOOST_CHECK(e.apply(cx)->get_value());
}

BOOST_FIXTURE_TEST_CASE(full_matchvector, IsEmptyFix)
{
	MatchVector v;
	v.append(boost::shared_ptr<TokenMatch>(new TokenMatch(0)));
	Match v_match(v);
	boost::shared_ptr<Function<Match> > match_expr(new Constant<Match>(v_match));
	IsEmpty<Match> e(match_expr);
	BOOST_CHECK(!e.apply(cx)->get_value());
}

//------------ To string ----------

BOOST_FIXTURE_TEST_CASE(empty_strset_tostring, IsEmptyFix)
{
	StrSet full_set;
	full_set.insert("word");
	boost::shared_ptr<Function<StrSet> > full_set_expr(new Constant<StrSet>(full_set));
	IsEmpty<StrSet> e(full_set_expr);
	BOOST_CHECK_EQUAL("empty([\"word\"])", e.to_string(tagset));
}

BOOST_AUTO_TEST_CASE(empty_strset_tostring_to_raw_string)
{
	StrSet full_set;
	full_set.insert("word");
	boost::shared_ptr<Function<StrSet> > full_set_expr(new Constant<StrSet>(full_set));
	IsEmpty<StrSet> e(full_set_expr);
	BOOST_CHECK_EQUAL("empty([\"word\"])", e.to_raw_string());
}

BOOST_AUTO_TEST_SUITE_END()
