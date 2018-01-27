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

#include <iostream>
#include <boost/program_options.hpp>

#include "program_options.h"

prog_opts::variables_map program_options_handler(int argc, char **argv, const std::string& description)
{
	prog_opts::options_description desc = create_options_description(description);
	prog_opts::positional_options_description positional_desc = create_positional_options_description();

	return parse_options(argc, argv, desc, positional_desc);
}

prog_opts::options_description create_options_description(const std::string& description)
{
	prog_opts::options_description desc(description);
	desc.add_options()
		("help,h", "Print help message")
		("input-format,i", prog_opts::value<std::string>()->default_value("xces"), "set the input format")
		("output-format,o", prog_opts::value<std::string>()->default_value("xces"), "set the output format")
		("output-file,O", prog_opts::value<std::string>()->default_value(""), "set output filename (do not write to stdout)")
		("data-dir,d", prog_opts::value<std::string>()->default_value(""), "search for trainedmodel in the given dir")
		("maca-config,mc", prog_opts::value<std::string>()->default_value(""), "overrides maca config file")
		("wccl-config,wc", prog_opts::value<std::string>()->default_value(""), "overrides wccl config file")
		("ambiguity,A", prog_opts::bool_switch(), "preserve non-disamb interpretations after tagging")
		("sent-only,S", prog_opts::bool_switch(), "read sentence-by-sentence and ignore paragraphs")
		("verbose,v", prog_opts::bool_switch(), "verbose mode")
		("train", prog_opts::bool_switch(), "train the tagger")
		("batch", prog_opts::bool_switch(), "treat arguments as lists of paths to files")
		("config", prog_opts::value<std::string>()->required(), "Tagger configuration file")
		("input", prog_opts::value<std::vector< std::string> >()->required(), "Input file(s)")
	;

	return desc;
}

prog_opts::positional_options_description create_positional_options_description()
{
	prog_opts::positional_options_description positional_desc;
	positional_desc.add("config", 1);
	positional_desc.add("input", -1);
	return positional_desc;
}

prog_opts::variables_map parse_options(int argc, char **argv,
									   prog_opts::options_description desc,
									   prog_opts::positional_options_description positional_desc)
{
	prog_opts::variables_map var_map;

	try {
		prog_opts::store(prog_opts::command_line_parser(argc, argv)
									.options(desc)
									.positional(positional_desc)
									.run(), var_map);

		if(var_map.count("help")) {
			std::cout << desc << std::endl << std::endl;
			exit(EXIT_SUCCESS);
		}

		prog_opts::notify(var_map);
	} catch (prog_opts::required_option& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		std::cerr << desc << std::endl;
		exit(EXIT_FAILURE);
	} catch (prog_opts::error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		std::cerr << desc << e.what() <<std::endl;
		exit(EXIT_FAILURE);
	}

	return var_map;
}
