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

/**
 * @file main.cpp
 * @brief Tool for using WCRFT through command line interface.
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <libcorpus2/exception.h>

#include <libwcrft/config.h>
#include <libwcrft/corpusio.h>
#include <libwcrft/tagger.h>
#include <libwcrft/pathsearcher.h>

#include "program_options.h"

const std::string ADDITIONAL_FORMAT_INFO = 
"Supported I/O formats: txt premorph; require installed Maca and Morfeusz\n" +
boost::algorithm::replace_all_copy(Wcrft::FORMAT_HELP, "input formats: ccl", 
								   "input formats: txt premorph ccl");

const std::string DESCRIPTION = "wcrft [options] CONFIGFILE [INPUT...]\n\
\n\
WCRFT, Wroclaw CRF Tagger\n\
(C) 2012, Wroclaw University of Technology\n\
\n\
Tags input file(s) using the selected configuration (e.g. nkjk_e2.ini).\
Configurations may provide default name of a trained tagger model\
(the standard configurations do) so if both the tagger and the model\
is installed properly, you don't have to worry about traned tagger model.\
Otherwise, you can use -d to specify where to look for a trained tagger model.\
This may also be used to override default model dir. When training,\
use -d to specify a directory where trained model should be saved.\n\
\n\
Use -O to specify output path (by default will write to stdout).\n\
Use - to tag stdin to stdout.\n\
\n\
When tagging multiple files, either give the filenames directly as arguments,\
or use --batch and provide a filename to a list of paths. Either way, each file \
will be tagged and the output written to FILENAME.tag.\n\
\n\
Training (--train) requires a configuration to use and a filename of the training\
material. Trained model will be stored in DATA_DIR.\n\
\n" + ADDITIONAL_FORMAT_INFO;

typedef std::pair<std::string, std::string> FilenamePair;
typedef std::vector<FilenamePair> FilenamePairs;
typedef std::vector<std::string> Filenames;

int run_app(const prog_opts::variables_map& var_map);
int train_tagger(Wcrft::Tagger& tagger, const Filenames& input_files, const std::string input_format);
int run_tagger(Wcrft::Tagger& tagger, Filenames input_files, const prog_opts::variables_map& config);
void tag_one(Wcrft::Tagger& tagger, FilenamePair io_pair, const prog_opts::variables_map& config);

void update_taggers_config(Wcrft::Tagger& tagger, const prog_opts::variables_map& var_map);
FilenamePairs generate_filename_pairs(const Filenames &input_files);
FilenamePairs create_in_out_filenames(Filenames input_files, const prog_opts::variables_map& config);
Filenames unbatch_input_files(const Filenames &input_files);

int main(int argc, char** argv)
{
	try
	{
		prog_opts::variables_map var_map = program_options_handler(argc, argv, DESCRIPTION);
		return run_app(var_map);
	} catch (Wcrft::FileNotFound &e) {
		std::cerr << "Error: " << e.info() << std::endl;
		return 1;
	} catch (Corpus2::Corpus2Error &e) {
		std::cerr << "Error: " << e.info() << std::endl;
		return 1;
	}
}

int run_app(const prog_opts::variables_map& var_map)
{
	using namespace std;

	string config_path = var_map["config"].as<string>();
	string data_dir = var_map["data-dir"].as<string>();

	Wcrft::Tagger tagger(config_path, data_dir);
	update_taggers_config(tagger, var_map);

	Filenames input_files = var_map["input"].as< vector<string> >();
	if(var_map["train"].as<bool>()) {
		string input_format = var_map["input-format"].as<string>();
		return train_tagger(tagger, input_files, input_format);
	} else {
		// normal tagger performance
		return run_tagger(tagger, input_files, var_map);
	}
}

int train_tagger(Wcrft::Tagger& tagger, const Filenames& input_files, const std::string input_format)
{
	if(input_files.size() == 1) {
		std::string training_filename = input_files[0];
		tagger.train_and_save(training_filename, input_format);
		return 0;
	}
	else
	{
		std::cerr << "Tagger must be trained using exactly one input file only." << std::endl;
		return 1;
	}
}

int run_tagger(Wcrft::Tagger& tagger, Filenames input_files, const prog_opts::variables_map& config)
{
	FilenamePairs in_out_filenames = create_in_out_filenames(input_files, config);
	tagger.load_model();
	BOOST_FOREACH(FilenamePair io_pair, in_out_filenames) {
		tag_one(tagger, io_pair, config);
	}
	return 0;
}

void tag_one(Wcrft::Tagger& tagger, const FilenamePair io_pair, const prog_opts::variables_map& config)
{
	std::string in_file = io_pair.first;
	std::string out_file = io_pair.second;

	std::string input_format = config["input-format"].as<std::string>();
	std::string output_format = config["output-format"].as<std::string>();

	// the function will treat empty strings as stdin/stdout
	tagger.tag_input(in_file, input_format, out_file, output_format);
}

void update_taggers_config(Wcrft::Tagger& tagger, const prog_opts::variables_map& var_map)
{
	tagger.switch_paragraph_processing(!var_map["sent-only"].as<bool>());
	tagger.switch_preserve_ambiguity(var_map["ambiguity"].as<bool>());
	tagger.switch_verbose(var_map["verbose"].as<bool>());

	std::string maca_config = var_map["maca-config"].as<std::string>();
	if(!maca_config.empty())
		tagger.set_maca_config(maca_config);

	std::string wccl_config = var_map["wccl-config"].as<std::string>();
	if(!wccl_config.empty())
		tagger.set_wccl_config(wccl_config);
}

FilenamePairs create_in_out_filenames(Filenames input_files, const prog_opts::variables_map& var_map)
{
	FilenamePairs io_pairs;

	if(var_map["batch"].as<bool>()) {
		// every file contains list of filenames
		input_files = unbatch_input_files(input_files);
		// autogen output file names (empty_out = false)
		io_pairs = generate_filename_pairs(input_files);
	}
	else if(input_files.size() == 1) {
		std::string in_file = input_files[0];
		if(!in_file.compare("-")) {
			// stdin to stdout
			io_pairs.push_back(std::make_pair("", ""));
		}
		else {
			// user-provided input to user-provided output (or stdout if empty)
			std::string out_file = var_map["output-file"].as<std::string>();
			io_pairs.push_back(std::make_pair(in_file, out_file));
		}
	}
	else {
		// multiple input files provided as args
		io_pairs = generate_filename_pairs(input_files);
	}

	return io_pairs;
}

Filenames unbatch_input_files(const Filenames &input_files)
{
	Filenames unbatched;

	BOOST_FOREACH(const std::string &in_filename, input_files) {
		std::string line;
		std::ifstream batch_file;

		batch_file.open(in_filename.c_str());
		while(!batch_file.eof()) {
			std::getline(batch_file, line);
			boost::algorithm::trim(line);
			if(! line.empty())
				unbatched.push_back(line);
		}
		batch_file.close();
	}

	return unbatched;
}

FilenamePairs generate_filename_pairs(const Filenames &input_files)
{
	// generate output file names by suffixing input file names
	static const std::string OUTPUT_SUFFIX(".tag");
	FilenamePairs io_pairs;
	BOOST_FOREACH(std::string in_filename, input_files) {
		std::string out_filename = in_filename + OUTPUT_SUFFIX;
		io_pairs.push_back(std::make_pair(in_filename, out_filename));
	}
	return io_pairs;
}
