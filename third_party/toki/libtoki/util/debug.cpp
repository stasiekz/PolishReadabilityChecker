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

#include <libtoki/util/debug.h>
#include <libtoki/token.h>
#include <libtoki/tokensource.h>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <functional>
#include <ostream>
#include <sstream>

namespace Toki { namespace Debug {

void sink_to_stream(boost::function<std::string (const Token&)> func,
		std::ostream& os, Token* t)
{
	os << func(*t);
	delete t;
}

void sink_to_stream(boost::function<std::string (const Token&)> func,
	std::ostream& os, Token* t, int & count)
{
	os << func(*t);
	++count;
	delete t;
}

std::string tokenize(TokenSource &tok,
		boost::function<std::string(const Token &)> func, int* count)
{
	std::stringstream ss;
	if (count) {
		tok.tokenize(boost::bind(&sink_to_stream,
			boost::ref(func), boost::ref(ss), _1, boost::ref(*count)));
	} else {
		tok.tokenize(boost::bind(&sink_to_stream,
			boost::ref(func), boost::ref(ss), _1));
	}
	return ss.str();
}

std::string token_debug(const Token& t)
{
	std::stringstream ss;
	ss << "[" << t.orth_utf8() << "]-" << t.type() << "-"
			<< t.preceeding_whitespace() << "-";
	return ss.str();
}

std::string token_format(const std::string &format, const Token &t)
{
	std::string out = format;
	boost::replace_first(out, "$orth", t.orth_utf8());
	boost::replace_first(out, "$type", t.type());
	boost::replace_first(out, "$bs_01", t.begins_sentence() ? "1" : "0");
	boost::replace_first(out, "$bs|", t.begins_sentence() ? "|" : "");
	boost::replace_first(out, "$bs", t.begins_sentence() ? "bs" : "");
	boost::replace_first(out, "$ws_id",
		boost::lexical_cast<std::string>(t.preceeding_whitespace()));
	boost::replace_first(out, "$ws_any",
		t.preceeding_whitespace() == PwrNlp::Whitespace::None ? "0" : "1");
	boost::replace_first(out, "$ws_ws",
		PwrNlp::Whitespace::to_whitespace(t.preceeding_whitespace()));
	boost::replace_first(out, "$ws",
		PwrNlp::Whitespace::to_string(t.preceeding_whitespace()));
	return out;
}

void tokenize_orths_newline(TokenSource& ts, std::ostream& os, int* count)
{
	boost::function<std::string (const Token&)> ff;
	ff = boost::bind(std::plus<std::string>(),
				boost::bind(&Token::orth_utf8, _1), "\n");
	if (count) {
		ts.tokenize(boost::bind(&sink_to_stream,
			boost::ref(ff), boost::ref(os), _1, boost::ref(*count)));
	} else {
		ts.tokenize(boost::bind(&sink_to_stream,
			boost::ref(ff), boost::ref(os), _1));
	}
}

std::string tokenize_orths_newline(TokenSource& ts)
{
	return tokenize(ts,
		boost::bind(std::plus<std::string>(),
			boost::bind(&Token::orth_utf8, _1),
		"\n"));
}

std::string tokenize_formatted(TokenSource &tok, const std::string &format,
	int *count)
{
	std::stringstream ss;
	tokenize_formatted(tok, format, ss, count);
	return ss.str();
}

void tokenize_formatted(TokenSource &tok, const std::string &format,
	std::ostream& os, int *count)
{
	boost::function<std::string (const Token&)> ff;
	ff = boost::bind(&token_format, boost::ref(format), _1);
	if (count) {
		tok.tokenize(boost::bind(&sink_to_stream,
			boost::ref(ff), boost::ref(os), _1, boost::ref(*count)));
	} else {
		tok.tokenize(boost::bind(&sink_to_stream,
			boost::ref(ff), boost::ref(os), _1));
	}
}

void tokenize_progress(TokenSource &tok, std::ostream &os, int step,
		int *count)
{
	int i = step;
	while(Token* t = tok.get_next_token()) {
		--i;
		if (i == 0) {
			os << "*";
			i = step;
		}
		if (count) {
			(*count)++;
		}
		delete t;
	}
}

} /* end ns Debug */ } /* end namespace Toki */
