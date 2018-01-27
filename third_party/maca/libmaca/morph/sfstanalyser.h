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

#ifndef LIBMACA_SFSTMORPHANALYSER_H
#define LIBMACA_SFSTMORPHANALYSER_H

#include <libmaca/morph/morphanalyser.h>
#include <libmaca/typedefs.h>
#include <boost/thread/mutex.hpp>

/// forwrd declaration of a SFST type
class CompactTransducer;

namespace Maca {

/**
 * A morphological analyser using a SFST compact transducer.
 *
 * The transducer file should be prepared by the fst-compiler-utf8 program
 * from the SFST suite, with the -c option to create a compact transducer.
 *
 * The expected behaviour of the transducer is to return, when given an
 * orth it recognizes, one or more lemma-tag strings. A lemma-tag string
 * should contain the lemma, followed by a less-than sign (&lt;), followed
 * by the tag-string, followed by a more-than sign (&gt). The tag string
 * is composed of one or more tag identifiers in the colon-dot format,
 * tag identifiers should be separated by a pipe character (|). An example
 * lemma-tag string is "somelemma&lt;pos1:foo:bar|pos2:foo:bar.baz&gt;".
 *
 * Input for the fst-compiler tool can be prepared from an orth-lemma-tag
 * tab-separated dictionary by the tab-to-sfst tool which should be bundled
 * with the library.
 *
 * Configuration class key: \b sfst
 */
class SfstAnalyser : public MorphAnalyser, private boost::noncopyable
{
public:
	/// Constructor for a SFST analyser working with a tagset and using
	/// a transducer loaded from thegiven file
	SfstAnalyser(const Corpus2::Tagset* tagset, const std::string& filename);

	/**
	 * Config node constructor. recognized keys are:
	 * - file - the transducer file
	 * - lower-case - lowercase all input to the transducer
	 */
	SfstAnalyser(const Config::Node& cfg);

	/// Destructor
	~SfstAnalyser();

	/// Cloning
	SfstAnalyser* clone() const;

	/// MorphAnalyser override
	bool process_functional(const Toki::Token& t,
			boost::function<void (Corpus2::Token*)> sink);

	/// helper function for interfacing with the transducer output
	static std::string unescape_analysis(const std::string& sfst_analysis);

	/// Class identifier
	static const char* identifier;

	/// Registered flag
	static bool registered;

private:
	/// ctor for use in clone()
	SfstAnalyser(const Corpus2::Tagset *tagset);

	/// convenience function for loading a transducer file
	void open_transducer(const std::string& filename);

	/// the transducer, potentially shared between clones
	boost::shared_ptr<CompactTransducer> ct_;

	/// Mutex for safe access in case there are clones
	boost::shared_ptr<boost::mutex> mutex_;

	/// force lower case flag
	bool lcase_;
};

} /* end ns Maca */

#endif // LIBMACA_SFSTMORPHANALYSER_H
