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

// not checking for HAVE_VERSION..., no reason for its absence
#include <libwccl/version.h>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <libcorpus2/io/xcesreader.h>

#include <antlr/NoViableAltException.hpp>
#include <antlr/MismatchedTokenException.hpp>
// ----------------------------------------------------------------------------

#ifdef HAVE_LIBEDIT
#include <histedit.h>
#endif

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/**
 * @desc A simple command line tester for testing operators
 */

namespace {
	const char* _prompt = "Enter any operator expression: ";
}

void std_read_loop(boost::function<bool (const std::string&)>& line_cb)
{
	while (std::cin.good()) {
		std::cout << _prompt << std::flush;
		std::string s;
		getline(std::cin, s);
		if (line_cb(s)) {
			return;
		}
	}
}

#if defined(HAVE_LIBEDIT) && !defined(_WINDOWS) //non-Windows libedit read loop

const char* query_prompt(EditLine*) {
	return _prompt;
}

void read_loop(boost::function<bool (const std::string&)>& line_cb)
{
	EditLine *el = el_init("wccl-parser", stdin, stdout, stderr);
	el_set(el, EL_PROMPT, &query_prompt);
	el_set(el, EL_EDITOR, "emacs");
	History* myhistory = history_init();
	if (myhistory == NULL) {
		std::cerr << "EditLine history init error\n";
		el_end(el);
		std_read_loop(line_cb);
		return;
	}
	HistEvent ev;
	history(myhistory, &ev, H_SETSIZE, 1024);
	el_set(el, EL_HIST, history, myhistory);

	bool more = true;
	while (more) {
		int count;
		const char *line = el_gets(el, &count); // line gets a trailing \n
		if (line == NULL || line[0] == 0) {
			more = false;
		} else {
			std::string s(line, strlen(line) - 1);
			if (line_cb(s)) {
				more = false;
			} else {
				history(myhistory, &ev, H_ENTER, line);
			}
		}
	}

	history_end(myhistory);
	el_end(el);
}

#elif defined(_WINDOWS) // windows console read loop, doing UNICODE

struct ConsoleCPSetter
{
	UINT original_CP;
	ConsoleCPSetter(UINT cp) {
		original_CP = GetConsoleCP();
		SetConsoleCP(cp);
	}
	~ConsoleCPSetter() {
		SetConsoleCP(original_CP);
	}
};

void read_loop(boost::function<bool (const std::string&)>& line_cb)
{
	HANDLE stdinhandle = GetStdHandle(STD_INPUT_HANDLE);
	WCHAR buffer[1024];
	const int max = sizeof(buffer)/sizeof(WCHAR) - 1;
	ConsoleCPSetter cp(CP_UTF8);
	bool more = true;
	while(more) {
		std::cout << _prompt << std::flush;
		std::wstring s;
		while(more) {
			DWORD read;
			if (ReadConsoleW(stdinhandle, buffer, max, &read, NULL)) { 
				buffer[read] = NULL;
				while(buffer[read - 1] == L'\n' || buffer[read - 1] == L'\r') {
					buffer[--read] = 0;
				}
				more = (read == max);
				s += buffer;
			} else {
				if (GetLastError() == ERROR_INVALID_HANDLE) {
					// assume stdin redirect, fallback to standard getline
					getline(std::wcin, s);
				}
				more = false;
			}
		}
		UnicodeString us(s.c_str());
		more = !line_cb(PwrNlp::to_utf8(us));
	}
}

#else // standard io read loop

inline
void read_loop(boost::function<bool (const std::string&)>& line_cb)
{
	std_read_loop(line_cb);
}

#endif

int clear_screen()
{
#ifdef _WINDOWS
	return std::system("cls");
#else
	return std::system("clear");
#endif
}

bool process_line(const std::string& line, Wccl::Parser& parser,
	Wccl::SentenceContext& sc, bool all_positions, bool dump_variables)
{
	if (line.empty() || line == "exit" || line == "quit") {
		return true;
	} else if (line == "clear" || line == "cls") {
		if (clear_screen()) {}
		return false;
	}

	boost::shared_ptr<const Wccl::Value> retVal;
	boost::shared_ptr<Wccl::FunctionalOperator> retOp;
	try {
		retOp = parser.parseAnyOperator(line);
		if (retOp) {
			int pb, pe;

			if (all_positions) {
				pb = 0;
				pe = sc.get_sentence().empty() ? 1 : sc.size();
			} else {
				pb = sc.get_position();
				pe = sc.get_position() + 1;
			}

			for (int i = pb; i < pe; ++i) {
				sc.set_position(i);
				retVal = retOp->base_apply(sc);

				if (retVal) {
					std::cerr << "[" << std::setw(2) << sc.get_position() << "] "
						<< "Parsed expression: "
						<< retVal->to_string(*parser.tagset())
						<< std::endl;
					if (dump_variables) {
						retOp->dump_variables(std::cerr, *parser.tagset());
					}
				} else {
					std::cerr << "Problem while parsing -- "
						<< "retVal is NULL!" << std::endl;
				}
			}
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

int main(int argc, char** argv)
{
	std::string tagset_load = "kipi";
	std::string query = "";
	std::string sentence_load = "";
	std::string query_load = "";
	std::string position = "0";
	bool quiet = false;
	bool dump_variables = false;
	using boost::program_options::value;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("tagset,t", value(&tagset_load),
			 "Tagset to use\n")
			("sentence,s", value(&sentence_load),
			 "Sentence to load (XCES)\n")
			("position,p", value(&position),
			 "Position in the sentence to use, 'all' iterates through the sentence\n")
			("query-file,f", value(&query_load),
			 "Run query from file (disables interohaiactive mode) (- is stdin)\n")
			("query,Q", value(&query),
			 "Query to run (disables interactive mode)\n")
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress messages\n")
			("variables,V", value(&dump_variables)->zero_tokens(),
			 "Dump variables after running each query\n")
			("help,h", "Show help")
			("version", "print version string")
			;
	boost::program_options::variables_map vm;
	boost::program_options::positional_options_description p;
	p.add("query", -1);

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
		std::cout << desc << "\n";
		return 1;
	}

	if (vm.count("version")) {
		std::cout << "wccl-parser (libwccl) " << LIBWCCL_VERSION << "\n";
		return 0;
	}

	try {
		const Corpus2::Tagset& tagset = Corpus2::get_named_tagset(tagset_load);
		boost::shared_ptr<Corpus2::Sentence> sentence;
		if (sentence_load.empty()) {
			sentence = boost::make_shared<Corpus2::Sentence>();
			sentence->append(new Corpus2::Token("", PwrNlp::Whitespace::ManySpaces));
		} else {
			std::ifstream ifs(sentence_load.c_str());
			if (ifs.good()) {
				Corpus2::XcesReader reader(tagset, ifs, false);
				sentence = reader.get_next_sentence();
				std::cerr << "Sentence loaded, " << sentence->size()
					<< " tokens.\n";
			} else {
				throw Wccl::FileNotFound(sentence_load, "", "Sentence");
			}
		}
		Wccl::SentenceContext sc(sentence);
		int pos = atoi(position.c_str());
		sc.set_position(pos);
		Wccl::Parser parser(tagset);
		if (!query.empty()) {
			process_line(query, parser, sc, position == "all", dump_variables);
			return 0;
		} else if (!query_load.empty()) {
			std::stringstream ss;
			if (query_load == "-") {
				ss << std::cin.rdbuf();
			} else {
				std::ifstream qf(query_load.c_str());
				if (qf.good()) {
					ss << qf.rdbuf();
				} else {
					throw Wccl::FileNotFound(sentence_load, "", "Query file");
				}
			}
			process_line(ss.str(), parser, sc, position == "all", dump_variables);
			return 0;
		}
		boost::function<bool (const std::string&)> f;
		f = boost::bind(&process_line, _1, boost::ref(parser), boost::ref(sc),
			position == "all", dump_variables);
		read_loop(f);
	} catch (PwrNlp::PwrNlpError& e) {
		std::cerr << e.info() << std::endl;
		return 2;
	}

	return 0;
}
