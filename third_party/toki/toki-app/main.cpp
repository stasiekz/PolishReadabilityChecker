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
#include <libtoki/layers/tokenlayer.h>
#include <libtoki/token.h>
#include <libtoki/unicode/istreamwrapper.h>
#include <libtoki/unicode/icustringwrapper.h>
#include <libpwrutils/util.h>
#include <libtoki/util/debug.h>
#include <libtoki/exception.h>
#include <libtoki/srx/srx.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <unicode/ustream.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>


struct nullstream : std::ostream {
	struct nullbuf : std::streambuf {
		int overflow(int c) { return traits_type::not_eof(c); }
	} m_sbuf;
	nullstream(): std::ios(&m_sbuf), std::ostream(&m_sbuf) {}
};

int main(int argc, char** argv)
{
	/*
	TokenizerConfig::Cfg global = TokenizerConfig::from_file("config.ini");
	TokenizerConfig::Cfg user = TokenizerConfig::from_file("user.ini");
	TokenizerConfig::merge(global, user);
	TokenizerConfig::write(global, "combined");
	*/

	std::string input_enc, format;
	std::string config_file;
	std::string config_path;
	std::string srx, srx_lang, srx_mode, srx_mark_start, srx_mark_end;
	std::string initial_wa_override;
	bool end_newline;
	int bufsize;
	bool orths;
	bool verbose;
	bool quiet;
	bool stats;
	bool no_output;
	int progress;
	using boost::program_options::value;
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("input-encoding,e", value(&input_enc)->default_value("UTF8"),
			 "Input encoding (ICU string identifier), for example UTF8, cp1250")
			("config-file,c", value(&config_file)->default_value(""),
			 "Config file to use, defaults to library-default config")
			("config-path,C", value(&config_path)->default_value(""),
			 "Override config search path")
			("buffer-size,b", value(&bufsize)->default_value(1),
			 "Stream buffer size, set to 0 to convert the entire input "
			 "in-memory before processing and disregard the encoding, assuming UTF-8.")
			("format,f", value(&format),
			 "Output format override")
			("orths,o", value(&orths)->default_value(false)->zero_tokens(),
			 "Only output orths, not entire token descriptions "
			 "(ignore debug.format in config file)")
			("initial-wa-override,i", value(&initial_wa_override),
			 "Initial whitespace (overrides config file)")
			("newline,n", value(&end_newline)->default_value(false)->zero_tokens(),
			 "Ensure tokenization output ends with a newline")
			("verbose,v", value(&verbose)->default_value(false)->zero_tokens(),
			 "Verbose init info")
			("quiet,q", value(&quiet)->default_value(false)->zero_tokens(),
			 "Suppress info output")
			("srx,S", value(&srx)->default_value(""),
			 "Load SRX from file and only do SRX processing")
			("lang,l", value(&srx_lang)->default_value("pl"),
			 "SRX language selection")
			("srx-mode", value(&srx_mode)->default_value("icu"),
			 "SRX mode selection")
			("stats,s", value(&stats)->default_value(false)->zero_tokens(),
			 "Display tokenization stats (token count) at end")
			("no-output", value(&no_output)->default_value(false)->zero_tokens(),
			 "Disable tokenization output")
			("srx-begin-marker", value(&srx_mark_start)->default_value(""),
			 "SRX segment begin marker")
			("srx-end-marker", value(&srx_mark_end)->default_value("\n"),
			 "SRX segment end marker")
			("progress-only", value(&progress)->default_value(0),
			 "No output, progress display every N tokens")
			("help,h", "Show help")
			;
	boost::program_options::variables_map vm;

	try {
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).run(), vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << "\n";
		return 2;
	}
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cerr << desc << "\n";
		return 1;
	}

	nullstream nullstr;
	std::ostream& out = no_output ? nullstr : std::cout;

	if (!srx.empty()) {
		Toki::Srx::Document doc;
		std::ifstream srx_ifs(srx.c_str());
		doc.load(srx_ifs);
		if (verbose) {
			std::cerr << doc.info() << "\n";
		}
		Toki::Srx::Segmenter* segm;
		segm = Toki::Srx::Segmenter::get_segmenter_by_name(srx_mode);
		if (!segm) return 5;
		segm->load_rules(doc.get_rules_for_lang(srx_lang));
		Toki::Srx::SourceWrapper srx(new Toki::UnicodeIstreamWrapper(std::cin), segm, 65536, 256);
		int segments = 0;
		out << srx_mark_start;
		while (srx.has_more_chars()) {
			if (srx.peek_begins_sentence()) {
				++segments;
				out << srx_mark_end;
				out << srx_mark_start;
			}
			out << PwrNlp::to_utf8(srx.get_next_char());
		}
		out << srx_mark_end;
		std::cerr << segments << "\n";
		return 0;
	}

	if (bufsize < 0) return 3;

	if (!config_path.empty()) {
		Toki::Path::Instance().set_search_path(config_path);
	}
	if (verbose) {
		std::cerr << "Config search path: " << Toki::Path::Instance().get_search_path_string()
			<< "\n";
	}
	int count = 0;
	int* cptr = 0;
	if (stats) {
		cptr = &count;
	}
	Toki::Path::Instance().set_verbose(!quiet);

	try {
		const Toki::Config::Node& conf = config_file.empty() ?
			Toki::default_config() :
			Toki::get_named_config(config_file);
		Toki::LayerTokenizer tok(conf);
		if (!quiet) {
			std::cerr << "Available layer types: "
				<< boost::algorithm::join(Toki::TokenLayer::available_layer_types(), " ")
				<< "\n";
			if (verbose) {
				std::cerr << "Tokenizer layers:\n";
				std::cerr << tok.layers_long_info("\n");
			} else {
				std::cerr << "Tokenizer: " << tok.layers_info() << "\n";
			}
			std::cerr << "Tokenizer started. C-d or C-c to exit.\n";
		}
		tok.set_input_source(std::cin, bufsize);
		if (!initial_wa_override.empty()) {
			PwrNlp::Whitespace::Enum wa = PwrNlp::Whitespace::from_string(initial_wa_override);
			if (PwrNlp::Whitespace::is_valid(wa)) {
				tok.input_tokenizer().set_initial_whitespace(wa);
			} else {
				std::cerr << "Invalid initial whitespace: " << initial_wa_override << "\n";
			}
		}
		if (progress) {
			Toki::Debug::tokenize_progress(tok, std::cerr, progress, cptr);
		} else if (orths) {
			Toki::Debug::tokenize_orths_newline(tok, out, cptr);
		} else {
			if (format.empty()) {
				format = conf.get("debug.format", "[$orth]-$type-$ws-\n");
			}
			format = PwrNlp::unescape_utf8(format);
			Toki::Debug::tokenize_formatted(tok, format, out, cptr);
			if (end_newline) {
				std::cout << std::endl;
			}
		}
		if (stats) {
			std::cerr << count << "\n";
		}
	} catch (Toki::TokiError& e) {
		std::cerr << "Error: " << e.info() << "\n";
	}
}
