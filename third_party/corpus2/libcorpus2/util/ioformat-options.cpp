/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.CORPUS2, LICENSE.POLIQARP, COPYING.LESSER and COPYING files for more details.
*/

#include <libcorpus2/util/ioformat-options.h>
#include <boost/algorithm/string/join.hpp>

namespace Corpus2 {

void add_input_options(
	boost::program_options::options_description& desc,
	const std::string& default_format /*= "xces"*/
	)
{
	std::string readers = boost::algorithm::join(
		Corpus2::TokenReader::available_reader_types_help(), " ");
	std::string readers_help = "Input format, any of: " + readers + "\n";
	desc.add_options()
		("input-format,i",
		 boost::program_options::value<std::string>()->default_value(default_format),
		 readers_help.c_str());
}

void add_output_options(
	boost::program_options::options_description& desc,
	const std::string& default_format /*= "xces"*/
	)
{
	std::string writers = boost::algorithm::join(
		Corpus2::TokenWriter::available_writer_types_help(), " ");
	std::string writers_help = "Output format, any of: " + writers + "\n";
	desc.add_options()
		("output-format,o",
		 boost::program_options::value<std::string>()->default_value(default_format),
		 writers_help.c_str());
}

boost::shared_ptr<Corpus2::TokenReader> create_reader(
	boost::program_options::variables_map& vm,
	const Corpus2::Tagset& tagset,
	const std::string& filename /*= "-"*/
	)
{
	std::string format = vm["input-format"].as<std::string>();
	if (filename.empty() || filename == "-") {
		return Corpus2::TokenReader::create_stream_reader(
			format, tagset, std::cin);
	} else {
		return Corpus2::TokenReader::create_path_reader(
			format, tagset, filename);
	}
}

boost::shared_ptr<Corpus2::TokenWriter> create_writer(
	boost::program_options::variables_map& vm,
	const Corpus2::Tagset& tagset,
	const std::string& filename /*= "-"*/
	)
{
	std::string format = vm["output-format"].as<std::string>();
	if (filename.empty() || filename == "-") {
		return Corpus2::TokenWriter::create_stream_writer(
			format, std::cout, tagset);
	} else {
		return Corpus2::TokenWriter::create_path_writer(
			format, filename, tagset);
	}
}

} /* end ns Corpus2 */
