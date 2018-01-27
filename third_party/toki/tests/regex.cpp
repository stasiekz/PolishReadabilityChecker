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

#include <fstream>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/regex/icu.hpp>
#include <boost/test/unit_test.hpp>

namespace {

#ifdef LIBTOKI_TEST_DATA_DIR
	static std::string data_dir = LIBTOKI_TEST_DATA_DIR;
#else
	static std::string data_dir = "./";
#endif

}


BOOST_AUTO_TEST_SUITE( regex );

std::vector<int> all_matches(RegexMatcher& r)
{
	std::vector<int> v;
	int i = 0;
	int ln = -1;
	UErrorCode u = U_ZERO_ERROR;
	while (r.find(i, u) && U_SUCCESS(u)) {
		UErrorCode status = U_ZERO_ERROR;
		int n = r.end(1, status);
		if (U_SUCCESS(status) && ln != n) {
			ln = n;
			//std::cerr << "match at " << n << "\n";
			v.push_back(n);
		}
		++i;
	}
	return v;
}

BOOST_AUTO_TEST_CASE( grouping_icu )
{
	return;
	//                012345678
	UnicodeString s1("zabbaccde");
	UErrorCode e = U_ZERO_ERROR;
	UnicodeString r1("(ab+a)(c)");
	UnicodeString r2("(ac+d)(e)");
	RegexMatcher m1(r1, s1, 0, e);
	BOOST_REQUIRE(U_SUCCESS(e));
	RegexMatcher m2(r2, s1, 0, e);
	BOOST_REQUIRE(U_SUCCESS(e));
	UnicodeString r3 = "(?|" + r1 + ")|(?|" + r2 + ")";
	RegexMatcher m3(r3, s1, 0, e);
	BOOST_REQUIRE(U_SUCCESS(e));

	std::vector<int> v1 = all_matches(m1);
	BOOST_CHECK(v1.at(0) == 5);
	std::vector<int> v2 = all_matches(m2);
	BOOST_CHECK(v2.at(0) == 8);
	std::vector<int> v3 = all_matches(m3);
	std::vector<int> v1_2;
	std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), std::inserter(v1_2, v1_2.begin()));
	//v3.push_back(0);
	BOOST_CHECK_EQUAL_COLLECTIONS( v1_2.begin(), v1_2.end(), v3.begin(), v3.end() );
}

std::vector<int> all_matches(boost::u32regex& r, const UnicodeString& s)
{
	boost::match_results<const UChar*> what;
	std::vector<int> v;
	const UChar* start = s.getBuffer();
	const UChar* end = start + s.length();
	int ln = -1;
	boost::match_flag_type flags = boost::match_default;
	while (boost::u32regex_search(start, end, what, r, flags)) {
		int n = what[1].second - s.getBuffer();
		if (n != ln) {
			ln = n;
			v.push_back(n);
			//std::cerr << "match at " << n << "\n";
		}
		start = what[0].first + 1; // sad panda
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
	}
	return v;
}

BOOST_AUTO_TEST_CASE( grouping_boost )
{
	//                0123456789AB
	UnicodeString s1("zabbaccdefff");
	//                 =====
	//                    =====
	UnicodeString r1("(ab+a)(c)");
	UnicodeString r2("(ac+d)(e)");
	UnicodeString r3 = "(?|(?:" + r2 + ")|(?:" + r1 + "))";
	boost::u32regex m1 = boost::make_u32regex(r1);
	boost::u32regex m2 = boost::make_u32regex(r2);
	boost::u32regex m3 = boost::make_u32regex(r3);

	std::vector<int> v1 = all_matches(m1, s1);
	BOOST_CHECK(v1.at(0) == 5);
	std::vector<int> v2 = all_matches(m2, s1);
	BOOST_CHECK(v2.at(0) == 8);
	std::vector<int> v3 = all_matches(m3, s1);
	std::vector<int> v1_2;
	std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), std::inserter(v1_2, v1_2.begin()));
	//v3.push_back(0);
	BOOST_CHECK_EQUAL_COLLECTIONS( v1_2.begin(), v1_2.end(), v3.begin(), v3.end() );

}

BOOST_AUTO_TEST_CASE( grouping_boost2 )
{
	//                0123456789AB
	UnicodeString s1("zcaabcaabcaz");
	//                 ======
	//                     ======
	UnicodeString r1("(ca+bca)()");
	boost::u32regex m1 = boost::make_u32regex(r1);

	std::vector<int> v1 = all_matches(m1, s1);
	BOOST_CHECK(v1.at(0) == 7);
	BOOST_CHECK(v1.at(1) == 11);
}

BOOST_AUTO_TEST_SUITE_END()
