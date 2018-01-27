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

#include <antlr/RecognitionException.hpp>

#include <libpwrutils/util.h>
#include <boost/foreach.hpp>
#include <libpwrutils/pathsearch.h>
#include <libcorpus2/util/settings.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/io/xcesreader.h>
#include <libcorpus2/io/cclreader.h>
#include <libcorpus2/ann/annotatedsentence.h>

#include <libwccl/sentencecontext.h>
#include <libwccl/parser/Parser.h>
#include <libwccl/ops/funexeccontext.h>

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

struct rule_compare_test
{
	std::string tagset;
	path corpus_file;
	path rule_file;
	path output_corpus;
};

typedef std::map<std::string, boost::shared_ptr<Corpus2::Chunk> > corpus_cache_t;
corpus_cache_t corpus_cache;

boost::shared_ptr<Corpus2::Chunk> get_corpus(const std::string& path, const Corpus2::Tagset& tagset)
{
	corpus_cache_t::const_iterator i;
	i = corpus_cache.find(path);
	if (i != corpus_cache.end()) {
		return i->second;
	} else {
		boost::shared_ptr<Corpus2::Chunk> chunk;
		boost::filesystem::path fp(path);
		std::string fn = fp.filename().c_str();
		if (fn.find("ccl") == fn.npos) {
			Corpus2::XcesReader xr(tagset, path);
			chunk = xr.get_next_chunk();
		} else {
			Corpus2::CclReader cr(tagset, path);
			chunk = cr.get_next_chunk();
		}
		corpus_cache.insert(std::make_pair(path, chunk));
		return chunk;
	}
}

void dump_lexemes(const std::set<Corpus2::Lexeme>& lex, std::set<Corpus2::Lexeme>& expected_lex, const Corpus2::Tagset& tagset)
{
	std::set<Corpus2::Lexeme>::const_iterator i = lex.begin();
	std::set<Corpus2::Lexeme>::const_iterator ei = expected_lex.begin();
	while (i != lex.end() && ei != expected_lex.end()) {
		if (*i < *ei) {
			std::cerr << "EXTRA:   " << i->lemma_utf8() << "\t" << tagset.tag_to_string(i->tag()) << "\n";
			++i;
		} else if (*i > *ei) {
			std::cerr << "MISSING: " << ei->lemma_utf8() << "\t" << tagset.tag_to_string(ei->tag()) << "\n";
			++ei;
		} else if (*i == *ei) {
			std::cerr << "OK:      " << i->lemma_utf8() << "\t" << tagset.tag_to_string(i->tag()) << "\n";
			++i;
			++ei;
		} else {
			std::cerr << "DARK FORCES AT PLAY: "
				<< i->lemma_utf8() << "\t" << tagset.tag_to_string(i->tag()) << "\t"
				<< ei->lemma_utf8() << "\t" << tagset.tag_to_string(ei->tag()) << "\n";
		}
	}
	while (i != lex.end()) {
		std::cerr << "EXTRA:   " << i->lemma_utf8() << "\t" << tagset.tag_to_string(i->tag()) << "\n";
		++i;
	}
	while (ei != expected_lex.end()) {
		std::cerr << "MISSING: " << ei->lemma_utf8() << "\t" << tagset.tag_to_string(ei->tag()) << "\n";
		++ei;
	}
}

void check_sentences(int sentence_i, const Corpus2::Tagset& tagset,
	const Corpus2::Sentence::Ptr& sentence,
	const Corpus2::Sentence::Ptr& expected_sentence)
{
	BOOST_REQUIRE_EQUAL(sentence->size(), expected_sentence->size());
	for (size_t ti = 0; ti < sentence->size(); ++ti) {
		Corpus2::Token& token = *sentence->tokens()[ti];
		Corpus2::Token& expected_token = *expected_sentence->tokens()[ti];
		BOOST_CHECK_EQUAL(token.orth_utf8(), expected_token.orth_utf8());
		std::set<Corpus2::Lexeme> lex;
		std::copy(token.lexemes().begin(), token.lexemes().end(),
			std::inserter(lex, lex.begin()));
		std::set<Corpus2::Lexeme> expected_lex;
		std::copy(expected_token.lexemes().begin(), expected_token.lexemes().end(),
			std::inserter(expected_lex, expected_lex.begin()));
		if (lex != expected_lex) {
			BOOST_ERROR("Lexeme mismatch in sentence " << sentence_i
				<< ", token " << ti << " [" << expected_token.orth_utf8() << "]"
				<< " (" << lex.size() << ", expected " << expected_lex.size() << ")");
			dump_lexemes(lex, expected_lex, tagset);
		}
	}
	boost::shared_ptr<Corpus2::AnnotatedSentence> annotated, expected_annotated;
	annotated = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(sentence);
	expected_annotated = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(expected_sentence);
	if (expected_annotated) {
		BOOST_REQUIRE(annotated);
		BOOST_FOREACH (const Corpus2::AnnotatedSentence::chan_map_t::value_type& v, expected_annotated->all_channels()) {
			std::string channel_name = v.first;
			BOOST_REQUIRE_MESSAGE(annotated->has_channel(channel_name), "Expected channel " + channel_name);
			const Corpus2::AnnotationChannel& expected_channel = v.second;
			std::string expected_channel_data = channel_name + ":" + expected_channel.dump_alpha();
			Corpus2::AnnotationChannel& channel = annotated->get_channel(channel_name);
			std::string channel_data = channel_name + ":" + channel.dump_alpha();
			BOOST_CHECK_EQUAL(channel_data, expected_channel_data);
		}
	}
}

void test_one_rule_item_actual(const rule_compare_test& c)
{
	const Corpus2::Tagset& tagset = Corpus2::get_named_tagset(c.tagset);
	boost::shared_ptr<Corpus2::Chunk> chunk = get_corpus(c.corpus_file.string(), tagset);
	if (!chunk) {
		BOOST_ERROR("Empty chunk loaded");
	}
	boost::shared_ptr<Corpus2::TokenReader> reader;
	std::string ofn = c.output_corpus.filename().c_str();
	if (ofn.find("ccl") != ofn.npos || ofn.find("match") != ofn.npos) {
		reader = Corpus2::TokenReader::create_path_reader("ccl", tagset, c.output_corpus.string());
	} else {
		reader = Corpus2::TokenReader::create_path_reader("xces", tagset, c.output_corpus.string());
	}
	reader->set_option("loose");
	boost::shared_ptr<Corpus2::Chunk> expected = reader->get_next_chunk();
	BOOST_REQUIRE_EQUAL(chunk->sentences().size(), expected->sentences().size());
	Wccl::Parser parser(tagset);
	std::string rf = c.rule_file.string();
	std::ifstream is(rf.c_str());
	BOOST_REQUIRE(is.good());

	boost::shared_ptr<Wccl::WcclFile> parsed;
	parsed = parser.parseWcclFile(is, LIBWCCL_TEST_DATA_DIR);

	for (size_t i = 0; i < chunk->sentences().size(); ++i) {
		Corpus2::Sentence::Ptr sentence = chunk->sentences()[i]->clone_shared();
		Corpus2::Sentence::Ptr expected_sentence = expected->sentences()[i];
		BOOST_REQUIRE_EQUAL(sentence->size(), expected_sentence->size());
		if (parsed->has_tag_rules()) {
			parsed->get_tag_rules_ptr()->execute_once(sentence);
		}
		if (parsed->has_match_rules()) {
			boost::shared_ptr<Corpus2::AnnotatedSentence> annotated;
			annotated = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(sentence);
			BOOST_REQUIRE(annotated);
			parsed->get_match_rules_ptr()->apply_all(annotated);
		}
		check_sentences(i, tagset, sentence, expected_sentence);
	}
}


struct init_status
{
	path input;
	std::string tagset;
};

int init_subdir(const path& dir, std::string ps, std::vector<rule_compare_test>& tests,
	init_status status = init_status())
{
	int count = 0;
	ps += dir.string();
	if (!ps.empty()) {
		ps += Corpus2::Path::Instance().get_path_separator();
	}

	directory_iterator end_itr; // default-constructed is past-the-end
	std::set<path> txt_tests;
	std::set<path> subdirs;

	for (directory_iterator itr(dir); itr != end_itr; ++itr) {
		if (is_directory(itr->status())) {
			subdirs.insert(itr->path());
		} else {
			if (itr->path().extension() == ".ccl") {
				txt_tests.insert(itr->path());
			} else if (itr->path().extension() == ".xml") {
				if (!boost::algorithm::ends_with(itr->path().stem().c_str(), ".out")) {
					status.input = itr->path();
				}
			} else if (itr->path().extension() == ".is-the-tagset") {
				status.tagset = itr->path().stem().c_str();
			}
		}
	}
	BOOST_FOREACH (const path& s, txt_tests) {
		path o = s;
		o.replace_extension(".out.xml");
		if (boost::filesystem::is_regular_file(o)) {
			rule_compare_test c;
			c.corpus_file  = status.input;
			c.rule_file = s;
			c.output_corpus = o;
			c.tagset = status.tagset;
			tests.push_back(c);
			++count;
		} else {
			std::cerr << "No output file: " << o.string() << "\n";
		}
	}
	BOOST_TEST_MESSAGE("Found " << count << " valid data test case"
		<< (count > 1 ? "s" : "")
		<< " in " << dir
		<< " [" << ps << "]"
		);
	BOOST_FOREACH (const path& s, subdirs) {
		count += init_subdir(s, ps, tests, status);
	}
	return count;
}

void test_one_rule_item(const rule_compare_test& c)
{
	try {
		test_one_rule_item_actual(c);
	} catch (PwrNlp::PwrNlpError& e) {
		BOOST_ERROR("Caught " << e.scope() << " exception: \n" << e.info());
	} catch (antlr::RecognitionException &e) {
		BOOST_ERROR("Caught " << "ANTLR" << " exception: \n"
			<< e.getFileLineColumnString()
			<< " " << e.getMessage());
	} catch (antlr::ANTLRException &e) {
		BOOST_ERROR("Caught " << "ANTLR" << " exception: \n"
			<< " " << e.getMessage());
	}
}

}

void init_data_rule_suite(boost::unit_test::test_suite *ts, const std::string& path)
{
	std::string subdir_name = LIBWCCL_TEST_DATA_DIR "rules-data";
	if (!path.empty()) {
		subdir_name = path;
	}
	if (!exists(subdir_name)) {
		BOOST_TEST_MESSAGE("Rules test data subdir does not exist");
	}
	std::vector<rule_compare_test> compares;
	init_subdir(subdir_name, "", compares);
	BOOST_FOREACH (const rule_compare_test& ci, compares) {
		std::string rel_path = boost::algorithm::replace_first_copy(
				ci.rule_file.string(), subdir_name, "");
		std::string name = "rule_data_test:" + rel_path;
		//std::cout << name << "\n";
		ts->add(boost::unit_test::make_test_case(
			boost::bind(test_one_rule_item, ci), name));
	}
}

