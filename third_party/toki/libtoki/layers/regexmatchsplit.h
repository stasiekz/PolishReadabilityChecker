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

#ifndef LIBTOKI_REGEXMATCHSPLITLAYER_H
#define LIBTOKI_REGEXMATCHSPLITLAYER_H

#include <libtoki/layers/outputqueue.h>

#include <unicode/regex.h>

namespace Toki {

/**
 * A layer to extract parts of a token that mach a specified regular
 * expression. Parts that match are extracted into separate tokens with the
 * given type, remaining parts retain the original token's type.
 */
class RegexMatchSplitLayer : public OutputQueueLayer
{
public:
	/**
	 * Constructor.
	 *
	 * Keys recognized in the configuration:
	 * - regex                - The regular expression to use.
	 * - separator_token_type - Type to set in the extracted tokens. Defaults
	 *                          to "sep".
	 */
	RegexMatchSplitLayer(TokenSource* input, const Config::Node& props);

	/// Destructor
	~RegexMatchSplitLayer();

	/// TokenLayer override
	virtual std::string info() const;

	/// TokenLayer override
	std::string long_info() const;

	/// getter for the separator (extracted token) type
	const std::string& separator_type() const {
		return sep_type_;
	}
protected:
	/// OutputQueueLayer override
	void prepare_more_tokens(Token *t);

private:
	/// the regex
	RegexMatcher* regex_;

	/// token type for extracted token parts
	std::string sep_type_;
};

} /* end ns Toki */

#endif // LIBTOKI_REGEXMATCHSPLITLAYER_H
