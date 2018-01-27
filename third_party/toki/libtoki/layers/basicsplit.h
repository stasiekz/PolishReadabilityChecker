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

#ifndef LIBTOKI_BASICSPLITLAYER_H
#define LIBTOKI_BASICSPLITLAYER_H

#include <libtoki/layers/outputqueue.h>

#include <unicode/utypes.h>

#include <set>

namespace Toki {

/**
 * A layer to split tokens containing one of the listed characters.
 * Extracted characters are made into separate tokens with a given type,
 * remaining bits of the orth are made tokens with the original token's
 * type.
 */
class BasicSplitLayer : public OutputQueueLayer
{
public:
	/**
	 * Constructor.
	 *
	 * Keys recognized in the configuration:
	 * - separators - List of characters treated as spearators, treated as
	 *                UTF-8 and unescaped. If it starts with [ and ends
	 *                with ], it is instead treated as a regex-like unicode
	 *                set, e.g. [a-zA-Z_]. Defaults to empty.
	 * - separator_token_type - Type to set in the separator tokens.
	 *                          Defaults to "sep".
	 */
	BasicSplitLayer(TokenSource* input, const Config::Node& props);

	/// TokenLayer override
	virtual std::string info() const;

	/// TokenLayer override
	std::string long_info() const;

	/// getter for the separator type
	const std::string& separator_type() const {
		return sep_type_;
	}
	/// getter for the separators set
	const std::set<UChar>& separators() const {
		return split_chars_;
	}

protected:
	/// OutputQueueTokenLayer override
	void prepare_more_tokens(Token* t);

	/// test function for the split chars
	bool is_split_char(UChar c);

private:
	/// the split characters
	std::set<UChar> split_chars_;

	/// type of the extracted tokens
	std::string sep_type_;
};

} //end namespace Toki

#endif // LIBTOKI_BASICSPLITLAYER_H
