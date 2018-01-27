/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <libtoki/srx/document.h>
#include <libtoki/srx/segmenter.h>
#include <libtoki/srx/srx.h>
#include <libtoki/tokenizer/layertokenizer.h>
#include <libtoki/sentencesplitter.h>
#include <libtoki/token.h>
#include <libtoki/unicode/icustringwrapper.h>

#include <fstream>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

namespace {

#ifdef LIBTOKI_TEST_DATA_DIR
	static std::string data_dir = LIBTOKI_TEST_DATA_DIR;
#else
	static std::string data_dir = "./";
#endif

}


BOOST_AUTO_TEST_SUITE( srx );

BOOST_AUTO_TEST_CASE( parse )
{
	Toki::Srx::Document d;
	std::string s = data_dir + "/one.srx";
	std::ifstream ifs(s.c_str());
	d.load(ifs);
	std::vector<Toki::Srx::Rule> rules = d.get_all_rules();
	BOOST_CHECK_EQUAL(rules.size(), 2);
}

namespace {
	//                      0000000000111111111122222222223333333333444444444455555555556666666666
	//                      0123456789012345678901234567890123456789012345678901234567890123456789
	static std::string t = "Mr. Holmes is from the U.K. not from the U.S.A. Is Dr. Watson from there too?";
	int tb[2] = {3, 54};
	int* tbe = tb + 2;
}


BOOST_AUTO_TEST_CASE( simple )
{
	Toki::Srx::Document d;
	std::string s = data_dir + "/one.srx";
	std::ifstream ifs(s.c_str());
	d.load(ifs);
	Toki::Srx::NaiveIcuSegmenter proc;
	proc.load_rules(d.get_all_rules());

	proc.compute_breaks(UnicodeString::fromUTF8(t), 0, t.size());
	std::vector<int> breaks = proc.get_break_positions();

	BOOST_CHECK_EQUAL_COLLECTIONS(tb, tbe, breaks.begin(), breaks.end());

#ifdef HAVE_BOOST_REGEX
	// test only if build with boost.regex support
	Toki::Srx::NaiveBoostSegmenter boo;
	boo.load_rules(d.get_all_rules());

	boo.compute_breaks(UnicodeString::fromUTF8(t), 0, t.size());
	breaks = boo.get_break_positions();

	BOOST_CHECK_EQUAL_COLLECTIONS(tb, tbe, breaks.begin(), breaks.end());

	Toki::Srx::HxoIcuSegmenter hxo;
	hxo.load_rules(d.get_all_rules());

	hxo.compute_breaks(UnicodeString::fromUTF8(t), 0, t.size());
	breaks = hxo.get_break_positions();

	BOOST_CHECK_EQUAL_COLLECTIONS(tb, tbe, breaks.begin(), breaks.end());
#endif
}

BOOST_AUTO_TEST_CASE( variable_window )
{
	Toki::Srx::Document d;
	std::string s = data_dir + "/one.srx";
	std::ifstream ifs(s.c_str());
	d.load(ifs);
	boost::shared_ptr<Toki::Srx::Segmenter> segm(new Toki::Srx::NaiveIcuSegmenter);
	segm->load_rules(d.get_all_rules());

	BOOST_MESSAGE("Running " << (15 - 4) * 40 << " variable_window srx tests");

	UnicodeString us = UnicodeString::fromUTF8(t);
	for (int m = 4; m < 15; ++m) {
		for(int w = 40; w > 0; --w) {
			boost::shared_ptr<Toki::UnicodeIcuStringWrapper> isw;
			isw.reset(new Toki::UnicodeIcuStringWrapper(us));
			Toki::Srx::SourceWrapper srx(isw, segm, w, m);
			std::vector<int> breaks;
			int i = 0;
			while (srx.has_more_chars()) {
				if (srx.peek_begins_sentence()) {
					breaks.push_back(i);
				}
				++i;
				srx.get_next_char();
			}
			bool okay = true;
			int* a = &breaks[0];
			int* ae = a + breaks.size();
			int* b = tb;
			while (a < ae && b < tbe) {
				if (*a++ != *b++) {
					okay = false;
				}
			}
			if (a != ae || b != tbe) {
				okay = false;
			}
			BOOST_CHECK_MESSAGE(okay, "Mismatch for window " << w << " and margin " << m);
			BOOST_REQUIRE_EQUAL_COLLECTIONS(tb, tbe, breaks.begin(), breaks.end());
		}
	}

}


BOOST_AUTO_TEST_CASE( sentence )
{
	std::vector<Toki::Token*> v;
	v.push_back(new Toki::Token("a", "1", PwrNlp::Whitespace::Space));
	v[0]->set_begins_sentence(true);
	v.push_back(new Toki::Token("b", "2", PwrNlp::Whitespace::Space));
	v.push_back(new Toki::Token("c", "3", PwrNlp::Whitespace::Space));
	v.push_back(new Toki::Token("d", "4", PwrNlp::Whitespace::Space));
	v[3]->set_begins_sentence(true);
	v.push_back(new Toki::Token("e", "5", PwrNlp::Whitespace::Space));
	v[4]->set_begins_sentence(true);
	v.push_back(new Toki::Token("f", "6", PwrNlp::Whitespace::Space));
	v.push_back(new Toki::Token("g", "7", PwrNlp::Whitespace::Space));
	v[6]->set_begins_sentence(true);
	Toki::TokenSource* src = Toki::make_range_source(v);

	Toki::SentenceSplitter s(*src);
	Toki::Sentence* o;

	o = s.get_next_sentence();
	BOOST_REQUIRE_EQUAL(o->size(), 3);
	delete o;
	o = s.get_next_sentence();
	BOOST_REQUIRE_EQUAL(o->size(), 1);
	BOOST_REQUIRE_EQUAL((*o)[0]->orth_utf8(), "d");
	delete o;
	o = s.get_next_sentence();
	BOOST_REQUIRE_EQUAL(o->size(), 2);
	delete o;
	o = s.get_next_sentence();
	BOOST_REQUIRE_EQUAL(o->size(), 1);
	BOOST_REQUIRE_EQUAL((*o)[0]->orth_utf8(), "g");
	delete o;
	delete src;
}


BOOST_AUTO_TEST_SUITE_END()
