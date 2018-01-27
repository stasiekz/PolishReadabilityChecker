/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Lesser Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.

    See the LICENSE.CORPUS2, POLIQARP, COPYING.LESSER and COPYING files for more details.
*/

#include <boost/test/unit_test.hpp>
#include <boost/make_shared.hpp>
#include <libcorpus2/ann/annotatedsentence.h>
#include <libcorpus2/io/xcesreader.h>
#include <libcorpus2/tagsetmanager.h>

BOOST_AUTO_TEST_SUITE(ann)

BOOST_AUTO_TEST_CASE( wrap )
{
	Corpus2::Sentence::Ptr s = boost::make_shared<Corpus2::Sentence>();
	s->append(new Corpus2::Token(UnicodeString::fromUTF8("orth"), PwrNlp::Whitespace::None));
	Corpus2::Sentence::Ptr a = Corpus2::AnnotatedSentence::wrap_sentence(s);
	BOOST_CHECK(s->empty());
	BOOST_REQUIRE(!a->empty());
	BOOST_CHECK_EQUAL(a->tokens()[0]->orth_utf8(), "orth");
}

BOOST_AUTO_TEST_CASE( wrap_clone )
{
	Corpus2::Sentence::Ptr s = boost::make_shared<Corpus2::Sentence>();
	s->append(new Corpus2::Token(UnicodeString::fromUTF8("orth"), PwrNlp::Whitespace::None));
	Corpus2::Sentence::Ptr a = Corpus2::AnnotatedSentence::wrap_sentence_clone(s);
	BOOST_REQUIRE(!s->empty());
	s->tokens()[0]->set_orth(UnicodeString::fromUTF8("aaa"));
	BOOST_REQUIRE(!a->empty());
	BOOST_CHECK_EQUAL(a->tokens()[0]->orth_utf8(), "orth");
}


BOOST_AUTO_TEST_CASE( empty_chan )
{
	Corpus2::AnnotationChannel ch(10);
	BOOST_REQUIRE(ch.segments().size() == 10);
	for (size_t i = 0; i < 10; ++i) {
		BOOST_CHECK(ch.get_iob_at(i) == Corpus2::IOB::O);
		BOOST_CHECK(ch.segments()[i] == 0);
	}
}

BOOST_AUTO_TEST_CASE( chan_iob_outofrange )
{
	Corpus2::AnnotationChannel ch(10);
	BOOST_CHECK(ch.get_iob_at(-1) == Corpus2::IOB::O);
	BOOST_CHECK(ch.get_iob_at(11) == Corpus2::IOB::O);
	BOOST_CHECK_NO_THROW(ch.set_iob_at(-1, Corpus2::IOB::B));
}

BOOST_AUTO_TEST_CASE( chan_iob_to_seg )
{
	Corpus2::AnnotationChannel ch(10);
	// 0123456
	// BOBBIBO
	// 1023340
	ch.set_iob_at(0, Corpus2::IOB::B);
	ch.set_iob_at(2, Corpus2::IOB::B);
	ch.set_iob_at(3, Corpus2::IOB::B);
	ch.set_iob_at(4, Corpus2::IOB::I);
	ch.set_iob_at(5, Corpus2::IOB::B);
	ch.make_segments_from_iob();
	BOOST_CHECK_EQUAL(ch.segments()[0], 1);
	BOOST_CHECK_EQUAL(ch.segments()[1], 0);
	BOOST_CHECK_EQUAL(ch.segments()[2], 2);
	BOOST_CHECK_EQUAL(ch.segments()[3], 3);
	BOOST_CHECK_EQUAL(ch.segments()[4], 3);
	BOOST_CHECK_EQUAL(ch.segments()[5], 4);
	BOOST_CHECK_EQUAL(ch.segments()[6], 0);
	ch.make_iob_from_segments();
	BOOST_CHECK_EQUAL(ch.get_iob_at(0), Corpus2::IOB::B);
	BOOST_CHECK_EQUAL(ch.get_iob_at(1), Corpus2::IOB::O);
	BOOST_CHECK_EQUAL(ch.get_iob_at(2), Corpus2::IOB::B);
	BOOST_CHECK_EQUAL(ch.get_iob_at(3), Corpus2::IOB::B);
	BOOST_CHECK_EQUAL(ch.get_iob_at(4), Corpus2::IOB::I);
	BOOST_CHECK_EQUAL(ch.get_iob_at(5), Corpus2::IOB::B);
	BOOST_CHECK_EQUAL(ch.get_iob_at(6), Corpus2::IOB::O);
}

BOOST_AUTO_TEST_CASE( make_ann )
{
	Corpus2::AnnotationChannel ch(10);
	// 0123456
	// BOBBIBO
	// 1023340
	ch.set_iob_at(0, Corpus2::IOB::B);
	ch.set_iob_at(2, Corpus2::IOB::B);
	ch.set_iob_at(3, Corpus2::IOB::B);
	ch.set_iob_at(4, Corpus2::IOB::I);
	ch.set_iob_at(5, Corpus2::IOB::B);
	ch.make_segments_from_iob();
	std::vector<Corpus2::Annotation> anns = ch.make_annotation_vector();
	BOOST_REQUIRE_EQUAL(anns.size(), 4);
	BOOST_FOREACH(const Corpus2::Annotation& a, anns) {
		BOOST_REQUIRE(!a.empty());
		BOOST_REQUIRE(a.sane());
	}
	BOOST_REQUIRE_EQUAL(anns[0].indices.size(), 1);
	BOOST_REQUIRE_EQUAL(anns[1].indices.size(), 1);
	BOOST_REQUIRE_EQUAL(anns[2].indices.size(), 2);
	BOOST_REQUIRE_EQUAL(anns[3].indices.size(), 1);
	BOOST_CHECK_EQUAL(anns[0].indices[0], 0);
	BOOST_CHECK_EQUAL(anns[1].indices[0], 2);
	BOOST_CHECK_EQUAL(anns[2].indices[0], 3);
	BOOST_CHECK_EQUAL(anns[2].indices[1], 4);
	BOOST_CHECK_EQUAL(anns[3].indices[0], 5);
	BOOST_CHECK_EQUAL(anns[2].head_index, 3);
	BOOST_CHECK_EQUAL(anns[0].seg_number, 1);
	BOOST_CHECK_EQUAL(anns[1].seg_number, 2);
	BOOST_CHECK_EQUAL(anns[2].seg_number, 3);
	BOOST_CHECK_EQUAL(anns[3].seg_number, 4);
}


BOOST_AUTO_TEST_CASE( split_ann )
{
	//012345
	//0BBBB0
	//012120
	Corpus2::AnnotationChannel ch(10);
	ch.segments()[1] = 1;
	ch.segments()[2] = 2;
	ch.segments()[3] = 1;
	ch.segments()[4] = 2;
	ch.set_head_at(2, true);
	ch.set_head_at(3, true);
	ch.make_iob_from_segments();
	BOOST_CHECK_EQUAL(ch.get_iob_at(0), Corpus2::IOB::O);
	BOOST_CHECK_EQUAL(ch.get_iob_at(1), Corpus2::IOB::B);
	BOOST_CHECK_EQUAL(ch.get_iob_at(2), Corpus2::IOB::B);
	BOOST_CHECK_EQUAL(ch.get_iob_at(3), Corpus2::IOB::B);
	BOOST_CHECK_EQUAL(ch.get_iob_at(4), Corpus2::IOB::B);
	BOOST_CHECK_EQUAL(ch.get_iob_at(5), Corpus2::IOB::O);
	std::vector<Corpus2::Annotation> anns = ch.make_annotation_vector();
	BOOST_REQUIRE_EQUAL(anns.size(), 2);
	BOOST_FOREACH(const Corpus2::Annotation& a, anns) {
		BOOST_REQUIRE(!a.empty());
		BOOST_REQUIRE(a.sane());
	}
	BOOST_REQUIRE_EQUAL(anns[0].indices.size(), 2);
	BOOST_REQUIRE_EQUAL(anns[1].indices.size(), 2);
	BOOST_CHECK_EQUAL(anns[0].indices[0], 2);
	BOOST_CHECK_EQUAL(anns[0].indices[1], 4);
	BOOST_CHECK_EQUAL(anns[0].head_index, 2);
	BOOST_CHECK_EQUAL(anns[1].indices[0], 1);
	BOOST_CHECK_EQUAL(anns[1].indices[1], 3);
	BOOST_CHECK_EQUAL(anns[1].head_index, 3);
}

namespace {
static char swiatopoglad[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">"
"<cesAna version=\"1.0\" type=\"lex disamb\">"
"<chunkList>"
"<chunk id=\"ch51\" type=\"tok\">"
"<chunk type=\"s\">"
"<tok>"
"<orth>Uważam</orth>"
"<lex disamb=\"1\"><base>uważać</base><ctag>fin:sg:pri:imperf</ctag></lex>"
"</tok>"
"<ns/>"
"<tok>"
"<orth>,</orth>"
"<lex disamb=\"1\"><base>,</base><ctag>interp</ctag></lex>"
"</tok>"
"<tok>"
"<orth>że</orth>"
"<lex disamb=\"1\"><base>że</base><ctag>conj</ctag></lex>"
"</tok>"
"<tok>"
"<orth>światopogląd</orth>"
"<lex><base>światopogląd</base><ctag>subst:sg:acc:m3</ctag></lex>"
"<lex disamb=\"1\"><base>światopogląd</base><ctag>subst:sg:nom:m3</ctag></lex>"
"</tok>"
"<tok>"
"<orth>może</orth>"
"<lex><base>może</base><ctag>qub</ctag></lex>"
"<lex disamb=\"1\"><base>móc</base><ctag>fin:sg:ter:imperf</ctag></lex>"
"</tok>"
"<tok>"
"<orth>być</orth>"
"<lex disamb=\"1\"><base>być</base><ctag>inf:imperf</ctag></lex>"
"</tok>"
"<tok>"
"<orth>materialistyczny</orth>"
"<lex><base>materialistyczny</base><ctag>adj:sg:acc:m3:pos</ctag></lex>"
"<lex><base>materialistyczny</base><ctag>adj:sg:nom:m1:pos</ctag></lex>"
"<lex><base>materialistyczny</base><ctag>adj:sg:nom:m2:pos</ctag></lex>"
"<lex disamb=\"1\"><base>materialistyczny</base><ctag>adj:sg:nom:m3:pos</ctag></lex>"
"</tok>"
"<tok>"
"<orth>lub</orth>"
"<lex disamb=\"1\"><base>lub</base><ctag>conj</ctag></lex>"
"<lex><base>lubić</base><ctag>impt:sg:sec:imperf</ctag></lex>"
"</tok>"
"<tok>"
"<orth>idealistyczny</orth>"
"<lex><base>idealistyczny</base><ctag>adj:sg:acc:m3:pos</ctag></lex>"
"<lex><base>idealistyczny</base><ctag>adj:sg:nom:m1:pos</ctag></lex>"
"<lex><base>idealistyczny</base><ctag>adj:sg:nom:m2:pos</ctag></lex>"
"<lex disamb=\"1\"><base>idealistyczny</base><ctag>adj:sg:nom:m3:pos</ctag></lex>"
"</tok>"
"<tok>"
"<orth>(</orth>"
"<lex disamb=\"1\"><base>(</base><ctag>interp</ctag></lex>"
"</tok>"
"<ns/>"
"<tok>"
"<orth>w</orth>"
"<lex><base>w</base><ctag>prep:acc:nwok</ctag></lex>"
"<lex disamb=\"1\"><base>w</base><ctag>prep:loc:nwok</ctag></lex>"
"</tok>"
"<tok>"
"<orth>prymitywnych</orth>"
"<lex><base>prymitywny</base><ctag>adj:pl:acc:m1:pos</ctag></lex>"
"<lex><base>prymitywny</base><ctag>adj:pl:gen:f:pos</ctag></lex>"
"<lex><base>prymitywny</base><ctag>adj:pl:gen:m1:pos</ctag></lex>"
"<lex><base>prymitywny</base><ctag>adj:pl:gen:m2:pos</ctag></lex>"
"<lex><base>prymitywny</base><ctag>adj:pl:gen:m3:pos</ctag></lex>"
"<lex><base>prymitywny</base><ctag>adj:pl:gen:n:pos</ctag></lex>"
"<lex disamb=\"1\"><base>prymitywny</base><ctag>adj:pl:loc:f:pos</ctag></lex>"
"<lex><base>prymitywny</base><ctag>adj:pl:loc:m1:pos</ctag></lex>"
"<lex><base>prymitywny</base><ctag>adj:pl:loc:m2:pos</ctag></lex>"
"<lex><base>prymitywny</base><ctag>adj:pl:loc:m3:pos</ctag></lex>"
"<lex><base>prymitywny</base><ctag>adj:pl:loc:n:pos</ctag></lex>"
"</tok>"
"<tok>"
"<orth>cywilizacjach</orth>"
"<lex disamb=\"1\"><base>cywilizacja</base><ctag>subst:pl:loc:f</ctag></lex>"
"</tok>"
"<tok>"
"<orth>nawet</orth>"
"<lex disamb=\"1\"><base>nawet</base><ctag>qub</ctag></lex>"
"</tok>"
"<tok>"
"<orth>magiczny</orth>"
"<lex><base>magiczny</base><ctag>adj:sg:acc:m3:pos</ctag></lex>"
"<lex><base>magiczny</base><ctag>adj:sg:nom:m1:pos</ctag></lex>"
"<lex><base>magiczny</base><ctag>adj:sg:nom:m2:pos</ctag></lex>"
"<lex disamb=\"1\"><base>magiczny</base><ctag>adj:sg:nom:m3:pos</ctag></lex>"
"</tok>"
"<ns/>"
"<tok>"
"<orth>)</orth>"
"<lex disamb=\"1\"><base>)</base><ctag>interp</ctag></lex>"
"</tok>"
"<ns/>"
"<tok>"
"<orth>,</orth>"
"<lex disamb=\"1\"><base>,</base><ctag>interp</ctag></lex>"
"</tok>"
"<tok>"
"<orth>ale</orth>"
"<lex disamb=\"1\"><base>ale</base><ctag>conj</ctag></lex>"
"</tok>"
"<tok>"
"<orth>nie</orth>"
"<lex disamb=\"1\"><base>nie</base><ctag>qub</ctag></lex>"
"<lex><base>on</base><ctag>ppron3:pl:acc:f:ter:praep</ctag></lex>"
"<lex><base>on</base><ctag>ppron3:pl:acc:m2:ter:praep</ctag></lex>"
"<lex><base>on</base><ctag>ppron3:pl:acc:m3:ter:praep</ctag></lex>"
"<lex><base>on</base><ctag>ppron3:pl:acc:n:ter:praep</ctag></lex>"
"<lex><base>on</base><ctag>ppron3:sg:acc:n:ter:praep</ctag></lex>"
"</tok>"
"<tok>"
"<orth>morski</orth>"
"<lex><base>morski</base><ctag>adj:sg:acc:m3:pos</ctag></lex>"
"<lex><base>morski</base><ctag>adj:sg:nom:m1:pos</ctag></lex>"
"<lex><base>morski</base><ctag>adj:sg:nom:m2:pos</ctag></lex>"
"<lex disamb=\"1\"><base>morski</base><ctag>adj:sg:nom:m3:pos</ctag></lex>"
"</tok>"
"<ns/>"
"<tok>"
"<orth>,</orth>"
"<lex disamb=\"1\"><base>,</base><ctag>interp</ctag></lex>"
"</tok>"
"<tok>"
"<orth>lotniczy</orth>"
"<lex><base>lotniczy</base><ctag>adj:pl:nom:m1:pos</ctag></lex>"
"<lex><base>lotniczy</base><ctag>adj:sg:acc:m3:pos</ctag></lex>"
"<lex><base>lotniczy</base><ctag>adj:sg:nom:m1:pos</ctag></lex>"
"<lex><base>lotniczy</base><ctag>adj:sg:nom:m2:pos</ctag></lex>"
"<lex disamb=\"1\"><base>lotniczy</base><ctag>adj:sg:nom:m3:pos</ctag></lex>"
"</tok>"
"<ns/>"
"<tok>"
"<orth>,</orth>"
"<lex disamb=\"1\"><base>,</base><ctag>interp</ctag></lex>"
"</tok>"
"<tok>"
"<orth>rolny</orth>"
"<lex><base>rolny</base><ctag>adj:sg:acc:m3:pos</ctag></lex>"
"<lex><base>rolny</base><ctag>adj:sg:nom:m1:pos</ctag></lex>"
"<lex><base>rolny</base><ctag>adj:sg:nom:m2:pos</ctag></lex>"
"<lex disamb=\"1\"><base>rolny</base><ctag>adj:sg:nom:m3:pos</ctag></lex>"
"</tok>"
"<tok>"
"<orth>czy</orth>"
"<lex disamb=\"1\"><base>czy</base><ctag>conj</ctag></lex>"
"<lex><base>czy</base><ctag>qub</ctag></lex>"
"</tok>"
"<tok>"
"<orth>leśny</orth>"
"<lex><base>leśny</base><ctag>adj:sg:acc:m3:pos</ctag></lex>"
"<lex><base>leśny</base><ctag>adj:sg:nom:m1:pos</ctag></lex>"
"<lex><base>leśny</base><ctag>adj:sg:nom:m2:pos</ctag></lex>"
"<lex disamb=\"1\"><base>leśny</base><ctag>adj:sg:nom:m3:pos</ctag></lex>"
"</tok>"
"<ns/>"
"<tok>"
"<orth>.</orth>"
"<lex disamb=\"1\"><base>.</base><ctag>interp</ctag></lex>"
"</tok>"
"</chunk>"
"</chunk>"
"</chunkList>"
"</cesAna>"
;

}

struct AF
{
	AF()
	 : tagset(Corpus2::get_named_tagset("kipi"))
	{
		std::stringstream ss;
		ss << swiatopoglad;
		Corpus2::XcesReader xr(tagset, ss);
		s = xr.get_next_sentence();
		as = Corpus2::AnnotatedSentence::wrap_sentence_clone(s);
	}


	const Corpus2::Tagset& tagset;
	Corpus2::Sentence::Ptr s;
	boost::shared_ptr<Corpus2::AnnotatedSentence> as;
};

BOOST_FIXTURE_TEST_CASE( create, AF )
{
	BOOST_CHECK(!as->empty());
	BOOST_REQUIRE_EQUAL(as->size(), s->size());
	for (size_t i = 0; i < as->size(); ++i) {
		BOOST_CHECK(as->tokens()[i] != s->tokens()[i]);
		BOOST_CHECK(*as->tokens()[i] == *s->tokens()[i]);
	}
	BOOST_CHECK(!as->has_channel("a"));
	BOOST_CHECK_THROW(as->get_channel("a"), Corpus2::MissingAnnotationChannel);
	as->create_channel("a");
	BOOST_CHECK(as->has_channel("a"));
	BOOST_CHECK_NO_THROW(as->get_channel("a"));
	BOOST_CHECK(!as->has_channel("aa"));
	BOOST_CHECK_THROW(as->get_channel("aa"), Corpus2::MissingAnnotationChannel);
}


BOOST_AUTO_TEST_SUITE_END()
