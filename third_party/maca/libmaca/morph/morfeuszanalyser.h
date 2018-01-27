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

#ifndef LIBMACA_MORFEUSZANALYSER_H
#define LIBMACA_MORFEUSZANALYSER_H

#include <libmaca/morph/morphanalyser.h>
#include <libmaca/conv/tagsetconverter.h>

namespace Maca {

/// Compatibility structure for data returned by Morfeusz
struct MorfeuszData {
	int node_from;
	int node_to;
	char* orth;
	char* lemma;
	char* tag_string;
};

/// Helper struct for holding preprocessed Morfeusz results
struct MorfeuszEdge
{
	explicit MorfeuszEdge(const MorfeuszData& morf);

	int node_from, node_to;
	UnicodeString orth;
	UnicodeString lemma;
	std::string tag_string;
	Corpus2::Token* token;
};

/**
 * Exception class for signalling Morfeusz startup errors
 */
class MorfeuszInitError : public MacaError
{
public:
	/// Constructor
	MorfeuszInitError(const std::string& error, const std::string& extra,
			const std::string& name);

	/// Destructor
	~MorfeuszInitError() throw();

	/// Info accessor
	std::string info() const;

	/// the error info, extra info and (requested) Morfeusz library name
	std::string error, extra, name;
};

/**
 * Exception class for signalling Morfeusz-related analysis errors
 */
class MorfeuszError : public MacaError
{
public:
	/// Constructor
	MorfeuszError(const std::string& error, const std::string input,
		const std::vector<MorfeuszEdge>& interp);

	/// Destructor
	~MorfeuszError() throw();

	/// Info accessor
	std::string info() const;

	/// The error info and Morfeusz input during the error, if available
	std::string error, input;

	/// The structure returned by Morfeusz during the error, if available
	std::vector<MorfeuszEdge> interp;
};

/**
 * A morphological analyser class that interfaces with the Morfeusz library.
 *
 * Morfeusz requires a tagset converter since it sometimes returns ambiguous
 * token segmentation. The converter should modify tokens coming from
 * Morfeusz so that an unambiguous segmentation is possible.
 */
class MorfeuszAnalyser : public MorphAnalyser
{
public:
	/**
	 * Constructor for a Morfeusz analyser with a given tagset and converter.
	 * The tagset should be the output tagset of the converter.
	 */
	MorfeuszAnalyser(const Corpus2::Tagset* tagset,
			Conversion::TagsetConverter* conv,
			const std::string& libname,
			const std::string& require_version);

	/**
	 * Config node constructor. Recognized keys are:
	 * - converter - the converter to load (from standard paths)
	 * - ign_tag - the tag to use when Morfeusz returns no analysis,
	 *             defaults to "ign"
	 * - warn_on_ign - warn when using the ign tag, false by default
	 * - warn_on_fold_failure - issue a warning when folding ambiguous paths
	 *                          is unsuccesful after conversion (off by def.)
	 * - library - the Morfeusz library to use, defaults to a "reasonable"
	 *             name that should work. Pass a soname or full path.
	 * - require_version - check for the presence of the version string in
	 *                     the output of morfeusz_about() and trigger an
	 *                     error if it is not found.
	 *                     "Morfeusz SIAT" by default
	 */
	MorfeuszAnalyser(const Config::Node& cfg);

	/// Cloning
	MorfeuszAnalyser* clone() const;

	/// Destructor
	~MorfeuszAnalyser();

	/// MorphAnalyser override
	bool process_functional(const Toki::Token &t,
			boost::function<void(Corpus2::Token *)> sink);

	/// helper to create a token from a Morfeusz interpretation struct
	Corpus2::Token* make_token(const Toki::Token& t,
			const MorfeuszEdge& m) const;

	/// helper to add lexemes from a Morfeusz interp struct into a token
	void morfeusz_into_token(Corpus2::Token* tt, const MorfeuszEdge& m) const;

	/// convert gathered tokens and pass them to the sink
	void flush_convert(std::vector<Corpus2::Token*>& vec,
			boost::function<void(Corpus2::Token *)> sink);

	/// convert gethered tokens (ambiguously segmented), try folding and
	/// pass the resulting tokens to the sink
	void flush_convert(std::vector< std::vector<Corpus2::Token*> >& vec,
			boost::function<void(Corpus2::Token *)> sink);

	/// Class identifier
	static const char* identifier;

	/// Registered flag
	static bool registered;

private:
	bool process_complex_analysis(const Toki::Token &t,
			std::vector<MorfeuszEdge>& pmorf,
			boost::function<void(Corpus2::Token *)>sink);

	void load_morfeusz_library();

	/// the tagset converter
	Conversion::TagsetConverter* conv_;

	Corpus2::Tag ign_tag_;

	bool warn_on_ign_;

	bool warn_on_fold_failure_;

	std::string morfeusz_library_;

	std::string require_version_;

	void* morfeusz_lib_handle_;

	typedef MorfeuszData* (*morfeusz_func_t)(char*);

	morfeusz_func_t morfeusz_analyse_handle_;
};

} /* end ns Maca */

#endif // LIBMACA_MORFEUSZANALYSER_H
