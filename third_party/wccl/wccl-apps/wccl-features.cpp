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
#include <libwccl/values/bool.h>
#include <libwccl/values/tset.h>
#include <libwccl/values/strset.h>

// not checking for HAVE_VERSION..., no reason for its absence
#include <libwccl/version.h>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <libcorpus2/io/xcesreader.h>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <antlr/NoViableAltException.hpp>
#include <antlr/MismatchedTokenException.hpp>

namespace {
	bool quiet = false;
	bool tabs = false;
	bool output_orths = true;
	bool output_variables = false;
	bool global_numbering = false;
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

class FeatureRunner
{
public:
	FeatureRunner(const Corpus2::Tagset& tagset)
	 : tagset_(tagset), parser_(tagset_), token_idx(0)
	{
	}

	int load_more_operators(const std::string &filename);

	int load_operator_string(const std::string &line);

	void print_header_head();
	void print_header_body(const std::string &attribute_prefix,
		bool nos = false);
	void print_header_foot();

	void print_data(const std::vector< std::vector<std::string> >& data);
	void print_data(const std::vector<std::vector<std::string> > &data,
		const std::vector<bool> rowmask);

	std::vector< std::vector<std::string> > do_sentence(
		const boost::shared_ptr<Corpus2::Sentence>& sentence);

	void do_sentence(
		const boost::shared_ptr<Corpus2::Sentence>& sentence,
		std::vector< std::vector<std::string> >& sfeats,
		bool nos = false);

	void do_stream(std::istream& is, bool first);

	void do_files(std::vector<std::string>& files, bool first);

	bool empty() {
		return bool_ops_.empty() && str_ops_.empty() && tset_ops_.empty();
	}

private:
	const Corpus2::Tagset& tagset_;
	Wccl::Parser parser_;

	typedef std::map<
		std::string,
		boost::shared_ptr<Wccl::Operator<Wccl::Bool> >
	> bool_ops_map_t;
	bool_ops_map_t bool_ops_;
	typedef std::map<
		std::string,
		boost::shared_ptr<Wccl::Operator<Wccl::StrSet> >
	> str_ops_map_t;
	str_ops_map_t str_ops_;

	typedef std::map<
		std::string,
		std::pair<std::set<Corpus2::Tag>, boost::shared_ptr<Wccl::Operator<Wccl::TSet> > >
	> tset_ops_map_t;
	tset_ops_map_t tset_ops_;

	int token_idx;
};

int FeatureRunner::load_more_operators(const std::string& filename)
{
	int ops_parsed = 0;

		std::ifstream is(filename.c_str());
		if (!is.good()) {
			throw Wccl::FileNotFound(filename, "", __FUNCTION__);
		}
		std::string line;
		int line_no = 0;
		while (std::getline(is, line)) {
			++line_no;
			if (line.size() < 3) continue;
			if (line[0] == '#') continue;
			int loaded = load_operator_string(line);
			if (loaded > 0) {
				ops_parsed += loaded;
			} else {
				std::cerr << "Line " << line_no << " did not match: " << line << "\n";
			}
		}
	return ops_parsed;
}

int FeatureRunner::load_operator_string(const std::string &line)
{
	int ops_loaded = 0;
	boost::regex e("(STRING|BOOL|MASK\\h([a-z@,]+))\\h+"
			"(?:name:([a-zA-Z0-9_-]+)\\h)?"
			"(?:range:([0-9-]+):([0-9-]+)\\h)?"
			"(.*)");
	boost::smatch what;
	if(boost::regex_match(line, what, e, boost::match_extra)) {
		try {
			const std::string& orig_name = what[3].matched ? what[3] : what[6];
			const std::string& orig_op_string = what[6];
			std::vector <std::string> op_strings;
			std::vector <std::string> names;
			if (what[4].matched) {
				int rfrom = boost::lexical_cast<int>(what[4]);
				int rto = boost::lexical_cast<int>(what[5]);
				for (int i = rfrom; i <= rto; ++i) {
					std::string pos = boost::lexical_cast<std::string>(i);
					op_strings.push_back(boost::algorithm::replace_all_copy(
						orig_op_string, "_R_", pos));
					names.push_back(orig_name + pos);
				}
			} else {
				op_strings.push_back(orig_op_string);
				names.push_back(orig_name);
			}
			for (size_t opi = 0; opi < op_strings.size(); ++opi) {
				const std::string& name = names[opi];
				const std::string& op_string = op_strings[opi];
				if (what[1] == "STRING") {
					str_ops_.insert(std::make_pair(name,
						parser_.parseStringOperator(op_string)));
					++ops_loaded;
				} else if (what[1] == "BOOL") {
					bool_ops_.insert(std::make_pair(name,
						parser_.parseBoolOperator(op_string)));
					++ops_loaded;
				} else {
					Corpus2::Tag tag = tagset_.parse_symbol_string(what[2]);
					std::vector<std::string> sym = tagset_.tag_to_symbol_string_vector(tag, false);
					std::set<Corpus2::Tag> t;
					BOOST_FOREACH (const std::string& s, sym) {
						t.insert(tagset_.parse_symbol(s));
					}
					tset_ops_.insert(std::make_pair(name, std::make_pair(t,
						parser_.parseSymSetOperator(op_string))));
					++ops_loaded;
				}
			}
		} catch (PwrNlp::PwrNlpError& e) {
			std::cerr << e.scope() <<  " error: " << e.info() << std::endl;
		}
	}
	return ops_loaded;
}

void FeatureRunner::print_header_head()
{
	std::cout << "% Generated by wccl-features\n";
	std::cout << "@RELATION wccl\n";
	std::cout << "\n";
}

void FeatureRunner::print_header_body(const std::string& attribute_prefix,
	bool nos /*=false*/)
{
	if (!nos) {
		BOOST_FOREACH (const str_ops_map_t::value_type v, str_ops_) {
			std::cout << "@ATTRIBUTE "
				<< attribute_prefix << v.first << " string\n";
		}
	}
	BOOST_FOREACH (const bool_ops_map_t::value_type v, bool_ops_) {
		std::cout << "@ATTRIBUTE "
			<< attribute_prefix << v.first << " {0,1}\n";
	}
	BOOST_FOREACH (const tset_ops_map_t::value_type v, tset_ops_) {
		BOOST_FOREACH (const Corpus2::Tag& tag, v.second.first) {
			std::cout << "@ATTRIBUTE "
				<< attribute_prefix << v.first << "_"
				<< tagset_.tag_to_symbol_string(tag) << " {0,1}\n";
		}
	}
}

void FeatureRunner::print_header_foot()
{
	std::cout << "\n@DATA\n";
}

void FeatureRunner::print_data(const std::vector<std::vector<std::string> > &data)
{
	BOOST_FOREACH (const std::vector<std::string>& feats, data) {
		std::cout << boost::algorithm::join(feats, ",") << "\n";
	}
}

void FeatureRunner::print_data(
	const std::vector<std::vector<std::string> > &data,
	const std::vector<bool> rowmask)
{
	assert(data.size() == rowmask.size());
	for (size_t i = 0; i < data.size(); ++i) {
		if (rowmask[i]) {
			std::cout << boost::algorithm::join(data[i], ",") << "\n";
		}
	}
}

std::vector< std::vector<std::string> > FeatureRunner::do_sentence(
	const boost::shared_ptr<Corpus2::Sentence>& sentence)
{
	std::vector< std::vector<std::string> > sfeats;
	do_sentence(sentence, sfeats);
	return sfeats;
}


void FeatureRunner::do_sentence(
	const boost::shared_ptr<Corpus2::Sentence>& sentence,
	std::vector< std::vector<std::string> >& sfeats,
	bool nos /*=false*/)
{
	Wccl::SentenceContext sc(sentence);

	while (sc.is_current_inside()) {
		if (sfeats.size() < static_cast<size_t>(sc.get_position() + 1)) {
			sfeats.resize(sc.get_position() + 1);
		}
		assert(!sfeats.empty());
		std::vector<std::string>& feats = sfeats[sc.get_position()];
		if (!nos) {
			BOOST_FOREACH (const str_ops_map_t::value_type v, str_ops_) {
				boost::shared_ptr<const Wccl::StrSet> s = v.second->apply(sc);
				assert(s);
				if (s->contents().empty()) {
					feats.push_back("\"\"");
				} else {
					feats.push_back("\"" +
						boost::algorithm::replace_all_copy(
							PwrNlp::to_utf8(*s->contents().begin()),
							"\"", "\\\"") + "\"");
				}
			}
		}
		BOOST_FOREACH (const bool_ops_map_t::value_type v, bool_ops_) {
			boost::shared_ptr<const Wccl::Bool> b = v.second->apply(sc);
			assert(b);
			if (*b) {
				feats.push_back("1");
			} else {
				feats.push_back("0");
			}
		}
		BOOST_FOREACH (const tset_ops_map_t::value_type v, tset_ops_) {
			boost::shared_ptr<const Wccl::TSet> t = v.second.second->apply(sc);
			assert(t);
			BOOST_FOREACH (const Corpus2::Tag& tag, v.second.first) {
				if (!tag.get_masked(t->contents()).is_null()) {
					feats.push_back("1");
				} else {
					feats.push_back("0");
				}
			}
		}
		sc.advance();
	}
}


void FeatureRunner::do_stream(std::istream& is, bool first)
{
	Corpus2::XcesReader xr(tagset_, is);
	Corpus2::Sentence::Ptr s;
	print_header_head();
	print_header_body("");
	print_header_foot();
	while ((s = xr.get_next_sentence())) {
		print_data(do_sentence(s));
		std::cout << "\n";
		if (first) break;
	}
}

void FeatureRunner::do_files(std::vector<std::string>& files, bool first)
{
	std::vector<boost::shared_ptr<Corpus2::TokenReader> > readers;
	if (files.size() < 2) return;
	readers.push_back(Corpus2::TokenReader::create_path_reader(
		"xces", tagset_, files[0]));
	for (size_t i = 1; i < files.size(); ++i) {
		readers.push_back(Corpus2::TokenReader::create_path_reader(
			"xces,disamb_only", tagset_, files[i]));
	}
	print_header_head();
	for (size_t i = 0; i < files.size(); ++i) {
		print_header_body("T" + boost::lexical_cast<std::string>(i) + "_");
	}
	std::cout << "@ATTRIBUTE correct {0";
	for (size_t si = 1; si < files.size(); ++si) {
		//std::cout << "@ATTRIBUTE tag" << si << "ok " << "{0,1}" << "\n";
		std::cout << "," << si;
	}
	std::cout << "}\n";
	print_header_foot();
	bool more = !first;
	int processed = 0;
	do {
		std::vector<Corpus2::Sentence::Ptr> sentences;
		BOOST_FOREACH (const boost::shared_ptr<Corpus2::TokenReader>& r, readers) {
			Corpus2::Sentence::Ptr s = r->get_next_sentence();
			if (s) {
				sentences.push_back(s);
			}
		}
		if (sentences.size() == readers.size()) {
			std::vector< std::vector< std::string> > data;
			std::vector<bool> rowmask;
			size_t gold_size = sentences[0]->size();
			for (size_t si = 1; si < sentences.size(); ++si) {
				if (sentences[si]->size() != gold_size) {
					std::cerr << "Sentence size mismatch at " <<
						processed << " " << si << "\n";
					return;
				}
			}
			data.resize(gold_size);
			rowmask.resize(gold_size);
			do_sentence(sentences[0], data, false);
			for (size_t si = 1; si < sentences.size(); ++si) {
				do_sentence(sentences[si], data, false);
			}
			int maxv = 0;
			for (size_t i = 0; i < gold_size; ++i) {
				std::set<Corpus2::Tag> gold_tags;
				const Corpus2::Token& gold_token = *(*sentences[0])[i];
				BOOST_FOREACH (const Corpus2::Lexeme& gl, gold_token.disamb_lexemes()) {
					gold_tags.insert(gl.tag());
				}
				int wci = 0;
				std::map<Corpus2::Tag, int> v;
				for (size_t si = 1; si < sentences.size(); ++si) {
					const Corpus2::Token& token = *(*sentences[si])[i];
					bool wc = false;
					BOOST_FOREACH (const Corpus2::Lexeme& gl, token.lexemes()) {
						if (gold_tags.find(gl.tag()) != gold_tags.end()) {
							wc = true;
							wci = si;
						}
						v[gl.tag()]++;
						maxv = std::max(maxv, v[gl.tag()]);
					}
					//data[i].push_back(wc ? "1" : "0");
				}
				typedef std::pair<Corpus2::Tag, int> pp;
				int mv = 0;
				bool tie = false;
				BOOST_FOREACH (const pp& p, v) {
					if (p.second == mv) {
						tie = true;
					} else if (p.second > mv) {
						tie = false;
						mv = p.second;
					}
				}

				data[i].push_back(boost::lexical_cast<std::string>(wci));
				rowmask[i] = tie;
			}
			print_data(data, rowmask);
			++processed;
		} else {
			more = false;
		}
	} while (more);
}


int main(int argc, char** argv)
{
	std::string tagset_load = "kipi";
	bool first = false;
	std::vector<std::string> corpora_files, files, operator_strings;
	bool corpus_stdin = false;
	using boost::program_options::value;

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
			("corpus-from-stdin,I", value(&corpus_stdin)->zero_tokens(),
			 "Read corpus from stdin")
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress messages")
			("first-sentence-only,1", value(&first)->zero_tokens(),
			 "Only process first sentence")
			("tabs", value(&tabs)->zero_tokens(),
			 "Output a tab-separated file")
			("local-counts,l", value(&in_sentence_numbering),
			 "Output in-sentence token counts")
			("global-counts,g", value(&global_numbering),
			 "Output global counts")
			("output-orths,O", value(&output_orths),
			 "Output token orths")
			("output-variables,V", value(&output_variables),
			 "Output operator variables")
			("output-header,H", value(&output_header),
			 "Output table header")
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
		std::cout << "wccl-features (libwccl) " << LIBWCCL_VERSION << "\n";
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
	if ((corpora_files.empty() && !corpus_stdin) || (operator_strings.empty() && !output_orths)) {
		std::cerr << "Nothing to do, try " << argv[0] << " -h\n";
		return 2;
	}
	try {
		const Corpus2::Tagset& tagset = Corpus2::get_named_tagset(tagset_load);
		FeatureRunner runner(tagset);
		BOOST_FOREACH (const std::string& f, operator_strings) {
			if (boost::algorithm::ends_with(f, ".ccl")) {
				if (!runner.load_more_operators(f)) {
					std::cerr << "Warning: error while parsing " << f << "\n";
				}
			}
		}
		if (!runner.empty()) {
			if (corpora_files.size() == 1) {
				std::ifstream ifs(corpora_files[0].c_str());
				if (ifs.good()) {
					runner.do_stream(ifs, first);
				} else {
					std::cerr << "Error reading corpus from "
						<< corpora_files[0] << "\n";
				}
			} else {
				runner.do_files(corpora_files, first);
			}
		}
	} catch (PwrNlp::PwrNlpError& e) {
		std::cerr << e.info() << std::endl;
		return 2;
	}

	return 0;
}
