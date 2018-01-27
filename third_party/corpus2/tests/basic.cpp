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

#include <libcorpus2/token.h>

const char tagsetstr1[] = "[ATTR]\n"
	"A tag tog other a3 \n"
	"B data thing tag-thing thang\n"
	"C a b c \n"
	"ign\n"
	"[POS]\n some A B [C]\n";

BOOST_AUTO_TEST_CASE( token )
{
	Corpus2::Token t(UnicodeString::fromUTF8("ZZ"), PwrNlp::Whitespace::ManySpaces);
	BOOST_CHECK_EQUAL(t.orth_utf8(), "ZZ");
	BOOST_CHECK_EQUAL(t.wa(), PwrNlp::Whitespace::ManySpaces);
	BOOST_CHECK(t.lexemes().empty());
}

BOOST_AUTO_TEST_CASE( token_dup_lexemes )
{
	Corpus2::Token t(UnicodeString::fromUTF8("ZZ"), PwrNlp::Whitespace::ManySpaces);
	//Corpus2::Tagset tagset(tagsetstr1);
	Corpus2::Tag t1(Corpus2::mask_t(0));
	Corpus2::Lexeme l1(UnicodeString::fromUTF8("aaa"), t1);
	Corpus2::Lexeme l2(UnicodeString::fromUTF8("bbb"), t1);
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	t.add_lexeme(l1);
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	t.add_lexeme(l2);
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	Corpus2::Token* tt = t.clone();
	t.add_lexeme(l1);
	BOOST_CHECK(t != *tt);
	BOOST_CHECK(t.check_duplicate_lexemes());
	BOOST_CHECK(t.remove_duplicate_lexemes());
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	BOOST_CHECK(t == *tt);
	delete tt;
}

BOOST_AUTO_TEST_CASE( is_icu_working )
{
	std::string s("aaa");
	UnicodeString u(UnicodeString::fromUTF8(s));
	std::string s2 = PwrNlp::to_utf8(u);
	BOOST_CHECK_EQUAL(s, s2);
}

