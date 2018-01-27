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

#include <cstdlib>
#include <fstream>
#include <iomanip>

#include <libwccl/values/strset.h>
#include <libwccl/parser/Parser.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/util/tokentimer.h>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <libcorpus2/io/reader.h>
#include <libcorpus2/io/writer.h>

// not checking for HAVE_VERSION..., no reason for its absence
#include <libwccl/version.h>

namespace {
	bool quiet = false;

	struct options {
		bool first;
	};
}

class RuleRunner
{
public:
	RuleRunner(const Corpus2::Tagset& tagset)
		: tagset_(tagset), parser_(tagset_), progress_(false), search_path_(".")
		, tag_rule_iterations_(0), total_match_rules_(0), total_tag_rules_(0)
	{
	}

	void use_progress(bool use) {
		progress_ = use;
		if (use) {
			Corpus2::TokenTimer& timer = Corpus2::global_timer();
			timer.register_signal_handler();
		}
	}

	void set_tag_rule_iterations(int i) {
		tag_rule_iterations_ = i;
	}

	std::pair<int,int> load_more_rules(const std::string &filename);

	void apply_rules(boost::shared_ptr<Corpus2::TokenReader> reader,
		boost::shared_ptr<Corpus2::TokenWriter> writer);

	bool empty() const {
		return size() == 0;
	}

	size_t size() const {
		return total_match_rules_ + total_tag_rules_;
	}

	size_t total_match_rules() const {
		return total_match_rules_;
	}

	size_t total_tag_rules() const {
		return total_tag_rules_;
	}

	void set_search_path(const std::string& path) {
		search_path_ = path;
	}

private:
	const Corpus2::Tagset& tagset_;
	Wccl::Parser parser_;
#ifdef __unix__
	std::vector<std::string> file_names_;
#else
	std::vector<std::wstring> file_names_;
#endif
	std::vector<boost::shared_ptr<Wccl::WcclFile> > parsed_files_;
	bool progress_;
	std::string search_path_;
	int tag_rule_iterations_;
	size_t total_match_rules_, total_tag_rules_;
};

std::pair<int,int> RuleRunner::load_more_rules(const std::string& filename)
{
	boost::shared_ptr<Wccl::WcclFile> parsed_file;
	try {
		std::ifstream is(filename.c_str());
		if (!is.good()) {
			throw Wccl::FileNotFound(filename, "", __FUNCTION__);
		}
		parsed_file = parser_.parseWcclFile(is, search_path_);
		if (parsed_file) {
			boost::filesystem::path p(filename);
			file_names_.push_back(p.stem().c_str());
			size_t match_rules = 0, tag_rules = 0;
			if (parsed_file->has_tag_rules()) {
				tag_rules = parsed_file->get_tag_rules().size();
			}
			if (parsed_file->has_match_rules()) {
				match_rules = parsed_file->get_match_rules().size();
			}
			total_tag_rules_ += tag_rules;
			total_match_rules_ += match_rules;
			parsed_files_.push_back(parsed_file);
			return std::make_pair(tag_rules, match_rules);
		} else {
			std::cerr << "Problem while parsing -- "
				<< "parser returned NULL!" << std::endl;
		}
	} catch (PwrNlp::PwrNlpError& e) {
		std::cerr << e.scope() << " Error: " << e.info() << std::endl;
	}
	return std::make_pair(0,0);
}

void RuleRunner::apply_rules(boost::shared_ptr<Corpus2::TokenReader> reader,
	boost::shared_ptr<Corpus2::TokenWriter> writer)
{
	Corpus2::TokenTimer& timer = Corpus2::global_timer();
	while (boost::shared_ptr<Corpus2::Chunk> c = reader->get_next_chunk()) {
		BOOST_FOREACH (boost::shared_ptr<Corpus2::Sentence>& s, c->sentences()) {
			boost::shared_ptr<Corpus2::AnnotatedSentence> as;
			as = boost::dynamic_pointer_cast<Corpus2::AnnotatedSentence>(s);
			if (!as) {
				std::cerr << "Did not get an AnnotatedSentence from reader,"
					"'ann'' option broken?\n";
				return;
			}

			BOOST_FOREACH (boost::shared_ptr<Wccl::WcclFile>& f, parsed_files_) {
				if (f->has_tag_rules()) {
					if (tag_rule_iterations_ == 0) {
						f->get_tag_rules_ptr()->execute_once(as);
					} else if (tag_rule_iterations_ < 0) {
						f->get_tag_rules_ptr()->execute_until_done(as);
					} else  {
						f->get_tag_rules_ptr()->execute_until_done(as, tag_rule_iterations_);
					}
				}
				if (f->has_match_rules()) {
					f->get_match_rules_ptr()->apply_all(as);
				}
			}

			timer.count_sentence(*as);
			if (progress_) {
				timer.check_slice();
			}
			//writer->write_sentence(*as);
		}
		writer->write_chunk(*c);
	}
}

void usage(char* name)
{
	std::cerr << "This program runs WCCL match and/or tag rules. Tag rules are applied first.\n";
	std::cerr << "Usage " << name << " [OPTIONS] FILES\n"
		<< "Files ending with .xml are treated as corpora, otherwise "
		<< "as WCCL files. Use - to read corpus from stdin (as with -I)\n"
		<< "Note: the ,ann option is implied on all input formats\n";
}

int main(int argc, char** argv)
{
	std::string tagset_load = "kipi";
	std::string input_format;
	std::string output_format;
	std::string search_path;
	bool progress = false;
	options opts;
	opts.first = false;
	std::vector<std::string> corpora_files, ccl_files, files;
	bool corpus_stdin = true;
	using boost::program_options::value;

	std::string readers = boost::algorithm::join(Corpus2::TokenReader::available_reader_types_help(), " ");
	std::string readers_help = "Input format, any of: " + readers + "\n";
	std::string writers = boost::algorithm::join(Corpus2::TokenWriter::available_writer_types_help(), " ");
	std::string writers_help = "Output format, any of: " + writers + "\n";;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("tagset,t", value(&tagset_load),
			 "Tagset to use\n")
			("corpus,c", value(&corpora_files),
			 "Corpus file to load (XCES), do not load from stdin\n")
			("ccl-file,C", value(&ccl_files),
			 "CCL rule files\n")
			("files,f", value(&files),
			 "Files to load, looking at the extension to determine type\n")
			("search-path,P", value(&search_path),
			 "WCCL resources (lexicons) search path")
			("input-path,I", value(&corpora_files)->composing(),
			 "Corpus paths, use '-' for stdin")
			("input-format,i", value(&input_format)->default_value("xces"),
			 readers_help.c_str())
			("output-format,o", value(&output_format)->default_value("ccl"),
			 writers_help.c_str())
			("progress,p", value(&progress)->zero_tokens(),
			 "Show progress info")
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress messages\n")
			("until-done-iterations,u", value<int>()->implicit_value(1000),
			 "Until-done iteration limit, no arg for default limit(1000)\n")
			("first-sentence-only,1", value(&opts.first)->zero_tokens(),
			 "Only process first sentence\n")
			("help,h", "Show help")
			("version", "print version string")
			;
	boost::program_options::variables_map vm;
	boost::program_options::positional_options_description p;
	p.add("files", -1);

	try {
		boost::program_options::store(
			boost::program_options::command_line_parser(argc, argv)
			.options(desc).positional(p).run(), vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << std::endl;
		return 2;
	}
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		usage(argv[0]);
		std::cout << desc << "\n";
		return 1;
	}

	if (vm.count("version")) {
		std::cout << "wccl-rules (libwccl) " << LIBWCCL_VERSION << "\n";
		return 0;
	}

	BOOST_FOREACH (const std::string& f, files) {
		if (f == "-") {
			corpus_stdin = true;
		} else if (boost::algorithm::ends_with(f, ".xml")) {
			corpora_files.push_back(f);
		} else {
			ccl_files.push_back(f);
		}
	}
	corpus_stdin = corpus_stdin || (corpora_files.end() != std::find(
		corpora_files.begin(), corpora_files.end(), std::string("-")));
	corpora_files.erase(std::remove(
		corpora_files.begin(), corpora_files.end(), std::string("-")),
		corpora_files.end());
	// consider stdin only when no corpus files given
	corpus_stdin = corpus_stdin && corpora_files.empty();

	if (ccl_files.empty() || (corpora_files.empty() && !corpus_stdin)) {
		usage(argv[0]);
		return 2;
	}

	try {
		const Corpus2::Tagset& tagset = Corpus2::get_named_tagset(tagset_load);
		RuleRunner runner(tagset);
		if (vm.count("until-done-iterations")) {
			runner.set_tag_rule_iterations(vm["until-done-iterations"].as<int>());
		}
		runner.use_progress(progress);
		if (!search_path.empty()) {
			runner.set_search_path(search_path);
		}
		BOOST_FOREACH (const std::string& file, ccl_files) {
			std::pair<int,int> res = runner.load_more_rules(file);
			if (res.first == 0 && res.second == 0) {
				std::cerr << "Warning: no rules loaded from " << file << "\n";
			} else if (!quiet) {
				std::cerr << "Loaded " << res.first << " tag rule(s) and "
					<< res.second << " match rule(s) from " << file << "\n";
			}
		}
		if (!runner.empty()) {
			boost::shared_ptr<Corpus2::TokenWriter> writer;
			writer = Corpus2::TokenWriter::create_stream_writer(output_format, std::cout, tagset);
			boost::shared_ptr<Corpus2::TokenReader> reader;
			BOOST_FOREACH (std::string cf, corpora_files) {
				reader = Corpus2::TokenReader::create_path_reader(input_format, tagset, cf);
				reader->set_option("ann");
				runner.apply_rules(reader, writer);
			}
			if (corpus_stdin) {
				reader = Corpus2::TokenReader::create_stream_reader(input_format, tagset, std::cin);
				reader->set_option("ann");
				runner.apply_rules(reader, writer);
			}
			if (progress) {
				Corpus2::TokenTimer& timer = Corpus2::global_timer();
				timer.stats();
			}
		}
	} catch (PwrNlp::PwrNlpError& e) {
		std::cerr << e.scope() << "error: " << e.info() << std::endl;
		return 2;
	}

	return 0;
}
