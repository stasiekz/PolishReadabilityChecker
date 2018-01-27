/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE and COPYING files for more details.
*/

#include <libcorpus2/util/settings.h>
#include <libcorpus2/tagsetparser.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/token.h>

#include <boost/foreach.hpp>

#include <boost/program_options.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <fstream>
#include <iterator>

#ifdef HAVE_LIBEDIT
#include <histedit.h>
#endif

#ifdef HAVE_CONFIG_D_H
#include <libcorpus2/config_d.h>
#endif

// not checking for HAVE_VERSION, there is no reason it shouldn't be there
#include <libcorpus2/version.h>

namespace {
	const char* _prompt = "> ";
}

void std_read_loop(boost::function<void (const std::string&)>& line_cb)
{
	while (std::cin.good()) {
		std::string s;
		std::cin >> s;
		if (!s.empty()) {
			line_cb(s);
		}
	}
}

#ifdef HAVE_LIBEDIT
const char* query_prompt(EditLine*) {
	return _prompt;
}

void libedit_read_loop(boost::function<void (const std::string&)>& line_cb)
{
	EditLine *el = el_init("tagset-tool", stdin, stdout, stderr);
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
			line_cb(s);
			history(myhistory, &ev, H_ENTER, line);
		}
	}

	history_end(myhistory);
	el_end(el);
}
#endif


void tagset_info(const Corpus2::Tagset& tagset)
{
	std::cerr << "Corpus2::Tagset loaded: "
		<< tagset.pos_count() << " POSes, "
		<< tagset.attribute_count() << " attributes, "
		<< tagset.value_count() << " values [ ";
	for (Corpus2::idx_t a = 0; a < tagset.attribute_count(); ++a) {
		std::cerr << tagset.get_attribute_values(a).size() << " ";
	}
	std::cerr << "]\n";
	std::cerr << "Size is " << tagset.size()
		<< " (extra size is " << tagset.size_extra() << ")\n";
	std::cerr << "POSes: ";
	BOOST_FOREACH(const std::string& s, tagset.pos_dictionary()) {
		std::cerr << s << " ";
	}
	std::cerr << "\n";
}

void tagset_query_cb(const Corpus2::Tagset& tagset, const std::string& s,
		bool internals)
{
	Corpus2::idx_t pos = tagset.pos_dictionary().get_id(s);
	Corpus2::idx_t atr = tagset.attribute_dictionary().get_id(s);
	Corpus2::mask_t val = tagset.get_value_mask(s);
	if (pos >= 0) {
		if (internals) {
			std::cout << tagset.get_pos_mask(pos) << " (" << (int)pos << ")\n";
		}
		std::cout << s << " -> POS ->" ;
		BOOST_FOREACH(Corpus2::idx_t a, tagset.get_pos_attributes(pos)) {
			std::string astr = tagset.attribute_dictionary().get_string(a);
			if (tagset.pos_requires_attribute(pos, a)) {
				std::cout << " " << astr;
			} else {
				std::cout << " [" << astr << "]";
			}
		}
		std::cout << "\n";
	} else if (atr >= 0) {
		if (internals) {
			std::cout << tagset.get_attribute_mask(atr) << " (" << (int)atr << ")\n";
		}
		std::cout << s << " -> attribute ->";
		BOOST_FOREACH(Corpus2::mask_t v, tagset.get_attribute_values(atr)) {
			std::cout << " " << tagset.get_value_name(v);
		}
		std::cout << "\nIn POSes:";
		for (Corpus2::idx_t p = 0; p < tagset.pos_count(); ++p) {
			if (tagset.pos_has_attribute(p,atr)) {
				std::cout << " " << tagset.pos_dictionary().get_string(p);
				if (!tagset.pos_requires_attribute(p, atr)) {
					std::cout << "?";
				}
			}
		}
		std::cout << "\n";
	} else if (val.any()) {
		Corpus2::idx_t a = tagset.get_value_attribute(val);
		if (internals) {
			std::cout << val << " (" << PwrNlp::lowest_bit(val) << ")\n";
			std::cout << tagset.get_attribute_mask(a) << " (" << (int)a << ")\n";
		}
		std::cout << s << " -> value -> attribute ";
		std::cout << tagset.attribute_dictionary().get_string(a);
		std::cout << " .";
		BOOST_FOREACH(Corpus2::mask_t v, tagset.get_attribute_values(a)) {
			std::cout << " " << tagset.get_value_name(v);
		}
		std::cout << "\nIn POSes:";
		for (Corpus2::idx_t p = 0; p < tagset.pos_count(); ++p) {
			if (tagset.pos_has_attribute(p, a)) {
				std::cout << " " << tagset.pos_dictionary().get_string(p);
				if (!tagset.pos_requires_attribute(p, a)) {
					std::cout << "?";
				}
			}
		}
		std::cout << "\n";
	} else {
		std::cout << "Not recognized: " << s << "\n";
	}
}

void tag_parse_cb(const Corpus2::Tagset& tagset, bool validate, bool sort,
		const std::string& s, bool internals)
{
	try {
		Corpus2::Token t;
		tagset.lexemes_into_token(t, UnicodeString(), s);
		std::vector<std::string> out;
		BOOST_FOREACH(const Corpus2::Lexeme& lex, t.lexemes()) {
			std::stringstream ss;
			ss << tagset.tag_to_string(lex.tag());
			if (validate) {
				tagset.validate_tag(lex.tag(), Corpus2::Tagset::ParseStrict, &ss);
			}
			if (internals) {
				ss << "\n" << lex.tag().raw_dump() << "";
			}
			out.push_back(ss.str());
		}
		if (sort) {
			std::sort(out.begin(), out.end());
		}
		std::copy(out.begin(), out.end(),
				std::ostream_iterator<std::string>(std::cout, "\n"));
	} catch (Corpus2::TagParseError& e) {
		std::cerr << "TagParseError: " << e.info() << "\n";
	}
}

int main(int argc, char** argv)
{
	std::string tagset_load, tagset_save;
	bool quiet = false, internals = false;
	bool parse = false, validate = false, sort = false;
	using boost::program_options::value;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("tagset,t", value(&tagset_load),
			 "Path to tagset ini file to load")
			("save-tagset,S", value(&tagset_save),
			 "Path to tagset ini file to save")
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress startup info")
			("parse,p", value(&parse)->zero_tokens(),
			 "Parse complex tag strings mode")
			("internals,i", value(&internals)->zero_tokens(),
			 "Output internal representations")
			("validate,v", value(&validate)->zero_tokens(),
			 "Validate parsed tags")
			("sort,s", value(&sort)->zero_tokens(),
			 "Sort parsed tags")
			("help,h", "Show help")
			("version", "print version string")
			;
	boost::program_options::variables_map vm;
	boost::program_options::positional_options_description p;
	p.add("tagset", -1);

	try {
		boost::program_options::store(
			boost::program_options::command_line_parser(argc, argv)
			.options(desc).positional(p).run(), vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << "\n";
		return 2;
	}
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		std::cout << "Available tagsets: ";
		std::cout << Corpus2::available_tagsets() << "\n";
		return 1;
	}
	if (vm.count("version")) {
		std::cout << "tagset-tool (libcorpus2) " << LIBCORPUS2_VERSION << "\n";
		return 0;
	}

	Corpus2::Path::Instance().set_verbose(!quiet);

	if (!tagset_load.empty()) {
		try {
			const Corpus2::Tagset& tagset = Corpus2::get_named_tagset(tagset_load);

			if (!quiet) {
				tagset_info(tagset);
			}

			if (!tagset_save.empty()) {
				std::ofstream ofs(tagset_save.c_str());
				if (!quiet) {
					std::cerr << "Saving tagset to " << tagset_save << " ... ";
				}
				Corpus2::TagsetParser::save_ini(tagset, ofs);
				if (!quiet) {
					std::cerr << "ok\n";
				}
			}
			boost::function<void (const std::string&)> f;
			if (parse) {
				if (!quiet) {
					std::cerr << "(Tag parse mode)\n";
				}
				_prompt = "tag-parse> ";
				f = boost::bind(&tag_parse_cb, boost::ref(tagset), validate, sort, _1, internals);
			} else {
				if (!quiet) {
					std::cerr << "(Tagset query mode)\n";
				}
				_prompt = "tagset-query> ";
				f = boost::bind(&tagset_query_cb, boost::ref(tagset), _1, internals);
			}
#ifdef HAVE_LIBEDIT
			libedit_read_loop(f);
#else
			std_read_loop(f);
#endif
		} catch (Corpus2::Corpus2Error& e) {
			std::cerr << "Error: " << e.info() << "\n";
			return 4;
		}
	} else {
		std::cerr << "Usage: tagset-tool [OPTIONS] <tagset-file>\n";
		std::cerr << "See tagset-tool --help\n";
		return 1;
	}
	return 0;
}
