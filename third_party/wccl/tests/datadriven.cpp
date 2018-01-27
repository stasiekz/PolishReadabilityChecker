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

#include "datadriven.h"

#include <libpwrutils/util.h>
#include <boost/foreach.hpp>
#include <libpwrutils/pathsearch.h>
#include <libcorpus2/util/settings.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/io/xcesreader.h>

#include <libwccl/sentencecontext.h>
#include <libwccl/parser/Parser.h>
#include <libwccl/ops/funexeccontext.h>

#include <libwccl/wcclfile.h>

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

using boost::filesystem::directory_iterator;
using boost::filesystem::exists;
using boost::filesystem::is_directory;
using boost::filesystem::path;
using boost::filesystem::ifstream;


namespace {

struct compare_test
{
	path in_file;
	path search_path;
};

void test_one_item_actual(const compare_test& c)
{
	ifstream ifs_in(c.in_file);

	std::string separators = "=";
	std::string tagset_name = "kipi";
	std::string sentence_filename;
	std::string wholeWccl;
	boost::shared_ptr<Wccl::WcclFile> wcclFile;
	std::string line;
	int line_no = 0;
	while (ifs_in.good() && line != "---") {
		++line_no;
		std::getline(ifs_in, line);
		std::vector<std::string> fields;
		boost::algorithm::split(fields, line, boost::is_any_of(separators));
		if (fields.size() == 2) {
			if (fields[0] == "tagset") {
				tagset_name = fields[1];
			} else if (fields[0] == "sentence") {
				sentence_filename = fields[1];
			} else if (fields[0] == "wholeWccl") {
				wholeWccl = fields[1];
			}
		}
	}
	const Corpus2::Tagset& tagset = Corpus2::get_named_tagset(tagset_name);
	boost::shared_ptr<Corpus2::Sentence> sentence(new Corpus2::Sentence);
	if (!sentence_filename.empty()) {
		path sentence_fullpath = c.search_path / sentence_filename;
		Corpus2::XcesReader reader(tagset, sentence_fullpath.string());
		sentence = reader.get_next_sentence();
		BOOST_REQUIRE(sentence);
	}
	else {
		//
		Corpus2::Token* the_token = new Corpus2::Token(".", PwrNlp::Whitespace::ManySpaces);
		Corpus2::Tag t1(Corpus2::mask_t(0));
		Corpus2::Lexeme l1("aaa", t1);
		the_token->add_lexeme(l1);
		sentence->append(the_token);

		BOOST_REQUIRE(sentence);
	}
	Wccl::SentenceContext sc(sentence);

	std::string operator_string, expected_output;

	int lexCounter = 0;
	while (ifs_in.good()) {
		++line_no;
		std::getline(ifs_in, line);
		if (line == "" && operator_string != "") {
			++line_no;
			std::getline(ifs_in, line);
			expected_output = line;
			Wccl::Parser parser(tagset);
			boost::shared_ptr<Wccl::FunctionalOperator> parsed;
			try {
				if(wholeWccl.size() == 0){
					parsed = parser.parseAnyOperator(operator_string);
				}else{
					path wholeWcclPath = c.search_path / wholeWccl;
					wcclFile = parser.parseWcclFileFromPath(wholeWcclPath.string(), c.search_path.string());
					parsed = wcclFile->get_untyped_op_ptr("indecl", lexCounter);
					operator_string = "operator number ";
					char lexCounterStr[10];
					sprintf(lexCounterStr, "%d", lexCounter);
					operator_string += lexCounterStr;
					operator_string += " defined in file: ";
					operator_string += wholeWcclPath.string();
					lexCounter++;
				}
			} catch (Wccl::WcclError& e) {
				std::cerr << e.info() << "\n---\n" << operator_string << "\n---\n";
				throw;
			}
			std::string output = parsed->base_apply(sc)->to_string(tagset);
			if (output != expected_output) {
				BOOST_ERROR("Mismatch on line " << line_no << ":\n"
				 << "Expected: " << expected_output << "\n"
				 << "Actual  : " << output << "\n"
				 << "Operator: " << operator_string);
			}
			expected_output = "";
			operator_string = "";
			while (ifs_in.good() && line != "---" && line != "") {
				++line_no;
				std::getline(ifs_in, line);
				std::vector<std::string> fields;
				boost::algorithm::split(fields, line, boost::is_any_of(separators));
				if (fields.size() == 2) {
					try {
						const Wccl::Value& v = (*parsed)[fields[0]];

						if (v.to_string(tagset) != fields[1]) {
							BOOST_ERROR("Variable " << fields[0]
													<< " value mismatch on line "
													<< line_no << "\n: expected " << fields[1]
													<< " got " << v.to_string(tagset));
						}
					} catch (Wccl::InvalidVariableName &e) {
						BOOST_ERROR("Invalid variable name in test: "
												<< fields[0] << " on line " << line_no);
					}
				}
			}
			BOOST_REQUIRE(line == "---" || line == "");
		} else {
			if (operator_string.empty() && line.substr(0, 9) == "position=") {
				std::string new_position = line.substr(9);
				sc.set_position(atoi(new_position.c_str()));
			} else {
				operator_string += line + "\n";
			}
		}
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
	std::set<std::string> txt_tests;
	std::set<path> subdirs;

	for (directory_iterator itr(dir); itr != end_itr; ++itr) {
		if (is_directory(itr->status())) {
			subdirs.insert(itr->path());
		} else {
			if (itr->path().extension() == ".ccl") {
				txt_tests.insert(itr->path().string());
			}
		}
	}
	BOOST_FOREACH (const std::string& s, txt_tests) {
		compare_test c;
		c.in_file = s;
		c.search_path = dir;
		tests.push_back(c);
		++count;
	}
	BOOST_TEST_MESSAGE("Found " << count << " valid data test case"
		<< (count > 1 ? "s" : "")
		<< " in " << dir
		<< " [" << ps << "]"
		);
	BOOST_FOREACH (const path& s, subdirs) {
		count += init_subdir(s, ps, tests);
	}
	return count;
}

void test_one_item(const compare_test& c)
{
	try {
		test_one_item_actual(c);
	} catch (PwrNlp::PwrNlpError& e) {
		BOOST_ERROR("Caught " << e.scope() << " exception: \n" << e.info());
	}
}

}

void init_data_suite(boost::unit_test::test_suite *ts, const std::string& path)
{
	std::string subdir_name = LIBWCCL_TEST_DATA_DIR "data";
	if (!path.empty()) {
		subdir_name = path;
	}
	if (!exists(subdir_name)) {
		BOOST_TEST_MESSAGE("Test data subdir does not exist");
	}
	std::vector<compare_test> compares;
	init_subdir(subdir_name, "", compares);
	BOOST_FOREACH (const compare_test& ci, compares) {
		std::string rel_path = boost::algorithm::replace_first_copy(
				ci.in_file.string(), subdir_name, "");
		std::string name = "data_test:" + rel_path;
		ts->add(boost::unit_test::make_test_case(
			boost::bind(test_one_item, ci), name));
	}
}
