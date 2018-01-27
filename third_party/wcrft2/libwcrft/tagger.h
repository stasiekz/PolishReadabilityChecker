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

#ifndef WCRFT_TAGGER_H
#define WCRFT_TAGGER_H

#include <iostream>
#include <utility>
#include <set>

#include <boost/shared_ptr.hpp>
#include <crfpp.h>

#include <libcorpus2/tagset.h>
#include <libcorpus2/io/reader.h>
#include <libcorpus2/io/writer.h>

#include "config.h"
#include "classify.h"
#include "layers.h"

namespace Wcrft {

/// Class for displaying tagger statistics.
class Statistics {
public:
	Statistics();

	/// Clear all fields (set to 0)
	void clear();

	/**
	 * @brief Prints whole data to the @c output_stream (or @c std::cerr if not given).
	 *
	 * Shows
	 * @li number of tokens processed
	 * @li number of sentences processed
	 * @li number of classifier calls
	 * @li number of layers gets/fails
	 */
	void dump(std::ostream& output_stream = std::cerr);

	/** @brief Reports number of processed tokens and sentences.
	 *
	 * If second parameter is not 0 report is shown 
	 * every @c sents sentences.
	 */
	void report(std::ostream& output_stream = std::cerr, int sents = 100);

	std::map<std::string, int> layer_gets, layer_fails;
	int num_tokens, num_sentences, num_evals;
};

/**
 * @brief The WCRFT main class to be instantiated when using the tagger.
 *
 * The actual implementation of the WCRFT tagger. The tagger is created using
 * a given config file and optionally model dir (default model location may
 * be inferred from config).
 * May be used on different levels of abstraction:
 * * tag whole file or stream and write output to file or stream,
 * * tag singular sentence or paragaph and directly access tagged output using
 *   Corpus2 data structures (this way you can consume parts of tagger output
 *   as soon as they are produced and don't need to store whole tagged material
 *   before using it).
 * Tagger object also allows for tagger training and saving of trained model
 * to model_dir.
 */
class Tagger {
private:
	typedef boost::shared_ptr<Corpus2::TokenReader> TokenReaderPtr;
	typedef boost::shared_ptr<Corpus2::TokenWriter> TokenWriterPtr;

	typedef boost::shared_ptr<Corpus2::Chunk> ChunkPtr;
	typedef boost::shared_ptr<Corpus2::Sentence> SentencePtr;

public:
	/**
	 * @brief Creates WCRFT Tagger instance using given config name
	 * (optionally model dir).
	 *
	 * WCRFT is created using a particular configuration that specifies
	 * its parameters (e.g. tagset, layer definition, unknown word handling
	 * strategy) and points to files that define features for classification
	 * (written in WCCL language) and txt file that describe how WCCL
	 * features will be collapsed to CRF++ features
	 *
	 * NOTE: before tagging any input you need to call load_model().
	 *
	 * @param config_name Configuration name (with or without ".ini")
	 *        or full path to congiguration file.
	 * @param model_dir Optional. Path to tagger model or empty string if model location
	 *        is to be taken from the configuration (default).
	 */
	Tagger(const std::string& config_name, const std::string& model_dir = "");
	~Tagger();

	/**
	 * Load trained model, must be called once before tagging.
	 */
	void load_model();

	/**
	 * @brief Train the tagger and store trained model inside given model dir.
	 *
	 * Perform full tagger training using the given training corpus. The exact
	 * course of action will depend on the config used. To obtain optimal
	 * results, consider running morphological reanalysis of the training data
	 * before training proper (consult documentation).
	 *
	 * @param input_file Training corpus; should contain the interpretations
	 *        selected as golden-standard (marked as 'disamb') but also the
	 * 	other 'possible choices' as selected by morphological analyser.
	 * @param input_format Format of the training data.
	 */
	void train_and_save(const std::string& input_file, const std::string& input_format);

	/**
	 * @brief Tag whole input file and write results to output file.
	 *
	 * Reads input file paragraph-by-paragraph (or sentence-by-sentence if
	 * told so in the config) and outputs tagged paragraphs (sentences) to
	 * the output file. Since the input is processed gradually, may be used
	 * to tag large files without risk of running out of memory.
	 *
	 * @param input_file path to input file.
	 * @param input_format Corpus2-compliant input format name, possibly with
	 *        additional options after comma.
	 * @param output_file path to output file.
	 * @param output_format Corpus2-compliant output format name, possibly with
	 *        additional options after comma.
	 */
	void tag_input(const std::string& input_file, const std::string& input_format,
				   const std::string& output_file, const std::string& output_format);

	/**
	 * @brief Tag the whole input from input stream and write tagged output to output stream.
	 *
	 * Reads input paragraph-by-paragraph (or sentence-by-sentence if told so
	 * in the config) and writes each tagged paragraph (sentence) to the output
	 * stream. Since the input is processed gradually, may be used to tag large
	 * texts without running out of memory.
	 *
	 * @param input_stream
	 * @param input_format
	 * @param output_stream
	 * @param output_format
	 */
	void tag_input(std::istream& input_stream, const std::string& input_format,
				   std::ostream& output_stream, const std::string& output_format);

	/**
	 * @brief Tag single sentence.
	 *
	 * @param sentence The sentence to be tagged, will be modified in-place.
	 * @param preserve_ambiguity Switch on keep all original interpretations;
	 *        use with caution, may limit the potential of tag guessing module.
	 * @param guess_unknown Turns on/off the tag guessing module.
	 */
	void tag_sentence(SentencePtr sentence,
					  bool preserve_ambiguity = false,
					  bool guess_unknown = true);

	/**
	 * @brief Tag single paragraph (all of its sentences).
	 *
	 * @param paragraph Paragraph to be tagged; its sentences will be modified
	 * in-place.
	 * @param preserve_ambiguity Switch on keep all original interpretations;
	 *        use with caution, may limit the potential of tag guessing module.
	 * @param guess_unknown Turns on/off the tag guessing module.
	 */
	void tag_paragraph(ChunkPtr paragraph,
					   bool preserve_ambiguity = false,
					   bool guess_unknown = true);

	/**
	 * Switch tagger verbose mode (dumping diagnostic information to stderr).
	 */
	 void switch_verbose(bool verbose);

	/**
	 * Switch ambiguity mode: if switched on, will keep all 'possible'
	 * interpretations (tags and lemmas from morphological analyser)
	 * at output while leaving those selected marked as 'disamb'.
	 * This mode is switched off by default and its use is discouraged
	 * as it may limit the potential of tag guessing module (if the guesser
	 * arrives at an interpretation that is known by the analyser, it may be
	 * discarded).
	 */
	void switch_preserve_ambiguity(bool ambiguity);

	/**
	 * Switch on/off paragraph-by-paragraph processing, default: on.
	 * When switched off, the tagger will ignore paragraph boundaries and
	 * process each sentence in isolation. Ignoring paragraphs may be
	 * desired if input file is already morphologically analysed but contains
	 * no division into paragraphs or contains large paragraphs that could
	 * cause memory problems (in "paragraphs" mode one paragraph must be
	 * loaded into memory before proceeding; when this mode is off, only one
	 * sentence must be loaded; when dealing with typical input, it doesn't
	 * make significant difference hence there is no need to ignore paragraphs.
	 */
	void switch_paragraph_processing(bool paragraphs);



	/**
	 * @brief Set or override any tagger configuration option.
	 * @param option_name Option name; if option belongs to a config section,
	 *        use notation section.option.
	 * @param value Option value to be set.
	 */
	template<typename T>
	inline void set_configuration_option(const std::string& option_name, T value)
	{
		tagger_conf_.set_config_option<T>(option_name, value);
	}

	/**
	 * Get tagset that this tagger instance operates on.
	 */
	const Corpus2::Tagset &get_tagset() const
	{
		return tagset_;
	}

	/**
	 * Override Maca (morphological analyser) configuration to be used.
	 * @param maca_cfg name of the configuration without .ini suffix
	 */
	void set_maca_config(const std::string &maca_cfg);

	/**
	 * Override WCCL configuration to be used.
	 * @param wccl_cfg name of the configuration without .ccl suffix
	 */
	void set_wccl_config(const std::string &wccl_cfg);

	/**
	 * Get name of Maca (morphological analyser) configuration this tagger
	 * instance uses.
	 * @return Maca configuration name (config filename without .ini)
	 */
	const std::string get_maca_config() const;

private:
	void tag_input_inner(TokenReaderPtr reader, TokenWriterPtr writer);

	typedef std::map<std::string, int> UnkTagsCount;
	/**
	 * Reads in_path token by token, gathers most frequent forms and stores it
	 * in the form of a lexicon readable by WCCL expressions.
	 * Also gathers a list of most frequent tags of "unknown" words if
	 * unknown frequency threshold is above zero.
	 */
	void gather_unknown_tags(const std::string& input_path, const std::string& input_format);
	UnkTagsCount gather_sentence_unknown_tags(SentencePtr sentence);
	/**
	 * @brief Filters unknown tags, so only tags that appeared
	 * at least @c min_freq times are included.
	 */
	std::vector<std::string> filter_unknown_tags(UnkTagsCount& entries, int min_freq);
	void save_unknown_tags(std::vector<std::string> unknown_tags, const std::string& filename);

	/**
	 * @brief Read training data and generate training examples for each
	 * layer.
	 *
	 * Reads training corpus and simulates normal tagger disambiguation in
	 * order to generate training data. The process is done for each layer:
	 * first, features are generated for each sentence token with respect to
	 * first layer, then the attribute corresponding to the layer is
	 * disambiguated (i.e. tags that assume incorrect value of the attribute
	 * are removed from tokens), then the process moves on to next layer
	 * and so on. The process results in training examples written to files
	 * stored within model directory, one file per layer.
	 *
	 * @param input_path path to training corpus containing manually selected
	 * tags+lemmas ("disamb") as well as items that are not selected but
	 * possible for a word form (typically, other possibilities resulting from
	 * morphological analysis).
	 * @param input_format format of the training data.
	 * @return
	 */
	std::set<std::string> write_training_examples(const std::string& input_path,
												  const std::string& input_format);
	std::set<std::string> write_sentence_training_examples(SentencePtr sentence,
														   TrainingFiles training_files);
	bool write_sentence_layer_examples(SentencePtr sentence, const LayerPtr layer,
									   boost::shared_ptr<std::ofstream> tfile);
	void train_classifiers(std::set<std::string> attributes_met);

	/**
	 * Read unknown tag list from model directory and popullate @c unknown_tags_.
	 */
	void load_unknown_tags();

	/**
	 * Preprocesses words recognised as "unknown" to also contain tags from
	 * memorised "uknown word tag" list. This is to make the tagger able
	 * to guess the tags for unknown words even if the analyser has failed.
	 * "Unknown" words are first identified -- those are the tokens having
	 * the unknown word tag and possibly other tags. Then tags from our list
	 * are added to such tokens. It is assumed here that the training corpus
	 * contains reference morphological analysis for "known" words, while words
	 * not present in the morphological dictionary are assigned just the
	 * correct tag (marked as disamb) as well as non-disamb "unknown word" tag.
	 * @param sentence sentence that will be modified in-place
	 */
	void preprocess_unknown_tokens(SentencePtr sentence);

	/**
	 * @brief Tag simple sentence and leave only selected tags.
	 * @param sentence sentence to be tagged in-place.
	 */
	void disambiguate_sentence(SentencePtr sentence);

	void feed_tagger(CRFTaggerPtr crf_tagger, SentencePtr sentence, const LayerPtr layer);
	void classify_sentence(CRFTaggerPtr crf_tagger, SentencePtr sentence, const LayerPtr layer);
	void select_preferred_tags(SentencePtr sentence);

	/// Return if token is assigned unknown-word tag (possibly among others).
	bool is_unknown_token(Corpus2::Token *token);

	/// Return if tagger is operating in verbose mode.
	bool is_verbose();

	/// Return if tagger is respecting paragraph boundaries.
	bool is_processing_paragraphs();

	/// Return if tagger is forced to keep morpho analysis interpretations.
	bool is_preserving_ambiguity();

	/// Return if tag guessing for unknown words is turned on in the config.
	bool is_guessing_unknown();

	WcrftConfig tagger_conf_;

	typedef std::map<std::string, CRFTaggerPtr> Model;


	Corpus2::Tagset tagset_;
	std::vector<Corpus2::Tag> unknown_tags_;

	boost::shared_ptr<Layers> layers_;
	Model model_;

	Statistics stats_;
};

}

#endif // WCRFT_TAGGER_H
