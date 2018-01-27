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
 * @file classify.h
 * @brief File containing global scope CRFPP interface functions.
 */

#ifndef WCRFT_CLASSIFY_H
#define WCRFT_CLASSIFY_H

#include <fstream>

#include <boost/shared_ptr.hpp>
#include <crfpp.h>

#include "config.h"
#include "layers.h"

namespace Wcrft {

typedef boost::shared_ptr<CRFPP::Tagger> CRFTaggerPtr;

/**
 * @brief Loads trained classifier (CRFPP tagger) for given attribute.
 *
 * Loads trained classifier (CRFPP tagger) for given attribute from model directory stored 
 * in @c tagger_conf. File with classifier data should be named @a attribute_name @c .cr.
 *
 * @param attribute_name Attribute for which the tagger should be loaded.
 * @param tagger_conf
 * @return Loaded CRF tagger. If no trained model file for given attribute was
 * found, NULL is returned.
 */
CRFTaggerPtr load_classifier(std::string attribute_name, const WcrftConfig& tagger_conf);

/// @brief Prepares CRF tagger (classifier) for eating a sentence-long list of feature vectors
void classifier_open_sentence(CRFTaggerPtr crf_tagger);

/// @brief Tells the classifier that feeding is over.
void classifier_close_sentence(CRFTaggerPtr crf_tagger);

/// @brief Feeds CRF tagger (classifier) with feature values for token, allowing it to classify it.
void eat_token(CRFTaggerPtr crf_tagger, std::vector<std::string> featured_vals);

/// @brief Returns CRF tagger (classifier) decision. You need to feed classifier with sentence first.
std::string classify_token(CRFTaggerPtr crf_tagger, int token_id);

typedef std::map<std::string, boost::shared_ptr<std::ofstream> > TrainingFiles;
/// @brief Opens training file for every attribute (layer).
TrainingFiles open_training_files(WcrftConfig& config, boost::shared_ptr<Layers> layers);

/// @brief Closes training files given in its first parameter.
void close_training_files(TrainingFiles& training_files);

/// @brief Train a CRF classifier for the given attr_name. The trained model is saved to files.
void train_classifier_and_save(const std::string attribute_name, const WcrftConfig& config);

/// @brief Writes single example in simple tab-separated format to training file.
void write_example_to_file(boost::shared_ptr<std::ofstream> file,
						   std::vector<std::string> feat_vals,
						   std::string class_label);

}

#endif // WCRFT_CLASSIFY_H
