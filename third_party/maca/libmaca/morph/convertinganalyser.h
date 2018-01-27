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

#ifndef LIBMACA_MORPH_CONVERTINGANALYSER_H
#define LIBMACA_MORPH_CONVERTINGANALYSER_H

#include <libmaca/morph/morphanalyser.h>
#include <libmaca/conv/tagsetconverter.h>
#include <boost/scoped_ptr.hpp>

namespace Maca {

/**
 * A wrapper analyser that passes the output of some analyser through a
 * converter.
 *
 * Configuration class key: \b wrap_convert
 */
class ConvertingAnalyser : public MorphAnalyser
{
public:
	/**
	 * Create a converting analyser from an analyser and a converter
	 */
	ConvertingAnalyser(MorphAnalyser* ma, Conversion::TagsetConverter* conv);

	/**
	 * Config node constructor. Recognized keys are:
	 * - wrapped_class - the actual analyser to instantiate and wrap
	 * - wrapped_converter - the converter to use
	 * - wrapped_tagset - tagset of the wrapped analyser -- the 'tagset' key is
	 *                    the tagset of this analyser, so should be equal to
	 *                    the output tagset of the converter. Conversely, the
	 *                    input tagste of the converter should be the wrapped
	 *                    tagset.
	 * Keys are passed to the wrapped analyser, with the tagset substituted
	 * with the wrapped_tagset.
	 */
	explicit ConvertingAnalyser(const Config::Node& cfg);

	/// Cloning
	ConvertingAnalyser* clone() const;

	/// MapAnalyser override
	bool process_functional(const Toki::Token &t
			, boost::function<void (Corpus2::Token*)> sink);

	/// Class identifier
	static const char* identifier;

	/// Registered flag
	static bool registered;
private:
	/// The wrapped analyser
	boost::scoped_ptr<MorphAnalyser> wrapped_;

	/// The converter object
	boost::scoped_ptr<Conversion::TagsetConverter> converter_;
};

} /* end ns Maca */

#endif // LIBMACA_MORPH_CONVERTINGANALYSER_H
