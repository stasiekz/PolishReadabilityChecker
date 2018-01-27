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

#include <libtoki/tokenizer/layertokenizer.h>
#include "compare.h"
#include <libtoki/util/debug.h>
#include <libpwrutils/util.h>
#include <libtoki/token.h>
#include <boost/foreach.hpp>

#include <fstream>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>

namespace fs = boost::filesystem;

namespace {

#ifdef LIBTOKI_TEST_DATA_DIR
	static std::string data_dir = LIBTOKI_TEST_DATA_DIR;
#else
	static std::string data_dir = "./";
#endif

	static std::string subdir_name = data_dir + "compare";
}

//BOOST_AUTO_TEST_SUITE( test_compare )
struct compare_item
{
	Toki::Config::Node* base_config;

	std::vector<fs::path> configs;

	fs::path in_file;

	fs::path out_file;
};

static std::vector< boost::shared_ptr<Toki::Config::Node> > global_configs;

static std::vector< compare_item > global_compares;

void test_one_item(const compare_item& c)
{
	int created_before = Toki::Token::creation_count();
	int instances_before = Toki::Token::instance_count();
	int tok_count = 0;
	PwrNlp::ConfigPathSetter path_setter(Toki::Path::Instance(), data_dir);
	fs::path file_in = c.in_file;
	fs::path file_out = c.out_file;
	std::string rel_path = boost::algorithm::replace_first_copy(
			file_out.string(), subdir_name, "");
	BOOST_REQUIRE_MESSAGE(fs::exists(file_in), "Input file " << file_in << " not found!");
	BOOST_REQUIRE_MESSAGE(fs::exists(file_out), "Expected output file " << file_out << " not found!");

	fs::ifstream ifs_in(file_in);
	fs::ifstream ifs_out(file_out);
	Toki::Config::Node cfg;
	if (c.base_config != NULL) {
		cfg = *(c.base_config);
	}

	BOOST_FOREACH (const fs::path& p, c.configs) {
		Toki::Config::Node additional = Toki::Config::from_file(p.string());
		Toki::Config::merge_into(cfg, additional);
	}

	Toki::LayerTokenizer tok(ifs_in, cfg);
	std::string format = PwrNlp::unescape_utf8(cfg.get("debug.format", "[$orth]"));
	std::stringstream ss_expected;
	std::stringstream ss_actual;
	if (cfg.get("debug.warning_dest", "") == "stdout") {
		tok.set_error_stream(&ss_actual);
	}
	ss_expected << ifs_out.rdbuf();
	Toki::Debug::tokenize_formatted(tok, format, ss_actual, &tok_count);
	BOOST_REQUIRE_EQUAL (ss_actual.str(), ss_expected.str());

	ifs_in.close();
	ifs_in.open(file_in);
	std::stringstream ss;
	ss << ifs_in.rdbuf();
	UnicodeString us = UnicodeString::fromUTF8(ss.str());
	ifs_in.close();

	for (int i = 0; i < 5; ++i) {
		std::stringstream ss_in;
		ss_in << ss.str();
		tok.restart();
		tok.set_input_source(ss_in, i + 1);
		std::stringstream ss_actual_loop;
		if (cfg.get("debug.warning_dest", "") == "stdout") {
			tok.set_error_stream(&ss_actual_loop);
		}
		Toki::Debug::tokenize_formatted(tok, format, ss_actual_loop);
		BOOST_REQUIRE_EQUAL (ss_actual_loop.str(), ss_expected.str());
		tok.restart();
		tok.set_input_source(us);
		std::stringstream ss_actual2;
		if (cfg.get("debug.warning_dest", "") == "stdout") {
			tok.set_error_stream(&ss_actual2);
		}
		Toki::Debug::tokenize_formatted(tok, format, ss_actual2);
		BOOST_REQUIRE_EQUAL (ss_actual2.str(), ss_expected.str());
	}
	if (Toki::Token::creation_count() > 0) {
		BOOST_CHECK_EQUAL (instances_before, Toki::Token::instance_count());
		int created = Toki::Token::creation_count() - created_before;
		BOOST_TEST_MESSAGE("Test case " << rel_path << ": "
			<< (created/11) << " tokens per run with "
			<< tok_count << " output, ratio "
			<< std::setprecision(2) << (1.0*created/11/tok_count) << ")");
	}
}

void subdir_exists()
{
	BOOST_REQUIRE_MESSAGE(fs::exists(subdir_name),
		subdir_name << " directory must exist");
	BOOST_REQUIRE_MESSAGE(fs::is_directory(subdir_name),
		subdir_name << " must be a directory");
}

void init_subdir(fs::path dir)
{
	PwrNlp::ConfigPathSetter path_setter(Toki::Path::Instance(), data_dir);
	fs::directory_iterator end_itr; // default_config construction yields past-the-end

	std::set<std::string> tests_in;
	std::set<std::string> tests_out;
	std::set<std::string> configs;
	std::set<fs::path> subdirs;
	for (fs::directory_iterator itr(dir); itr != end_itr; ++itr) {
		if (fs::is_directory(itr->status())) {
			subdirs.insert(itr->path());
		} else {
			if (itr->path().extension() == ".in") {
				tests_in.insert(itr->path().stem().c_str());
			} else if (itr->path().extension() == ".out") {
				tests_out.insert(itr->path().stem().c_str());
			} else if (itr->path().extension() == ".ini") {
				configs.insert(itr->path().stem().c_str());
			}
		}
	}
	std::set<std::string>::iterator i = configs.find("main");
	Toki::Config::Node* cfg = new Toki::Config::Node();
	if (i != configs.end()) {
		std::string p = (dir / "main.ini").string();
		*cfg = Toki::Config::from_file(p);
	} else {
		//check dir name, load special config if possible, else default_config
		*cfg = Toki::default_config();
	}
	global_configs.push_back(boost::shared_ptr<Toki::Config::Node>(cfg));
	int count = 0;
	BOOST_FOREACH (const std::string& s, tests_out) {
		compare_item c;
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
		c.base_config = cfg;
		if (configs.find(s) != configs.end()) {
			c.configs.push_back(dir / (s + ".ini"));
		}
		global_compares.push_back(c);
		++count;
	}
	BOOST_TEST_MESSAGE("Found " << count << " valid compare test case"
		<< (count > 1 ? "s" : "")
		<< " in " << dir);
	BOOST_FOREACH (const fs::path& s, subdirs) {
		init_subdir(s);
	}
}


void init_compare_suite(boost::unit_test::test_suite *ts, const std::string& path)
{
	if (!path.empty()) {
		subdir_name = path;
	}
	ts->add(BOOST_TEST_CASE(&subdir_exists));
	if (!fs::exists(subdir_name)) return;
	init_subdir(subdir_name);
	BOOST_FOREACH (const compare_item& ci, global_compares) {
		std::string rel_path = boost::algorithm::replace_first_copy(
				ci.out_file.string(), subdir_name, "");
		std::string name = "test_compare:" + rel_path;
		ts->add(boost::unit_test::make_test_case(
				boost::bind(test_one_item, ci), name)
				);
	}
}



//BOOST_AUTO_TEST_SUITE_END()

