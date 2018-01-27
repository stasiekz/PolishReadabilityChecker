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

#include <libtoki/srx/srx.h>
#include <libtoki/tokenizer/tokenizer.h>
#include <libtoki/tokenizer/whitespacetokenizer.h>
#include <libpwrutils/util.h>

#include <unicode/uchar.h>
#include <unicode/ucnv.h>

#include <fstream>
#include <iostream>

#include <boost/make_shared.hpp>

namespace Toki {

	WhitespaceTokenizer::WhitespaceTokenizer(const Config::Node &cfg)
		: Tokenizer(cfg), wa_(PwrNlp::Whitespace::None), token_type_(),
		initial_wa_(PwrNlp::Whitespace::None), first_(true), begins_sentence_(true)
		, srx_()
	{
		process_config(cfg);
	}

	WhitespaceTokenizer::WhitespaceTokenizer(UnicodeSource* us,
			const Config::Node& cfg)
		: Tokenizer(us, cfg), wa_(PwrNlp::Whitespace::None), token_type_(),
		initial_wa_(PwrNlp::Whitespace::None), first_(true), begins_sentence_(true)
		, srx_()
	{
		process_config(cfg);
	}

	void WhitespaceTokenizer::process_config(const Config::Node &cfg)
	{
		token_type_ = cfg.get("token_type", "t");
		std::string init_wa = cfg.get("initial_whitespace", "");
		if (init_wa.empty()) {
			initial_wa_ = PwrNlp::Whitespace::Newline;
		} else {
			initial_wa_ = PwrNlp::Whitespace::from_string(init_wa);
			if (initial_wa_ == PwrNlp::Whitespace::PostLast) {
				std::cerr << "Bad initial whitespace value:" << init_wa
						<< "\n";
				initial_wa_ = PwrNlp::Whitespace::None;
			}
		}
		std::string srx = cfg.get("srx", "");
		if (!srx.empty()) {
			std::ifstream ifs;
			Path::Instance().open_stream_or_throw(srx, ifs, "SRX");
			Srx::Document d;
			d.load(ifs);
			std::string srx_lang = cfg.get("srx_language", "");
			std::vector<Srx::Rule> rules;
			if (srx_lang.empty()) {
				rules = d.get_all_rules();
			} else {
				rules = d.get_rules_for_lang(srx_lang);
			}
			//std::cerr << "SRX: " << rules.size() << " rules active\n";
			boost::shared_ptr<Srx::Segmenter> segm =
					boost::make_shared<Srx::NaiveIcuSegmenter>();
			segm->load_rules(rules);
			int window = cfg.get("srx_window", 10000);
			int margin = cfg.get("srx_margin", 100);
			boost::shared_ptr<Srx::SourceWrapper> u;
			u = boost::make_shared<Srx::SourceWrapper>(get_input_source(),
					segm, window, margin);
			set_input_source(u);
			srx_ = u;
		}
		wa_ = initial_wa_;
	}

	void WhitespaceTokenizer::set_initial_whitespace(PwrNlp::Whitespace::Enum wa)
	{
		initial_wa_ = wa_ = wa;
	}

	void WhitespaceTokenizer::restart()
	{
		wa_ = initial_wa_;
		begins_sentence_ = true;
		first_ = true;
	}

	bool WhitespaceTokenizer::has_srx() const
	{
		return !!srx_;
	}

	void WhitespaceTokenizer::new_input_source()
	{
		if (srx_) {
			srx_->set_source(get_input_source());
			input_ = srx_;
		}
	}


	void WhitespaceTokenizer::eat_whitespace()
	{
		int ws = 0; int nl = 0;
		while (input().has_more_chars()) {
			UChar u = input().peek_next_char();
			if (input().peek_begins_sentence()) {
				begins_sentence_ = true;
			}
			if (u == 0xfeff || u == 0x200b) { 
				//U+FEFF BOM mark (aka ZERO WIDTH NO-BREAK SPACE)
				//U+200B ZERO WIDTH SPACE
				//do not increment ws. BOM's are skipped entirely
				input().get_next_char();
			} else if (!u_isUWhiteSpace(u)) {
				break;
			} else {
				ws++;
				// Newlines recognized:
				// U+A <control> (newline)
				// U+2028 LINE SEPARATOR
				// U+2029 PARAGRAPH SEPARATOR
				if (u == 0xA || u == 0x2029 || u == 0x2028) {
					nl++;
				}
				input().get_next_char();
			}
		}
		if (nl > 1) {
			wa_ = PwrNlp::Whitespace::ManyNewlines;
		} else if (nl == 1) {
			wa_ = PwrNlp::Whitespace::Newline;
		} else if (ws > 1) {
			wa_ = PwrNlp::Whitespace::ManySpaces;
		} else if (ws == 1) {
			wa_ = PwrNlp::Whitespace::Space;
		} else {
			wa_ = PwrNlp::Whitespace::None;
		}
		if (first_) {
			if (wa_ < initial_wa_) {
				wa_ = initial_wa_;
			}
			first_ = false;
		}
	}

	Token* WhitespaceTokenizer::get_next_token()
	{
		if (!input().has_more_chars()) {
			return NULL;
		}
		eat_whitespace();
		UnicodeString orth;
		if (input().has_more_chars()) {
			bool next_token_begins_sentence = false;
			UChar u = input().get_next_char();
			orth = u;
			while (input().has_more_chars()) {
				std::string ou = PwrNlp::to_utf8(orth);
				//std::cerr << ">> " << ou << " "
				//		<< "begs=" << begins_sentence_ << " "
				//		<< "inbegs=" << input().peek_begins_sentence() << " "
				//		<< "nextbegs=" << next_token_begins_sentence << "\n";
				u = input().peek_next_char();
				if (u_isUWhiteSpace(u) || u == 0xfeff || u == 0x200b) {
					if (input().peek_begins_sentence()) {
						next_token_begins_sentence = true;
					}
					break;
				} else if (input().peek_begins_sentence()) {
					next_token_begins_sentence = true;
					break;
				} else {
					orth += u;
					input().get_next_char();
				}
			}
			Token* t  = new Token(orth, token_type_, wa_);
			t->set_begins_sentence(begins_sentence_);
			begins_sentence_ = next_token_begins_sentence;
			return t;
		} else {
			return NULL;
		}
	}

} /* end namespace Toki */
