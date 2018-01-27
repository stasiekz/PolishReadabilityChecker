/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

//#define BOOST_TEST_MODULE master
#include <boost/test/included/unit_test.hpp>
#include "compareconv.h"

BOOST_AUTO_TEST_CASE(test_test)
{
	int a = 0;
	BOOST_CHECK(a == 0);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
	boost::unit_test::test_suite* ts1 = BOOST_TEST_SUITE("compare");
	std::string compare_path;
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "--compare-tests-dir") == 0) {
			++i;
			if (i < argc) {
				compare_path = argv[i];
			}
		}
	}
	init_compareconv_suite(ts1, compare_path);
	boost::unit_test::framework::master_test_suite().add(ts1);
	return 0;
}
