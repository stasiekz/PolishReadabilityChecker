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

#include "compareconv.h"

#include <libpwrutils/util.h>
#include <boost/foreach.hpp>
#include <libpwrutils/pathsearch.h>
#include <libcorpus2/util/settings.h>
#include <libtoki/util/settings.h>
#include <libmaca/util/settings.h>
#include <libmaca/conv/tagsetconverter.h>
#include <libcorpus2/io/xcesreader.h>
#include <libcorpus2/io/xceswriter.h>

#include <fstream>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>

//#include <libcorpus2/config_d.h>

using boost::filesystem::directory_iterator;
using boost::filesystem::exists;
using boost::filesystem::is_directory;
using boost::filesystem::path;
using boost::filesystem::ifstream;

struct compare_test
{
	path search_path;
	path in_file;
	path out_file;
	path config;
};

void test_one_item_actual(const compare_test& c)
{
	PwrNlp::ConfigPathSetter corpus2_path_setter(Corpus2::Path::Instance(),
		c.search_path.string() + Corpus2::Path::Instance().get_search_path_string());
	PwrNlp::ConfigPathSetter toki_path_setter(Toki::Path::Instance(),
		c.search_path.string() + Toki::Path::Instance().get_search_path_string());
	PwrNlp::ConfigPathSetter maca_path_setter(Maca::Path::Instance(),
		c.search_path.string() + Maca::Path::Instance().get_search_path_string());

	ifstream ifs_in(c.in_file);
	ifstream ifs_out(c.out_file);

	Maca::Config::Node n = Maca::Config::from_file(c.config.string());
	Maca::Conversion::TagsetConverter conv(n);

	Corpus2::XcesReader reader(conv.tagset_from(), ifs_in);

	boost::shared_ptr<Corpus2::TokenWriter> writer;
	std::stringstream ss;
	writer = Corpus2::TokenWriter::create_stream_writer("xces,sorttags", ss, conv.tagset_to());
	while (boost::shared_ptr<Corpus2::Chunk> c = reader.get_next_chunk()) {
		BOOST_FOREACH(Corpus2::Sentence::Ptr& s, c->sentences()) {
			s = conv.convert_sentence(s);
		}
		writer->write_chunk(*c);
	}
	writer.reset();
	std::string expected, actual;
	int line = 0;
	bool failed = false;
	while (ifs_out.good() && ss.good()) {
		++line;
		std::getline(ifs_out, expected);
		std::getline(ss, actual);
		if (actual != expected) {
			if (!failed) {
				BOOST_ERROR("Difference in " << c.out_file);
				failed = true;
			}
			BOOST_MESSAGE(
				"\nline " << line << " got: " << actual << "\n" <<
				line << " expected: " << expected);
		}
	}
	if (failed) {
		BOOST_WARN_EQUAL(ifs_out.good(), ss.good());
	} else {
		BOOST_CHECK_EQUAL(ifs_out.good(), ss.good());
	}
}


void test_one_item(const compare_test& c)
{
	try {
		test_one_item_actual(c);
	} catch (PwrNlp::PwrNlpError& e) {
		BOOST_ERROR("Caught " << e.scope() << " exception: \n" << e.info());
	}
}






int init_subdir(const path& dir, std::string ps, std::vector<compare_test>& tests)
{
	int count = 0;
	ps += dir.string();
	if (!ps.empty()) {
		ps += Corpus2::Path::Instance().get_path_separator();
	}

	directory_iterator end_itr; // default-constructed is past-the-end
	std::set<std::string> tests_in;
	std::set<std::string> tests_out;
	std::set<std::string> configs;
	std::set<path> subdirs;

	for (directory_iterator itr(dir); itr != end_itr; ++itr) {
		if (is_directory(itr->status())) {
			subdirs.insert(itr->path());
		} else {
			if (itr->path().extension() == ".in") {
				tests_in.insert(itr->path().stem().c_str());
			} else if (itr->path().extension() == ".out") {
				tests_out.insert(itr->path().stem().c_str());
			} else if (itr->path().extension() == ".conv") {
				configs.insert(itr->path().stem().c_str());
			}
		}
	}
	BOOST_FOREACH(const std::string& s, tests_out) {
		compare_test c;
		if (tests_in.find(s) == tests_in.end()) {
			if (tests_in.find("main") == tests_in.end()) {
				BOOST_TEST_MESSAGE("missing .in file : " << s);
				continue;
			} else {
				c.in_file = dir / "main.in";
			}
		} else {
			c.in_file = dir / (s + ".in");
		}
		c.out_file = dir / (s + ".out");
		if (configs.find(s) == configs.end()) {
			if (configs.find("main") == configs.end()) {
				BOOST_TEST_MESSAGE("missing .conv file : " << s);
				continue;
			} else {
				c.config = dir / "main.conv";
			}
		} else {
			c.config = dir / (s + ".conv");
		}
		c.search_path = ps;
		tests.push_back(c);
		++count;
	}
	BOOST_TEST_MESSAGE("Found " << count << " valid compare test case"
		<< (count > 1 ? "s" : "")
		<< " in " << dir
		<< " [" << ps << "]"
		);
	BOOST_FOREACH(const path& s, subdirs) {
		count += init_subdir(s, ps, tests);
	}
	return count;
}

void init_compareconv_suite(boost::unit_test::test_suite *ts, const std::string& path)
{
	std::string subdir_name = LIBMACA_TEST_DATA_DIR "compare-conv";
	if (!path.empty()) {
		subdir_name = path;
	}
	if (!exists(subdir_name)) {
		BOOST_TEST_MESSAGE("Compare subdir does not exist");
	}
	std::vector<compare_test> compares;
	init_subdir(subdir_name, "", compares);
	BOOST_FOREACH(const compare_test& ci, compares) {
		std::string rel_path = boost::algorithm::replace_first_copy(
				ci.out_file.string(), subdir_name, "");
		std::string name = "test_compare:" + rel_path;
		ts->add(boost::unit_test::make_test_case(
			boost::bind(test_one_item, ci), name));
	}
}

