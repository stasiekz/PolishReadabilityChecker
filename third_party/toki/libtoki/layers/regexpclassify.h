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

#ifndef LIBTOKI_REGEXPCLASSIFYLAYER_H
#define LIBTOKI_REGEXPCLASSIFYLAYER_H

#include <libtoki/layers/tokenlayer.h>

#include <unicode/regex.h>

namespace Toki {

/**
 * Classification layer that changes a token type if the orth matches one
 * of the regular expressions given.
 *
 * Each regexp has a type attached to it that will be set in the token.
 * Rules are matched in order, the first matching rule taking effect. If a
 * token does not match any of the regexps, its type is not modified.
 */
class RegexpClassifyLayer : public TokenLayer
{
public:
	/**
	 * Constructor.
	 *
	 * Keys recognized in the configuration are of a pattern ``type:TYPE''
	 * TYPE indicates the type to set, the value of the config variable is
	 * the regular expression to use.
	 */
	RegexpClassifyLayer(TokenSource* input, const Config::Node& props);

	/// Destrctor
	~RegexpClassifyLayer();

	/// TokenLayer override
	Token* process_token(Token *t);

	/// TokenLayer override
	virtual std::string info() const;

	/// TokenLayer override
	std::string long_info() const;

private:
	/// token type + regexp vector
	std::vector< std::pair<std::string, RegexMatcher*> > classifiers_;
};

} //end namespace Toki

#endif // LIBTOKI_REGEXPCLASSIFYLAYER_H
