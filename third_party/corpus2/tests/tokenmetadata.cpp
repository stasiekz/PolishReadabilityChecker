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
#include <libcorpus2/tokenmetadata.h>

BOOST_AUTO_TEST_SUITE(token_metadata)

BOOST_AUTO_TEST_CASE(meta1)
{
	Corpus2::Token t1(UnicodeString::fromUTF8("t1"), PwrNlp::Whitespace::None);
	BOOST_CHECK(!t1.get_metadata());
	t1.create_metadata();
	BOOST_CHECK(t1.get_metadata());
	BOOST_CHECK(t1.get_metadata()->attributes().empty());
	Corpus2::Token* t2 = t1.clone();
	BOOST_CHECK(t2->get_metadata());
	BOOST_CHECK(t2->get_metadata()->attributes().empty());
	t2->get_metadata()->set_attribute("A", "B");
	BOOST_CHECK(!t2->get_metadata()->attributes().empty());
	BOOST_CHECK(t1.get_metadata()->attributes().empty());
	delete t2;
}

BOOST_AUTO_TEST_SUITE_END()
