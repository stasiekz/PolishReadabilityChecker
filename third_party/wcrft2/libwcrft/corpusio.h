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

#ifndef WCRFT_CORPUSIO_H
#define WCRFT_CORPUSIO_H

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <libcorpus2/tag.h>
#include <libcorpus2/tagset.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/io/reader.h>
#include <libcorpus2/io/writer.h>
#include <libwccl/values/value.h>

#include "config.h"

namespace Wcrft {

const std::string FORMAT_HELP =
"Available input formats: " +
boost::algorithm::join(Corpus2::TokenReader::available_reader_types(), " ") + "\n" +
"Available output formats: " +
boost::algorithm::join(Corpus2::TokenWriter::available_writer_types(), " ") + "\n";

const std::string WCRFT_PLAIN_TEXT_FORMAT = "text";
const std::string WCRFT_PLAIN_TEXT_FORMAT_ALT = "txt";
const std::string WCRFT_PREMORPH_TEXT_FORMAT = "premorph";

/**
 * @brief Get @c Corpus2::TokenReader that reads from file given in the first parameter.
 * If filename is empty, reads from stdin.
 *
 * @param input_path File to read from
 * @param input_format See
 * <a href="http://nlp.pwr.wroc.pl/redmine/projects/wcrft/wiki/User_guide#Output-formats">
 * User guide </a> for help on input/output formats
 * @param tagset
 * @param maca_config Optional. Name of MACA config file to override default filename.
 */
boost::shared_ptr<Corpus2::TokenReader> get_reader(
		const std::string& input_path, const std::string& input_format,
		const Corpus2::Tagset& tagset, const std::string& maca_config = "");


/**
 * @brief Get @c Corpus2::TokenReader that reads from stream given in the first parameter.
 *
 * @param input_stream Stream to read from
 * @param input_format See
 * <a href="http://nlp.pwr.wroc.pl/redmine/projects/wcrft/wiki/User_guide#Output-formats">
 * User guide </a> for help on input/output formats
 * @param tagset
 * @param maca_config Optional. Name of MACA config file to override default filename.
 */
boost::shared_ptr<Corpus2::TokenReader> get_reader(
		std::istream& input_stream, const std::string& input_format,
		const Corpus2::Tagset& tagset, const std::string& maca_config = "");


/**
 * @brief Get @c Corpus2::TokenWriter that writes to file given in the first parameter.
 * If filename is empty, writes to stdout.
 *
 * @param output_path File to write to
 * @param output_format See
 * <a href="http://nlp.pwr.wroc.pl/redmine/projects/wcrft/wiki/User_guide#Output-formats">
 * User guide </a> for help on input/output formats
 * @param tagset
 */
boost::shared_ptr<Corpus2::TokenWriter> get_writer(
		const std::string& output_path, const std::string& output_format,
		const Corpus2::Tagset& tagset);


/**
 * @brief Get @c Corpus2::TokenWriter that writes to stream given in the first parameter.
 *
 * @param output_stream Stream to write to
 * @param output_format See
 * <a href="http://nlp.pwr.wroc.pl/redmine/projects/wcrft/wiki/User_guide#Output-formats">
 * User guide </a> for help on input/output formats
 * @param tagset
 */
boost::shared_ptr<Corpus2::TokenWriter> get_writer(
		std::ostream& output_stream, const std::string& output_format,
		const Corpus2::Tagset& tagset);

inline const std::string mask2text(const Corpus2::Tagset& tagset, Corpus2::Tag& mask) 
{
	if (mask.is_null())
		return "-";
	else
		return boost::algorithm::join(tagset.tag_to_symbol_string_vector(mask), "-");
}

inline const Corpus2::Tag text2mask(const Corpus2::Tagset& tagset, const std::string& text)
{
	if (text.empty() || text.compare("-") == 0)
		return Corpus2::Tag();
	else {
		std::string text_copy = boost::algorithm::replace_all_copy(text, ",", "-");
		return tagset.parse_symbol_string(text_copy);
	}
}

inline const std::string value2text(const Corpus2::Tagset& tagset, const Wccl::Value& value)
{
	return value.to_string(tagset);
}
}

#endif // WCRFT_CORPUSIO_H
