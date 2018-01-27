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

#ifndef LIBTOKI_DEBUG_H
#define LIBTOKI_DEBUG_H

#include <libtoki/tokensource.h>

#include <iosfwd>
#include <string>

namespace Toki {
namespace Debug {

/**
 * Helper function to pass a token through a given function and feed the
 * resulting std::strings into an ostream
 */
void sink_to_stream(boost::function<std::string (const Token&)> func,
		std::ostream& os, Token* t);

/**
 * Tokenize the tokens from the TokenSource by passing all tokens through
 * the given functon and concatenating the output std::strings.
 */
std::string tokenize(TokenSource& tok,
	boost::function<std::string (const Token&)> func, int *count = NULL);

/**
 * Formatted token debug output. The following replacements will be done:
 * - $orth   -> the token's orth
 * - $type   -> the token's type
 * - $bs_01  -> 1 if the token has the begins_senctence flag set, else 0
 * - $bs|    -> "|" if token begins sentence, empty string otherwise
 * - $bs     -> "bs" if token begins sentence, empty string otherwise
 * - $ws     -> the preceeding whitespace amount string name, e.g. "none"
 *              "space" or "newline". @see Token::WhitespaceAmount
 * - $ws_id  -> the preceeding whitespace numeric code, 0 for no whitespace
 * - $ws_any -> 0 if there was any whitespace preceeding the token, else 1
 * - $ws_ws  -> the approximate whitespace characters that were preceeding
 *              the token (actual spaces / newlines)
 *
 * The format string will usually be unescaped before getting here, so
 * e.g. backslash-n is usually a valid way of putting a newline into the
 * debug output.
 */
std::string token_format(const std::string& format, const Token& t);

/**
 * "Standard" token debug output
 */
std::string token_debug(const Token& t);

/**
 * "Standard" debug tokenization -- orths with newline, equivalent to a
 * $orth\n format. If count is not null it *count will be set to the number
 * of tokens read.
 */
std::string tokenize_orths_newline(TokenSource& ts);

/**
 * "Standard" debug tokenization -- orths with newline, equivalent to a
 * $orth\n format. If count is not null it *count will be set to the number
 * of tokens read.
 */
void tokenize_orths_newline(TokenSource& ts, std::ostream& os,
		int* count = NULL);

/**
 * Return a string with all tokens processed and printed according to the
 * given format string, @see token_format for format options.
 */
std::string tokenize_formatted(TokenSource& tok, const std::string& format,
	int *count = NULL);

/**
 * Output into the stream all tokens processed and printed according to the
 * given format string, @see token_format for format options.
 */
void tokenize_formatted(TokenSource& tok, const std::string& format,
		std::ostream& os, int *count = NULL);

/**
 * Ignore the tokens but keep track of how many there are and show a
 * progress indicator
 */
void tokenize_progress(TokenSource& tok, std::ostream& os, int step,
		int *count = NULL);

} /*end ns Debug */
} /* end ns Toki */

#endif // LIBTOKI_DEBUG_H
