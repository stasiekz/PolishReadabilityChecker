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

#ifndef SENTENCESPLITTER_H
#define SENTENCESPLITTER_H

#include <libtoki/tokensource.h>
#include <libtoki/sentence.h>

#include <vector>

namespace Toki {

/**
 * Helper class to get sentences from the tokenizer as separate vectors
 * of Tokens.
 */
class SentenceSplitter
{
public:
	/// Create an empty splitter working with a source
	SentenceSplitter(TokenSource& ts);

	/// Destructor
	~SentenceSplitter();

	/// Source setter
	void set_source(TokenSource& ts);

	/// Source getter
	TokenSource& source() {
		return *ts_;
	}

	/// Source getter, const version
	const TokenSource& source() const {
		return *ts_;
	}

	/**
	 * Returns true if there will be more sentences, false if the source
	 * has been exhausted. May result in a token being taken from the
	 * source.
	 */
	bool has_more();

	/**
	 * Returns the next sentence, or an empty vector if the source has been
	 * exhausted.
	 */
	Sentence* get_next_sentence();

	/// Buffer accessor
	Token* get_buf();

private:
	/// The used source
	TokenSource* ts_;

	/// Buffer for the sigle token that needs to be kept around between
	/// get_next_sentence calls, this is always the token that begins
	/// the next sentence.
	Token* buf_;
};

} /* end ns Toki */

#endif // SENTENCESPLITTER_H
