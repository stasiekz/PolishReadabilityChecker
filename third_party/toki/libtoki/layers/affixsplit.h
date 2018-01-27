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

#ifndef LIBTOKI_AFFIXSPLITLAYER_H
#define LIBTOKI_AFFIXSPLITLAYER_H

#include <libtoki/layers/outputqueue.h>

#include <set>

namespace Toki {

/**
 * Layer to recognize and split prefix and posftix characters in tokens.
 * All prefix characters are extracted as separate tokens, then the
 * unrecognized part is output as the body token (with the type
 * unmodified), and then the suffix characters are output.
 */
class AffixSplitLayer : public OutputQueueLayer
{
public:
	/**
	 * Constructor.
	 *
	 * Keys recognized in the configuration:
	 * - prefix_chars  - List of prefix characters, treated as UTF-8 and
	 *                   unescaped. If it starts with [ and ends with ],
	 *                   it is instead treated as a regex-like unicode set,
	 *                   e.g. [a-zA-Z_]. Defaults to empty.
	 * - suffix_chars -  List of suffix characters, treated as UTF-8 and
	 *                   unescaped. If it starts with [ and ends with ],
	 *                   it is instead treated as a regex-like unicode set,
	 *                   e.g. [a-zA-Z_]. Defaults to empty.
	 * - prefix_token_type  - Token type to set in the extracted prefixes.
	 *                        Defaults to "pre".
	 * - suffix_token_type - Token type to set in the extracted suffixes.
	 *                        Defaults to "suf".
	 */

	AffixSplitLayer(TokenSource* input, const Config::Node& props);

	/// TokenLayer override
	virtual std::string info() const;

	/// TokenLayer override
	std::string long_info() const;

	/// getter for the type of the extracted prefix tokens
	const std::string& prefix_type() const {
		return prefix_type_;
	}

	/// getter for the type of the extracted suffix tokens
	const std::string& suffix_type() const {
		return suffix_type_;
	}

	/// prefix characters set accessor
	const std::set<UChar>& prefix_chars() const {
		return prefix_chars_;
	}

	/// suffix characters set accessor
	const std::set<UChar>& suffix_chars() const {
		return suffix_chars_;
	}

protected:
	/// OutputQueueLayer override
	void prepare_more_tokens(Token* t);

	/// test function for prefix characters
	bool is_prefix_char(UChar c);

	/// test function for suffix characters
	bool is_suffix_char(UChar c);

private:
	/// prefix characters
	std::set<UChar> prefix_chars_;

	/// suffix characters
	std::set<UChar> suffix_chars_;

	/// type of the extracted prefix tokens
	std::string prefix_type_;

	/// type of the extracted suffix tokens
	std::string suffix_type_;
};

} //end namespace Toki

#endif // LIBTOKI_AFFIXSPLITLAYER_H
