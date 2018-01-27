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
#include <libcorpus2/ann/annotatedsentence.h>
#include <libwccl/ops/functions/constant.h>

#include <libwccl/ops/tagactions/mark.h>

#include "optest.h"

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(mark)

struct MarkFix : public Wccl::PositionFixture
{
	MarkFix()
		: Wccl::PositionFixture(),
		as(boost::make_shared<Corpus2::AnnotatedSentence>()),
		sc(as),
		cx(sc, boost::make_shared<Variables>())
	{
		as->append(new Corpus2::Token(UnicodeString::fromUTF8("t1"), PwrNlp::Whitespace::Newline));
		as->append(new Corpus2::Token(UnicodeString::fromUTF8("t2"), PwrNlp::Whitespace::Newline));
		as->append(new Corpus2::Token(UnicodeString::fromUTF8("t3"), PwrNlp::Whitespace::Newline));
		as->append(new Corpus2::Token(UnicodeString::fromUTF8("t4"), PwrNlp::Whitespace::Newline));
		as->append(new Corpus2::Token(UnicodeString::fromUTF8("t5"), PwrNlp::Whitespace::Newline));
		as->create_channel("ch1");
		as->get_channel("ch1").set_segment_at(2, 1);
		as->get_channel("ch1").set_segment_at(3, 1);
		as->get_channel("ch1").set_head_at(3, true);
	}

	boost::shared_ptr<Corpus2::AnnotatedSentence> as;
	SentenceContext sc;
	Corpus2::Tagset tagset;
	ActionExecContext cx;
};

BOOST_FIXTURE_TEST_CASE(mark_empty, MarkFix)
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> as_clone = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(as->clone_shared());
	Mark mark(pos(-1), pos(-1), "ch1");
	BOOST_CHECK(!mark.execute(cx));
	//BOOST_CHECK((*as) == (*as_clone));
	BOOST_CHECK_EQUAL(as->get_channel("ch1").dump_alpha(), "__aA_");
}

BOOST_FIXTURE_TEST_CASE(mark_already_there, MarkFix)
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> as_clone = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(as->clone_shared());
	sc.set_position(1);
	Mark mark(begin(), pos(1), "ch1");
	BOOST_CHECK(!mark.execute(cx));
	BOOST_CHECK_EQUAL(as->get_channel("ch1").dump_alpha(), "__aA_");
}

BOOST_FIXTURE_TEST_CASE(mark_begin, MarkFix)
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> as_clone = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(as->clone_shared());
	sc.set_position(1);
	Mark mark(begin(), pos(-1), "ch1");
	BOOST_CHECK(mark.execute(cx));
	BOOST_CHECK_EQUAL(as->get_channel("ch1").dump_alpha(), "B_aA_");
}

BOOST_FIXTURE_TEST_CASE(mark_begin_head0, MarkFix)
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> as_clone = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(as->clone_shared());
	sc.set_position(0);
	Mark mark(begin(), pos(1), pos(0), "ch1");
	BOOST_CHECK(mark.execute(cx));
	BOOST_CHECK_EQUAL(as->get_channel("ch1").dump_alpha(), "BbaA_");
}

BOOST_FIXTURE_TEST_CASE(mark_begin_head1, MarkFix)
{
	boost::shared_ptr<Corpus2::AnnotatedSentence> as_clone = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(as->clone_shared());
	sc.set_position(0);
	Mark mark(begin(), pos(1), pos(1), "ch1");
	BOOST_CHECK(mark.execute(cx));
	BOOST_CHECK_EQUAL(as->get_channel("ch1").dump_alpha(), "bBaA_");
}

BOOST_FIXTURE_TEST_CASE(mark_other, MarkFix)
{
	sc.set_position(1);
	Mark mark(begin(), pos(1), begin(), "ch2");
	BOOST_CHECK(mark.execute(cx));
	BOOST_CHECK_EQUAL(as->get_channel("ch2").dump_alpha(), "Aaa__");
}


//------ to_string test cases -------

BOOST_FIXTURE_TEST_CASE(mark_to_string, MarkFix)
{
	Mark mark(begin(), end(), "ch");
	BOOST_CHECK_EQUAL("mark(begin, end, \"ch\")", mark.to_string(tagset));
	Mark mark2(begin(), pos(1), "ch2");
	BOOST_CHECK_EQUAL("mark(begin, 1, \"ch2\")", mark2.to_string(tagset));
	Mark mark3(pos(-1), end(), "ch3");
	BOOST_CHECK_EQUAL("mark(-1, end, \"ch3\")", mark3.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(mark_to_string_head, MarkFix)
	{
	Mark mark(begin(), end(), pos(1), "ch");
	BOOST_CHECK_EQUAL("mark(begin, end, 1, \"ch\")", mark.to_string(tagset));
	Mark mark2(begin(), pos(1), pos(0), "ch2");
	BOOST_CHECK_EQUAL("mark(begin, 1, 0, \"ch2\")", mark2.to_string(tagset));
	Mark mark3(pos(-1), end(), end(), "ch3");
	BOOST_CHECK_EQUAL("mark(-1, end, end, \"ch3\")", mark3.to_string(tagset));
}

BOOST_AUTO_TEST_SUITE_END()
