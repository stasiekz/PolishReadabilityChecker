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
#include <set>
#include <boost/foreach.hpp>
#include <libpwrutils/bitset.h>
#include <libcorpus2/tagset.h>
#include <libcorpus2/token.h>

BOOST_AUTO_TEST_SUITE( tag_split )

struct F {
	F() {
		const char tagset_string[] = "[ATTR]\n"
			"A tag tog other a3 \n"
			"B data thing tag-thing thang\n"
			"C a b c \n"
			"[POS]\n some A B [C]\n same A B \n P3 [A] [B]\n"
			"[IGN]\nP3\n";
		tagset.reset(new Corpus2::Tagset());
		*tagset = Corpus2::Tagset::from_data(tagset_string);
	}
	boost::shared_ptr<Corpus2::Tagset> tagset;

	std::vector<Corpus2::Tag> check_split(const std::string s, const std::set<std::string> expect)
	{
		std::set<std::string> actual;
		std::vector<Corpus2::Tag> tags;
		Corpus2::Token t;
		try {
			tagset->lexemes_into_token(t, UnicodeString(), s);
		} catch (Corpus2::TagParseError& e) {
			throw;
		}

		BOOST_FOREACH(const Corpus2::Lexeme& lex, t.lexemes()) {
			const Corpus2::Tag& tag = lex.tag();
			BOOST_WARN(tagset->validate_tag(tag, Corpus2::Tagset::ParseStrict, &std::cerr));
			actual.insert(tagset->tag_to_string(tag));
			tags.push_back(tag);
		}
		BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expect.begin(), expect.end());
		return tags;
	}
};


BOOST_FIXTURE_TEST_CASE( plain, F )
{
	const char tag[] = "some:tag:data";
	std::set<std::string> r;
	r.insert(tag);
	check_split(tag, r);
}

BOOST_FIXTURE_TEST_CASE( plus, F )
{
	const char tag[] = "some:tag:data+some:other:tag-thing";
	std::set<std::string> result;
	result.insert("some:tag:data");
	result.insert("some:other:tag-thing");
	check_split(tag, result);
}

BOOST_FIXTURE_TEST_CASE( dot, F )
{
	const char tag[] = "some:tag.tog:data";
	std::set<std::string> result;
	result.insert("some:tag:data");
	result.insert("some:tog:data");
	check_split(tag, result);
}

BOOST_FIXTURE_TEST_CASE( dots, F )
{
	const char tag[] = "some:tag.tog:data:a.b.c";
	std::set<std::string> result;
	result.insert("some:tag:data:a");
	result.insert("some:tog:data:a");
	result.insert("some:tag:data:b");
	result.insert("some:tog:data:b");
	result.insert("some:tag:data:c");
	result.insert("some:tog:data:c");
	check_split(tag, result);
}

BOOST_FIXTURE_TEST_CASE( dots_plus, F )
{
	const char tag[] = "some:tag.tog:data:a.b+some:other:thing.thang";
	std::set<std::string> result;
	result.insert("some:tag:data:a");
	result.insert("some:tog:data:a");
	result.insert("some:tag:data:b");
	result.insert("some:tog:data:b");
	result.insert("some:other:thing");
	result.insert("some:other:thang");
	check_split(tag, result);
}

BOOST_FIXTURE_TEST_CASE( missing, F )
{
	const char tag[] = "P3:data";
	std::set<std::string> r;
	r.insert("P3:data");
	check_split(tag, r);
}

BOOST_FIXTURE_TEST_CASE( bad_value, F )
{
	const char tag[] = "some:bad:data";
	std::set<std::string> r;
	BOOST_CHECK_THROW(
			check_split(tag, r), Corpus2::TagParseError
	);
}

BOOST_FIXTURE_TEST_CASE( bad_pos, F )
{
	const char tag[] = "something:data";
	std::set<std::string> r;
	BOOST_CHECK_THROW(
			check_split(tag, r), Corpus2::TagParseError
	);
}

BOOST_FIXTURE_TEST_CASE( underscore, F )
{
	const char tag[] = "some:_:data";
	std::set<std::string> r;
	r.insert("some:tag:data");
	r.insert("some:tog:data");
	r.insert("some:other:data");
	r.insert("some:a3:data");
	check_split(tag, r);
}

BOOST_FIXTURE_TEST_CASE( underscores, F )
{
	const char tag[] = "some:_:data:_";
	std::set<std::string> r0;
	r0.insert("some:tag:data");
	r0.insert("some:tog:data");
	r0.insert("some:other:data");
	r0.insert("some:a3:data");
	std::set<std::string> r;
	BOOST_FOREACH(const std::string& s, r0) {
		r.insert(s + ":a");
		r.insert(s + ":b");
		r.insert(s + ":c");
	}

	check_split(tag, r);
}

BOOST_FIXTURE_TEST_CASE( underscore_dots, F )
{
	const char tag[] = "some:_:data:c.a";
	std::set<std::string> r0;
	r0.insert("some:tag:data");
	r0.insert("some:tog:data");
	r0.insert("some:other:data");
	r0.insert("some:a3:data");
	std::set<std::string> r;
	BOOST_FOREACH(const std::string& s, r0) {
		r.insert(s + ":a");
		r.insert(s + ":c");
	}

	check_split(tag, r);
}


BOOST_FIXTURE_TEST_CASE( tag_size, F )
{
	Corpus2::Tag t = tagset->parse_simple_tag("some:tag:data");
	Corpus2::Tag t2 = tagset->parse_simple_tag("some:tog:data");
	Corpus2::Tag t3 = tagset->parse_simple_tag("same:tag:data");
	BOOST_CHECK(tagset->tag_is_singular(t));
	BOOST_CHECK_EQUAL(tagset->tag_size(t), 1);
	BOOST_CHECK(tagset->tag_is_singular(t2));
	BOOST_CHECK_EQUAL(tagset->tag_size(t2), 1);
	BOOST_CHECK(tagset->tag_is_singular(t3));
	BOOST_CHECK_EQUAL(tagset->tag_size(t3), 1);
	t.add_values(t2.get_values());
	BOOST_CHECK(!tagset->tag_is_singular(t));
	BOOST_CHECK_EQUAL(tagset->tag_size(t), 2);
	t.add_pos(t3.get_pos());
	BOOST_CHECK(!tagset->tag_is_singular(t));
	BOOST_CHECK_EQUAL(tagset->tag_size(t), 4);
	Corpus2::Tag t4 = tagset->parse_simple_tag("same:other:thang", Corpus2::Tagset::ParseLoose);
	t.add_values(t4.get_values() & tagset->get_attribute_mask(std::string("A")));
	BOOST_CHECK_EQUAL(tagset->tag_size(t), 6);
	std::vector<Corpus2::Tag> tags = tagset->split_tag(t);
	BOOST_CHECK_EQUAL(tags.size(), 6);
	Corpus2::Tag tt;
	BOOST_FOREACH(Corpus2::Tag t, tags) {
		tt.combine_with(t);
	}
	BOOST_CHECK(tt == t);
}

BOOST_FIXTURE_TEST_CASE( s, F )
{
	Corpus2::Tag t = tagset->parse_simple_tag("some:tag:data");
	Corpus2::Tag t2 = tagset->parse_simple_tag("same:tog:data");
	BOOST_CHECK_EQUAL(tagset->tag_to_symbol_string(t), "some,tag,data");
	BOOST_CHECK_EQUAL(tagset->tag_to_symbol_string(t2), "same,tog,data");
	Corpus2::Tag t3 = t.get_combined(t2);
	std::vector<std::string> v = tagset->tag_to_symbol_string_vector(t3);
	std::sort(v.begin(), v.end());
	std::vector<std::string> v2;
	v2.push_back("some");
	v2.push_back("same");
	v2.push_back("tog");
	v2.push_back("tag");
	v2.push_back("data");
	std::sort(v2.begin(), v2.end());
	BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), v2.begin(), v2.end());

}

BOOST_FIXTURE_TEST_CASE( symbols, F )
{
	Corpus2::Tag t;
	t = tagset->parse_symbol("some");
	BOOST_CHECK_EQUAL(tagset->tag_to_symbol_string(t), "some");
	t = tagset->parse_symbol("tag");
	BOOST_CHECK_EQUAL(tagset->tag_to_symbol_string(t), "tag");
	t = tagset->parse_symbol("C");
	BOOST_CHECK_EQUAL(tagset->tag_to_symbol_string(t), "C");
	BOOST_CHECK(tagset->tag_to_symbol_string(t, false) != "C");
	BOOST_CHECK_THROW(tagset->parse_symbol("asdf"), Corpus2::TagParseError);
}

BOOST_FIXTURE_TEST_CASE(attribute_mask_to_name, F)
{
	BOOST_FOREACH(Corpus2::mask_t a, tagset->all_attribute_masks()) {
		std::string aname = tagset->get_attribute_name(a);
		BOOST_CHECK(!aname.empty());
		Corpus2::mask_t aa = tagset->get_attribute_mask(aname);
		BOOST_CHECK_EQUAL(a, aa);
	}
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(bs_split)
{
	std::bitset<32> x(0xf6543);
	std::bitset<32> y(0);
	BOOST_FOREACH(std::bitset<32> b, PwrNlp::set_bits(x)) {
		BOOST_CHECK_EQUAL(b.count(), 1);
		y |= b;
	}
	BOOST_CHECK_EQUAL(x, y);
}




