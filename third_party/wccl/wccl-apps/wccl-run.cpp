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
#include <cstdio>
#include <fstream>
#include <iomanip>


#include <libwccl/values/strset.h>
#include <libwccl/parser/Parser.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/util/tokentimer.h>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <libcorpus2/io/xcesreader.h>
#include <boost/lexical_cast.hpp>

#include <antlr/NoViableAltException.hpp>
#include <antlr/MismatchedTokenException.hpp>

// not checking for HAVE_VERSION..., no reason for its absence
#include <libwccl/version.h>

namespace {
	bool quiet = false;
	bool tabs = false;
	bool output_orths = true;
	bool output_variables = false;
	bool global_numbering = false;
	bool sentence_indices = true;
	bool output_header = true;
	bool in_sentence_numbering = true;
}

class streamsave
{
public:
	streamsave(std::ostream& os) : os_(os), flags_(os.flags()) {}
	~streamsave() { os_.flags(flags_); }
private:
	std::ostream& os_;
	std::ios_base::fmtflags flags_;
};

class Runner
{
public:
	Runner(const Corpus2::Tagset& tagset)
	 : tagset_(tagset), parser_(tagset_), token_idx(0), progress_(false),
	   search_path_("."), want_header_(true), aggregate_output_(false)
	{
	}

	void use_progress(bool use) {
		progress_ = use;
		if (use) {
			Corpus2::TokenTimer& timer = Corpus2::global_timer();
			timer.register_signal_handler();
		}
	}

	void set_aggregate_output(bool v) { aggregate_output_ = v; }

	bool load_more_operators(const std::string &filename);

	bool load_operator_string(const std::string &op_string);

	size_t size() const {
		return ops_.size() + (filter_op_ ? 1 : 0);
	}

	const std::vector< boost::shared_ptr<Wccl::FunctionalOperator> >& operators() const {
		return ops_;
	}

	void do_sentence(const boost::shared_ptr<Corpus2::Sentence>& sentence,
			std::vector< std::vector< UnicodeString > >& outputs, int sidx);

	void run(boost::shared_ptr<Corpus2::TokenReader> , bool first);

	void output_tabular(const std::vector< std::vector< UnicodeString > >& outputs);

	void set_search_path(const std::string& path) {
		search_path_ = path;
	}

	void set_filter_op(const std::string op_name, const std::string& op_value) {
		filter_op_name_ = op_name;
		filter_op_value_ = op_value;
	}

private:
	void do_operator_variables(const boost::shared_ptr<Wccl::FunctionalOperator>& op,
			std::vector<UnicodeString>& out, bool variables);

	const Corpus2::Tagset& tagset_;
	Wccl::Parser parser_;
	std::vector< boost::shared_ptr<Wccl::FunctionalOperator> > ops_;
	boost::shared_ptr< Wccl::FunctionalOperator > filter_op_;
	std::vector< std::string > op_names_;
	int token_idx;
	bool progress_;
	std::string search_path_;
	std::string filter_op_name_;
	std::string filter_op_value_;
	bool want_header_;
	bool aggregate_output_;
};

bool Runner::load_more_operators(const std::string& filename)
{
	boost::shared_ptr<Wccl::WcclFile> retOp;
	try {
		std::ifstream is(filename.c_str());
		if (!is.good()) {
			throw Wccl::FileNotFound(filename, "", __FUNCTION__);
		}

		retOp = parser_.parseWcclFile(is, search_path_);
		if (retOp) {
			boost::filesystem::path p(filename);
			std::string prefix = ""; //p.stem() + ":";
			Wccl::UntypedOpSequence::name_op_v_t pairs = retOp->gen_all_op_pairs();
			BOOST_FOREACH (const Wccl::UntypedOpSequence::name_op_pair_t v, pairs) {
				std::string opname = v.first;
				if (opname == filter_op_name_) {
					filter_op_ = v.second;
				} else {
					op_names_.push_back(opname);
					ops_.push_back(v.second);
				}
			}
			return true;
		} else {
			std::cerr << "Problem while parsing -- "
				<< "parser returned NULL!" << std::endl;
		}
	} catch (antlr::MismatchedTokenException &e) {
		std::cerr << e.getFileLineColumnString()
				<< " " << e.getMessage() << std::endl;
	} catch(antlr::NoViableAltException &e) {
		std::cerr << e.getFileLineColumnString()
				<< " " << e.getMessage() << std::endl;
	} catch (Wccl::InvalidVariableName &e) {
		std::cerr << "Wccl::InvalidVariableName " << e.info() << std::endl;
	} catch (Wccl::VariableTypeMismatch &e) {
		std::cerr << "Wccl::VariableTypeMismatch " << e.info() << std::endl;
	} catch (Wccl::WcclError& e) {
		std::cerr << "Wccl::WcclError:" << e.info() << std::endl;
	} catch (PwrNlp::PwrNlpError& e) {
		std::cerr << "PwrNlp::PwrNlpError " << e.info() << std::endl;
	} catch (antlr::ANTLRException& e) {
		std::cerr << "Antlr error " << e.getMessage() << std::endl;
	}
	return false;
}

bool Runner::load_operator_string(const std::string& op_string)
{
	boost::shared_ptr<Wccl::FunctionalOperator> retOp;
	try {
		retOp = parser_.parseAnyOperator(op_string);
		if (retOp) {
			op_names_.push_back(op_string);
			ops_.push_back(retOp);
			return true;
		} else {
			std::cerr << "Problem while parsing -- "
				<< "parser returned NULL!" << std::endl;
		}
	} catch (PwrNlp::PwrNlpError& e) {
		std::cerr << e.scope() << " error: " << e.info() << std::endl;
	} catch (antlr::ANTLRException& e) {
		std::cerr << "Antlr error " << e.getMessage() << std::endl;
	}
	return false;
}

void Runner::do_operator_variables(const boost::shared_ptr<Wccl::FunctionalOperator>& op,
		std::vector<UnicodeString>& out, bool variables)
{
	if (variables) {
		BOOST_FOREACH (const std::string& varname, op->valid_variable_names()) {
			out.push_back((*op)[varname].to_string_u(tagset_));
		}
	}
}

void Runner::do_sentence(const boost::shared_ptr<Corpus2::Sentence>& sentence,
		std::vector< std::vector< UnicodeString > >& outputs, int sidx)
{
	Wccl::SentenceContext sc(sentence);
	streamsave sv(std::cout);

	if (output_header && want_header_) {
		outputs.resize(outputs.size() + 1);
		std::vector< UnicodeString >& out = outputs.back();
		if (global_numbering) {
			out.push_back(UnicodeString::fromUTF8("##"));
		}
		if (sentence_indices) {
			out.push_back(UnicodeString::fromUTF8("S#"));
		}
		if (in_sentence_numbering) {
			out.push_back(UnicodeString::fromUTF8("#"));
		}
		if (output_orths) {
			out.push_back(UnicodeString::fromUTF8("orth"));
		}
		if (filter_op_) {
			out.push_back(UnicodeString::fromUTF8(filter_op_name_));
			if (output_variables) {
				boost::shared_ptr<Wccl::FunctionalOperator> o = filter_op_;
				BOOST_FOREACH (const std::string& varname, o->valid_variable_names()) {
					const Wccl::Value& value = (*o)[varname];
					std::string label = "(" + filter_op_name_ + ")" + value.make_var_repr(varname);
					out.push_back(UnicodeString::fromUTF8(label));
				}
			}
		}
		for (size_t i = 0; i < op_names_.size(); ++i) {
			out.push_back(UnicodeString::fromUTF8(op_names_[i]));
			if (output_variables) {
				boost::shared_ptr<Wccl::FunctionalOperator> o = ops_[i];
				BOOST_FOREACH (const std::string& varname, o->valid_variable_names()) {
					const Wccl::Value& value = (*o)[varname];
					std::string label = "(" + op_names_[i] + ")" + value.make_var_repr(varname);
					out.push_back(UnicodeString::fromUTF8(label));
				}
			}
		}
	}
	for (size_t i = 0; i < sentence->size(); ++i) {
		++token_idx;
		sc.set_position(i);
		UnicodeString vstr;
		if (filter_op_) {
			filter_op_->clean();
			boost::shared_ptr<const Wccl::Value> v = filter_op_->base_apply(sc);
			vstr = v->to_string_u(tagset_);
			std::string uvstr = PwrNlp::to_utf8(vstr);
			if (uvstr != filter_op_value_) {
				continue;
			} else {
			}
		}
		outputs.resize(outputs.size() + 1);
		std::vector< UnicodeString >& out = outputs.back();

		if (global_numbering) {
			out.push_back(UnicodeString::fromUTF8(boost::lexical_cast<std::string>(token_idx)));
		}
		if (sentence_indices) {
			out.push_back(UnicodeString::fromUTF8(boost::lexical_cast<std::string>(sidx)));
		}
		if (in_sentence_numbering) {
			out.push_back(UnicodeString::fromUTF8(boost::lexical_cast<std::string>(i + 1)));
		}
		if (output_orths) {
			out.push_back(sentence->tokens()[i]->orth());
		}
		if (filter_op_) {
			out.push_back(vstr);
			do_operator_variables(filter_op_, out, output_variables);
		}

		BOOST_FOREACH (const boost::shared_ptr<Wccl::FunctionalOperator>& o, ops_) {
			o->clean();
			boost::shared_ptr<const Wccl::Value> v = o->base_apply(sc);
			UnicodeString vstr = v->to_string_u(tagset_);
			out.push_back(vstr);
			do_operator_variables(o, out, output_variables);
		}
	}
}

void Runner::output_tabular(const std::vector<std::vector<UnicodeString> >& outputs)
{
	std::vector<int> lengths(outputs[0].size());
	BOOST_FOREACH (const std::vector< UnicodeString >& line, outputs) {
		if (line.empty()) continue;
		for (size_t i = 0; i < line.size(); ++i) {
			lengths[i] = std::max(lengths[i], line[i].length());
		}
	}

	BOOST_FOREACH (const std::vector< UnicodeString >& line, outputs) {
		if (!line.empty()) {
			for (size_t i = 0; i < line.size() - 1; ++i) {
				UnicodeString u = line[i];
				if (tabs) {
					std::cout << PwrNlp::to_utf8(line[i]) << "\t";
				} else {
					u.padTrailing(lengths[i] + 1);
					std::cout << PwrNlp::to_utf8(u);
				}
			}
			std::cout << PwrNlp::to_utf8(line.back());
		}
		std::cout << "\n";
	}
}

void Runner::run(boost::shared_ptr<Corpus2::TokenReader> reader, bool first)
{
	std::vector<std::vector<UnicodeString> > outputs;
	Corpus2::Sentence::Ptr s;
	Corpus2::TokenTimer& timer = Corpus2::global_timer();
	int sidx = 0;
	while ((s = reader->get_next_sentence())) {
		do_sentence(s, outputs, sidx++);
		timer.count_sentence(*s);
		if (aggregate_output_) {
			want_header_ = false;
		}
		if (!outputs.empty() && (tabs ||!aggregate_output_)) {
			output_tabular(outputs);
			outputs.clear();
		}
		if (progress_) {
			timer.check_slice();
		}
		if (!filter_op_) {
			if (tabs || !aggregate_output_) {
				std::cout << "\n";
			} else {
				outputs.resize(outputs.size() + 1);
			}
		}
		if (first) break;
	}
	if (!outputs.empty()) {
		output_tabular(outputs);
	}
}

int main(int argc, char** argv)
{
	std::string tagset_load = "kipi";
	std::string filter_op_name, filter_op_value;
	bool first = false, progress = false;
	std::string input_format;
	std::string search_path;
	std::vector<std::string> corpora_files, files, operator_strings;
	bool corpus_stdin = false;
	bool aggregate = false;
	using boost::program_options::value;
	std::string readers = boost::algorithm::join(Corpus2::TokenReader::available_reader_types_help(), " ");
	std::string readers_help = "Input format, any of: " + readers + "\n";

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("tagset,t", value(&tagset_load),
			 "Tagset to use")
			("corpus,c", value(&corpora_files),
			 "Corpus file to load (XCES)")
			("ccl-operator,C", value(&operator_strings),
			 "CCL operator file or string")
			("files,f", value(&files),
			 "Files to load, looking at the extension to determine type")
			("search-path,P", value(&search_path),
			 "WCCL resources (lexicons) search path")
			("input-path,I", value(&corpora_files)->composing(),
			 "Corpus paths, use '-' for stdin")
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress messages")
			("input-format,i", value(&input_format)->default_value("xces"),
			 readers_help.c_str())
			("first-sentence-only,1", value(&first)->zero_tokens(),
			 "Only process first sentence")
			("tabs", value(&tabs)->zero_tokens(),
			 "Output a tab-separated file")
			("local-counts,l", value(&in_sentence_numbering),
			 "Output in-sentence token counts")
			("global-counts,g", value(&global_numbering),
			 "Output global counts")
			("output-sentence-indices,S", value(&sentence_indices),
			 "Output sentence indices")
			("output-orths,O", value(&output_orths),
			 "Output token orths")
			("output-variables,V", value(&output_variables),
			 "Output operator variables")
			("output-header,H", value(&output_header),
			 "Output table header")
			("filter-operator,F", value(&filter_op_name),
			 "Filter operator name")
			("filter-value", value(&filter_op_value)->default_value("True"),
			 "Filter operator expected value")
			("aggregate-output,A", value(&aggregate),
			 "Aggregate output (prettier, slower)")
			("progress,p", value(&progress)->zero_tokens(),
			 "Show progress info")
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
		std::cerr << "Usage " << argv[0] << " [OPTIONS] FILES\n"
			<< "Files ending with .xml are treated as corpora, otherwise \n"
			<< "as CCL files. Use - to read corpus from stdin (as with -I)\n"
			<< "Files not ending with an extension are treated as raw operator strings\n";
		std::cout << desc << "\n";
		return 1;
	}

	if (vm.count("version")) {
		std::cout << "wccl-run (libwccl) " << LIBWCCL_VERSION << "\n";
		return 0;
	}

	BOOST_FOREACH (const std::string& f, files) {
		if (f == "-") {
			corpus_stdin = true;
		} else if (boost::algorithm::ends_with(f, ".xml")) {
			corpora_files.push_back(f);
		} else {
			operator_strings.push_back(f);
		}
	}
	corpus_stdin = corpus_stdin || (corpora_files.end() != std::find(
		corpora_files.begin(), corpora_files.end(), std::string("-")));
	corpora_files.erase(std::remove(
		corpora_files.begin(), corpora_files.end(), std::string("-")),
		corpora_files.end());
	if ((corpora_files.empty() && !corpus_stdin) || (operator_strings.empty() && !output_orths)) {
		std::cerr << "Nothing to do, try " << argv[0] << " -h\n";
		return 2;
	}
	try {
		const Corpus2::Tagset& tagset = Corpus2::get_named_tagset(tagset_load);
		Runner runner(tagset);
		runner.use_progress(progress);
		if (!search_path.empty()) {
			runner.set_search_path(search_path);
		}
		if (!filter_op_name.empty()) {
			runner.set_filter_op(filter_op_name, filter_op_value);
		}
		runner.set_aggregate_output(aggregate);
		BOOST_FOREACH (const std::string& f, operator_strings) {
			if (boost::algorithm::ends_with(f, ".ccl")) {
				size_t sz = runner.size();
				if (!runner.load_more_operators(f)) {
					std::cerr << "Warning: error while parsing " << f << "\n";
				}
				if (runner.size() == sz) {
					std::cerr << "Warning: no operators loaded from " << f << "\n";
				}
			} else {
				runner.load_operator_string(f);
			}
		}
		if (runner.size() > 0) {
			BOOST_FOREACH (const std::string& f, corpora_files) {
				runner.run(Corpus2::TokenReader::create_path_reader(
						input_format, tagset, f), first);
			}
			if (corpus_stdin) {
				runner.run(Corpus2::TokenReader::create_stream_reader(
						input_format, tagset, std::cin), first);
			}
			if (progress) {
				Corpus2::TokenTimer& timer = Corpus2::global_timer();
				timer.stats();
			}
		}
	} catch (PwrNlp::PwrNlpError& e) {
		std::cerr << e.info() << std::endl;
		return 2;
	}

	return 0;
}
