/* This file is part of WCRFT
  Copyright (C) 2014 Radosław Warzocha, Adam Radziszewski.
  WCRFT is free software; you can redistribute and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option)
  any later version.

  WCRFT is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.

  See the LICENCE and COPYING files for more details
 */

#ifndef WCRFT_CONFIG_H
#define WCRFT_CONFIG_H

#include <libtoki/util/confignode.h>

#include "pathsearcher.h"

namespace Wcrft {

namespace TConf = Toki::Config;
typedef TConf::Node Config;

inline Config read_config_file(const std::string& config_filename)
{
	std::string config_path = WcrftPathSearcherSingleton::Instance().find_file_or_throw(config_filename, "config");
	return TConf::from_file(config_path);
}

// special section that are typically set by the user rather than keeping in
// config file (although keeping them there is also possible)
const std::string CONFIG_S_USER = "user";
const std::string CONFIG_O_VERBOSE = "verbose";
const std::string CONFIG_O_PARA = "paragraphs"; // read paragraph-by-paragraph
const std::string CONFIG_O_AMBOUT = "ambiguity"; // preserve all tags at output


// global section with tagset-related definitions
const std::string CONFIG_S_GLOBAL = "general";
const std::string CONFIG_O_TAGSET = "tagset";
const std::string CONFIG_O_FEATS = "features";
const std::string CONFIG_O_MACACFG = "macacfg";
const std::string CONFIG_O_WCCLCFG = "wcclcfg";
const std::string CONFIG_O_MODELNAME = "defaultmodel";

// comma-separated list of tagset attributes, whose order will be translated into tagging layers
// NOTE: use attribute mnemonics as defined in corpus2 tagset (see .tagset file or run tagset-tool to check)
const std::string CONFIG_O_ATTRS = "attrs";

// settings related to gathering of lexicon of frequent forms
const std::string CONFIG_S_LEXICON = "lexicon";
const std::string CONFIG_O_CASESENS = "casesens";     // default: no
const std::string CONFIG_O_MINFREQ = "minfreq";       // default: 10
const std::string CONFIG_O_MAXENTRIES = "maxentries"; // default: 500

// tagging of unknown words
const std::string CONFIG_S_UNKNOWN = "unknown";
const std::string CONFIG_O_UNKGUESS = "guess";     // shall we tag unknown forms?
const std::string CONFIG_O_UNKFREQ = "unktagfreq"; // gathering unk tags occurring at least this frequent
const std::string CONFIG_O_UNKLOWER = "lowerlemmas"; // lowercase lemmas

// attrname for wordclass
const std::string CONFIG_WORDCLASS = "CLASS";

// WCCL file section defining ops for all layers
const std::string CONFIG_DEFAULT_OPS = "default";

// file extensions
const std::string CONFIG_EXT_INI = "ini";
const std::string CONFIG_EXT_LEX = "lex";
const std::string CONFIG_EXT_UNKTAGS = "unk"; // for storing unknown word tags
const std::string CONFIG_EXT_WCCL = "ccl";
const std::string CONFIG_EXT_DATA = "tab";
const std::string CONFIG_EXT_TEXT = "txt";
const std::string CONFIG_EXT_CR = "cr";
const std::string CONFIG_EXT_LOG = "log";

// classifier-specific options
const std::string CONFIG_S_CLASSIFIER = "crf";
const std::string CONFIG_O_PARAMS = "params";


/**
 * @brief Stores instance-specific configuration and paths; gives access to
 * tagger configuration and paths.
 *
 * Stores tagger configuration read from tagger config file. Able to resolve
 * necessary paths that are specific to a particular tagger instance.
 * Gives access to configuration options, provides getters for paths to config
 * and model files.
 */
class WcrftConfig {
public:
	/**
	 * @brief Read given configuration file and resolve paths.
	 * @param config_name configuration name (with or without ".ini")
	 *        or full path to congiguration file.
	 * @param model_dir model_dir path to tagger model or empty string if model
	 *        location is to be taken from the configuration.
	 */
	WcrftConfig(const std::string& config_name,
				const std::string& model_dir);

	/**
	 * @brief Return path to a additional dependency of tagger config using
	 * standard naming conventions and directory from which the INI was loaded.
	 * @param extension file extension.
	 * @param suffix optional suffix.
	 * @return string containing the path.
	 */
	inline const std::string get_config_filename(
			const std::string& extension,
			const std::string& suffix = "") const
	{
		return get_filename(config_dir_, config_name_, extension, suffix);
	}

	/**
	 * @brief Return path to a file inside model dir using config name
	 * as filename stem and optional suffix.
	 * @param extension file extension.
	 * @param suffix optional suffix.
	 * @return string containing the path.
	 */
	inline const std::string get_model_filename(
			const std::string& extension,
			const std::string& suffix = "") const
	{
		return get_filename(model_dir_, config_name_, extension, suffix);
	}

	/**
	 * @brief Return dir from which INI was loaded.
	 */
	const std::string get_config_dir() const {
		return config_dir_;
	}

	/**
	 * @brief Return model dir.
	 */
	const std::string get_model_dir() const {
		return model_dir_;
	}

	/**
	 * @brief Return config option stored under key @a option. If
	 * there is no such option or it is of different type than specified,
	 * exception is thrown.
	 */
	template<typename T>
	T get_config_option(const std::string &option) const
	{
		return config_.get<T>(option);
	}

	/**
	 * @brief Return config option stored under key @a option. If
	 * there is no such option, @a def_val is returned.
	 */
	template<typename T>
	T get_config_option(const std::string &option, T def_val) const
	{
		return config_.get<T>(option, def_val);
	}

	/**
	 * @brief Return config option stored under key @a option in @c section
	 * (full key is @a section.option). If there is no such option, 
	 * exception is thrown.
	 */
	template<typename T>
	T get_config_section_option(const std::string &section, const std::string &option) const
	{
		const std::string full_name = section + "." + option;
		return config_.get<T>(full_name);
	}

	/**
	 * @brief Return config option stored under key @a option in @c section
	 * (full key is @a section.option). If there is no such option, 
	 * @a def_val is returned.
	 */
	template<typename T>
	T get_config_section_option(const std::string &section, const std::string &option, T def_val) const
	{
		const std::string full_name = section + "." + option;
		return config_.get<T>(full_name, def_val);
	}

	/// @brief Set config option stored under @a option key to @a value.
	template<typename T>
	void set_config_option(const std::string &option, T value)
	{
		config_.put<T>(option, value);
	}

	/// @brief Set config option stored under @a option key in @a section to @a value.
	template<typename T>
	void set_config_section_option(const std::string &section, const std::string &option, T value)
	{
		std::string full_name = section + "." + option;
		set_config_option(full_name, value);
	}


private:
	const std::string get_filename(
		const std::string& dir,
		const std::string& base_name,
		const std::string& extension,
		const std::string& suffix) const;

	/// parsed INI file with tagger config.
	Config config_;
	/// path to directory containing config file and its dependents.
	std::string config_dir_;
	/// name of config (without ".ini", e.g. "nkjp_e2").
	std::string config_name_;
	/// path to directory where tagger model is sought or written to.
	std::string model_dir_;
};

}

#endif // WCRFT_CONFIG_H
