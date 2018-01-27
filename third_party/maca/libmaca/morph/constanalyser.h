/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBMACA_CONSTANALYSER_H
#define LIBMACA_CONSTANALYSER_H

#include <libmaca/morph/morphanalyser.h>

namespace Maca {

/**
 * A simple analyser that always returns the same interpretation for any
 * token. The tag is pre-set, and the lemma is the token's orth.
 *
 * Configuration class key: \b const
 */
class ConstAnalyser : public MorphAnalyser
{
public:
	/// Constructor for a ConstAnalyser with a tagset and a tag string
	ConstAnalyser(const Corpus2::Tagset* tagset, const std::string& tag);

	/// Constructor for a ConstAnalyser with a tagset and a tag string
	ConstAnalyser(const Corpus2::Tagset* tagset, const Corpus2::Tag& tag);

	/**
	 * Config node constructor. Recognized keys are:
	 * - tag - the tag to use as the analysis for all tokens
	 * - lower_lemma - if true, lowercase the lemma (false by default)
	 */
	explicit ConstAnalyser(const Config::Node& cfg);

	/// Cloning
	ConstAnalyser* clone() const;

	/// MapAnalyser override
	bool process_functional(const Toki::Token &t
			, boost::function<void (Corpus2::Token*)> sink);

	/// Class identifier
	static const char* identifier;

	/// Registered flag
	static bool registered;
private:
	/// The tag
	Corpus2::Tag tag_;

	/// flag to lowercase lemma
	bool lower_lemma_;
};

} /* end ns Maca */

#endif // LIBMACA_CONSTANALYSER_H
