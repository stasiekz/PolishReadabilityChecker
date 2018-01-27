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

//#define BOOST_TEST_MODULE master
#include <boost/test/included/unit_test.hpp>
#include <unicode/uclean.h>
#include "datadriven.h"
#include "datarule.h"

BOOST_AUTO_TEST_CASE(test_test)
{
	int a = 0;
	BOOST_CHECK(a == 0);
}

//Auto cleanup of ICU with a static variable destructor.
//ICU can hold onto some memory resources until program exit,
//when the system is reclaiming them as usual. This is not a memory
//leak but makes tools like valgrind all excited that
//there is memory which is "still reachable but not freed."
//So, the below will perform icu cleanup before test program exit.
static struct ICU_cleaner
{
	~ICU_cleaner()
	{
		u_cleanup();
	}
} the_cleaner;

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
	init_data_suite(ts1, compare_path);
	boost::unit_test::framework::master_test_suite().add(ts1);
	boost::unit_test::test_suite* ts2 = BOOST_TEST_SUITE("rule-compare");
	init_data_rule_suite(ts2, compare_path);
	boost::unit_test::framework::master_test_suite().add(ts2);
	return 0;
}
