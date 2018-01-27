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

#include <cstdlib>
#include <iostream>

#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include "classify.h"
#include "corpusio.h"

namespace Wcrft {

namespace details {
	const std::string DATA_SEP = "\t";
}

CRFTaggerPtr load_classifier(std::string attribute_name, const WcrftConfig& tagger_conf)
{
	std::string crf_filename = tagger_conf.get_model_filename(
				CONFIG_EXT_CR, attribute_name);
	boost::filesystem::path crf_path(crf_filename);

	CRFPP::Tagger *tagger;
	if(boost::filesystem::is_regular_file(crf_path)) {
		std::string options = "-m " + crf_filename;
		tagger = CRFPP::createTagger(options.c_str());
		return boost::shared_ptr<CRFPP::Tagger>(tagger);
	} else {
		// there's no trained model for this attr, return NULL
		boost::shared_ptr<CRFPP::Tagger> nullish;
		return nullish;
	}
}

void classifier_open_sentence(CRFTaggerPtr crf_tagger)
{
	crf_tagger->clear();
}
void classifier_close_sentence(CRFTaggerPtr crf_tagger)
{
	crf_tagger->parse();
}

void eat_token(CRFTaggerPtr crf_tagger, std::vector<std::string> featured_vals)
{
	std::string instance = boost::algorithm::join(featured_vals, details::DATA_SEP);
	crf_tagger->add(instance.c_str());
}

std::string classify_token(CRFTaggerPtr crf_tagger, int token_id)
{
	return crf_tagger->y2(token_id);
}

TrainingFiles open_training_files(WcrftConfig& config, boost::shared_ptr<Layers> layers)
{
	TrainingFiles training_files;
	BOOST_FOREACH(const LayerPtr layer, layers->get_layers()) {
		const std::string attribute_name = layer->get_attribute_name();
		const std::string filename =
				config.get_model_filename(CONFIG_EXT_DATA, attribute_name);
		training_files.insert(
					std::make_pair(
						attribute_name,
						boost::make_shared<std::ofstream>(filename.c_str())));
	}

	return training_files;
}

void close_training_files(TrainingFiles& training_files)
{
	BOOST_FOREACH(TrainingFiles::value_type& tfile, training_files) {
		tfile.second->close();
	}
}

void train_classifier_and_save(const std::string attribute_name, const WcrftConfig& tagger_conf)
{
	const std::string tr_filename = tagger_conf.get_model_filename(CONFIG_EXT_DATA, attribute_name);
	const std::string cr_filename = tagger_conf.get_model_filename(CONFIG_EXT_CR, attribute_name);
	const std::string cr_template_filename = tagger_conf.get_config_filename(CONFIG_EXT_TEXT, attribute_name);
	const std::string crf_opts = tagger_conf.get_config_section_option<std::string>(CONFIG_S_CLASSIFIER, CONFIG_O_PARAMS);

	const std::string log_filename = tagger_conf.get_model_filename(CONFIG_EXT_LOG);
	const std::string args[] = {"crf_learn", cr_template_filename, tr_filename, cr_filename};
	const std::string command = boost::algorithm::join(args, " ") + " " + crf_opts;
	const std::string redir_command = command + " >> " + log_filename + " 2>&1";

	int retval = system(redir_command.c_str());
	if(retval != 0) {
		const std::string message = "CRF classifier training failed! "
									"Check crf_learn.log for more info.\n"
									"Call: " + command + "\n";
		throw WcrftError(message);
	}
}

void write_example_to_file(boost::shared_ptr<std::ofstream> file,
						   std::vector<std::string> feat_vals,
						   std::string class_label)
{
	(*file) << boost::algorithm::join(feat_vals, details::DATA_SEP)
			<< details::DATA_SEP << class_label << std::endl;
}

}
