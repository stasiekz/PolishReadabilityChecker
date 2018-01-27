/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/
#ifndef LIBMACA_MORPH_GUESSER2_H
#define LIBMACA_MORPH_GUESSER2_H

#include <libmaca/morph/morphanalyser.h>
#include <libmaca/typedefs.h>
#include <boost/filesystem.hpp>
#include <libcorpus2/guesser/guesser.h>

namespace Maca {

/**
 * Corpuslib guesser analyser interface.
 * 
 * WARNING: There is no thread safety currently
 */
class Guesser2Analyser : public MorphAnalyser
{
public:
	/**
	 * Constructor for a Guesser analyser with the given tagset and a path to its learned data
	 */
	Guesser2Analyser(const Corpus2::Tagset* tagset, const boost::filesystem::path & data);

	/**
	 * Config node constructor. Recognized keys are:
	 * - guesser_data - path to learned data for Corpus2::Guesser
	 */
	Guesser2Analyser(const Config::Node& cfg);

	/// Cloning
	Guesser2Analyser* clone() const;

	/// Destructor
	~Guesser2Analyser();

	/// MorphAnalyser override
	bool process_functional(const Toki::Token& t,
			boost::function<void (Corpus2::Token*)> sink);

	/// Class identifier
	static const char* identifier;

	/// Registered flag
	static bool registered;
private:
	Corpus2::Guesser guesser;
};

} /* end ns Maca */

#endif // LIBMACA_MORPH_GUESSER2_H
