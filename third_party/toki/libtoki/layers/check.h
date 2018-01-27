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

#ifndef LIBTOKI_CHECKLAYER_H
#define LIBTOKI_CHECKLAYER_H

#include <libtoki/layers/tokenlayer.h>

namespace Toki {

/**
 * A sanity-check layer that can be configured to output warning messages
 * when very long tokens or very long sentences are noticed.
 */
class CheckLayer : public TokenLayer
{
public:
	/**
	 * Constructor.
	 *
	 * Keys recognized in the configuration:
	 * - max_token_size      - token size to consider overly huge, defaults
	 *                         to 300 characters
	 * - max_sentence_size   - sentence size to consider overly huge,
	 *                         defaults to 1000 tokens
	 * - huge_token_warn     - set to 0 to disable huge token warnings
	 * - huge_sentence_warn  - set to 0 to disable huge sentence warnings
	 * - huge_sentence_split - set to 1 to force-split huge sentences
	 * - check_spaces        - set to true to check for stray whitespace in
	 *                         the tokens' orths
	 * - warn_format         - either 1 for a '1' char for every warning,
	 *                       - 01 to also emit '0' for tokens with no
	 *                         warnings, and text for a text message.
	 */
	CheckLayer(TokenSource* input, const Config::Node& props);

	/// Destrctor
	~CheckLayer();

	/// TokenLayer override
	Token* process_token(Token *t);

	/// TokenLayer override
	virtual std::string info() const;

	/// TokenLayer override
	std::string long_info() const;

	/// TokenLayer override
	void restart();

private:
	/// helper function for actual warning output
	bool warn(const std::string& msg);

	/// behavior flags
	int max_token_size_;
	int max_sentence_size_;
	bool huge_token_warn_;
	bool huge_sentence_warn_;
	bool huge_sentence_split_;
	bool check_spaces_;

	bool emit_0_on_no_warnings_;
	bool emit_1_on_warnings_;

	/// Token counter to keep track of which tokens trigger warnings
	int token_counter_;
	/// Sentence size counter
	int sentence_size_counter_;
};

} /* end ns Toki */

#endif // LIBTOKI_CHECKLAYER_H
