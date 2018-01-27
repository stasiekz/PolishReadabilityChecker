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

#include <algorithm>
#include <fstream>
#include <map>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <libcorpus2/tagging.h>
#include <libcorpus2/tagsetmanager.h>

#include "config.h"
#include "tagger.h"
#include "corpusio.h"

namespace Wcrft {

Tagger::Tagger(const std::string& config_name, const std::string& model_dir)
	: tagger_conf_(config_name, model_dir)
{
	std::string tagset_name =
			tagger_conf_.get_config_section_option<std::string>(
				CONFIG_S_GLOBAL, CONFIG_O_TAGSET);
	this->tagset_ = Corpus2::get_named_tagset(tagset_name);
}

Tagger::~Tagger()
{
}

void Tagger::load_model()
{
	this->layers_ = boost::make_shared<Layers>(tagger_conf_, tagset_);
	BOOST_FOREACH(const LayerPtr layer, layers_->get_layers()) {
		const std::string attr_name = layer->get_attribute_name();
		this->model_[attr_name] = load_classifier(attr_name, tagger_conf_);
	}

	bool unk_guess = is_guessing_unknown();
	if(unk_guess)
		this->load_unknown_tags();
}

void Tagger::train_and_save(const std::string& input_file, const std::string& input_format)
{
	if(input_format.compare(WCRFT_PLAIN_TEXT_FORMAT) == 0 ||
	   input_format.compare(WCRFT_PLAIN_TEXT_FORMAT_ALT) == 0 ||
	   input_format.compare(WCRFT_PREMORPH_TEXT_FORMAT) == 0)
		throw WcrftError("Cannot train tagger using TXT or PREMORPH format.");

	// if guessing turned on, gather a list of tags typical for unknown tokens
	const bool treat_unknown =
			tagger_conf_.get_config_section_option<bool>(
				CONFIG_S_UNKNOWN, CONFIG_O_UNKGUESS);
	if(treat_unknown) {
		// this will also save the tag list to model directory
		this->gather_unknown_tags(input_file, input_format);
	}

	// set-up tagger layers
	this->layers_ = boost::make_shared<Layers>(tagger_conf_, tagset_);

	this->stats_.clear();

	// generate training examples for each layer and save them to files
	std::set<std::string> attr_met = this->write_training_examples(input_file, input_format);
	// run CRF++ training
	this->train_classifiers(attr_met);

	if(this->is_verbose()) {
		std::cerr << "Tagger trained\n";
		this->stats_.dump();
	}
}

void Tagger::tag_input(const std::string& input_file, const std::string& input_format,
					   const std::string& output_file, const std::string& output_format)
{
	const std::string maca_cfg =
			tagger_conf_.get_config_section_option<std::string>(
				CONFIG_S_GLOBAL, CONFIG_O_MACACFG);

	TokenReaderPtr reader = get_reader(
				input_file, input_format, this->tagset_, maca_cfg);
	TokenWriterPtr writer = get_writer(
				output_file, output_format, this->tagset_);

	this->tag_input_inner(reader, writer);
}

void Tagger::tag_input(std::istream& input_stream, const std::string& input_format,
					   std::ostream& output_stream, const std::string& output_format)
{
	const std::string maca_cfg =
			tagger_conf_.get_config_section_option<std::string>(
				CONFIG_S_GLOBAL, CONFIG_O_MACACFG);

	TokenReaderPtr reader = get_reader(
				input_stream, input_format, this->tagset_, maca_cfg);
	TokenWriterPtr writer = get_writer(
				output_stream, output_format, this->tagset_);

	this->tag_input_inner(reader, writer);
}

void Tagger::tag_input_inner(TokenReaderPtr reader, TokenWriterPtr writer)
{
	this->stats_.clear();

	const bool preserve_ambiguity = is_preserving_ambiguity();
	const bool guess_unknown = is_guessing_unknown();
	const bool preserve_paragraphs = is_processing_paragraphs();

	if(preserve_paragraphs)
		// iterate over paragraphs, process each par sentence
		while(true) {
			ChunkPtr paragraph = reader->get_next_chunk();
			if(!paragraph)
				break;
			this->tag_paragraph(paragraph, preserve_ambiguity, guess_unknown);
			writer->write_chunk(*paragraph);
		}
	else
		// iterate over sentences ignoring paragraph boundaries
		// (whether present or not in input)
		while(true) {
			SentencePtr sentence = reader->get_next_sentence();
			if(!sentence)
				break;
			this->tag_sentence(sentence, preserve_ambiguity, guess_unknown);
			writer->write_sentence(*sentence);
		}

	if(is_verbose())
		this->stats_.dump();

	writer->finish();
}

void Tagger::tag_paragraph(ChunkPtr paragraph, bool preserve_ambiguity, bool guess_unknown)
{
	BOOST_FOREACH( SentencePtr sentence_ptr, paragraph->sentences() ) {
		this->tag_sentence(sentence_ptr, preserve_ambiguity, guess_unknown);
	}
}

void Tagger::tag_sentence(SentencePtr sentence, bool preserve_ambiguity, bool guess_unknown)
{
	if(guess_unknown)
		this->preprocess_unknown_tokens(sentence);

	if(preserve_ambiguity) {
		SentencePtr disambd = sentence->clone_shared();
		this->disambiguate_sentence(disambd);
		std::vector<Corpus2::Token*> sentence_tokens = sentence->tokens();
		std::vector<Corpus2::Token*> disambd_tokens = disambd->tokens();
		for(unsigned int i = 0;i < sentence_tokens.size();++i) {
			Corpus2::Token* stoken = sentence_tokens[i];
			Corpus2::Token* dtoken = disambd_tokens[i];
			Corpus2::set_disambs(stoken, (dtoken->get_preferred_lexeme(this->tagset_)).tag());
		}
	} else
		this->disambiguate_sentence(sentence);
}

void Tagger::gather_unknown_tags(const std::string& input_path, const std::string& input_format)
{
	int unk_freq =
			tagger_conf_.get_config_section_option<int>(CONFIG_S_UNKNOWN, CONFIG_O_UNKFREQ);

	UnkTagsCount unktags_entries;

	TokenReaderPtr reader = get_reader(input_path, input_format, this->tagset_);
	while(true) {
		SentencePtr sentence = reader->get_next_sentence();
		if(!sentence)
			break;

		UnkTagsCount sentence_entries = gather_sentence_unknown_tags(sentence);

		BOOST_FOREACH(UnkTagsCount::value_type& unk_tag_entry, sentence_entries) {
			std::string unk_tag = unk_tag_entry.first;
			// no need to check existance, map will put 0 if `unk_tag` doesn't exists
			unktags_entries[unk_tag] += unk_tag_entry.second;
		}
	}

	std::vector<std::string> wanted_unknown = filter_unknown_tags(unktags_entries, unk_freq);
	BOOST_FOREACH(std::string tag_string, wanted_unknown) {
		Corpus2::Tag tag = this->tagset_.parse_simple_tag(tag_string);
		this->unknown_tags_.push_back(tag);
	}

	std::string unktags_filename = tagger_conf_.get_model_filename(CONFIG_EXT_UNKTAGS);
	save_unknown_tags(wanted_unknown, unktags_filename);
}

Tagger::UnkTagsCount Tagger::gather_sentence_unknown_tags(SentencePtr sentence)
{
	UnkTagsCount unktags_entries;
	BOOST_FOREACH(Corpus2::Token* tok, sentence->tokens()) {
		Corpus2::Tag ign_tag = tagset_.make_ign_tag();
		// is the token "unknown"? that is unknown tag appears among its possible tags
		Corpus2::Tag unkmask = Corpus2::mask_token(*tok, ign_tag, false);
		if(!unkmask.get_masked(ign_tag).is_null()) {
			// non-empty intersection with the unknown tag
			// get the preferred disamb tag
			Corpus2::Tag toktag = tok->get_preferred_lexeme(tagset_).tag();
			std::string tagstr = tagset_.tag_to_string(toktag);
			unktags_entries[tagstr] = unktags_entries[tagstr] + 1;
		}
	}

	return unktags_entries;
}

std::vector<std::string> Tagger::filter_unknown_tags(UnkTagsCount& entries, int min_freq)
{
	std::vector<std::string> wanted_unktags;
	BOOST_FOREACH(UnkTagsCount::value_type& entry, entries) {
		if(entry.second >= min_freq)
			wanted_unktags.push_back(entry.first);
	}

	return wanted_unktags;
}

void Tagger::save_unknown_tags(std::vector<std::string> unknown_tags, const std::string& filename)
{
	std::ofstream unktags_file(filename.c_str());
	BOOST_FOREACH(std::string unktag, unknown_tags) {
		unktags_file << unktag << std::endl;
	}
	unktags_file.close();
}

std::set<std::string> Tagger::write_training_examples(const std::string& input_path,
													  const std::string& input_format)
{
	if(this->is_verbose())
		std::cerr << "Generating_training data...\n";

	std::set<std::string> attributes_met;
	TrainingFiles training_files =
			open_training_files(this->tagger_conf_, this->layers_);
	TokenReaderPtr reader =
			get_reader(input_path, input_format, this->tagset_);
	while(true) {
		SentencePtr sentence = reader->get_next_sentence();
		if(!sentence)
			break;

		std::set<std::string> sentence_attrs_met = write_sentence_training_examples(sentence, training_files);
		attributes_met.insert(sentence_attrs_met.begin(), sentence_attrs_met.end());

		this->stats_.num_sentences++;
		this->stats_.num_tokens += sentence->tokens().size();
		if(this->is_verbose())
			this->stats_.report();
	}
	close_training_files(training_files);

	return attributes_met;
}

std::set<std::string> Tagger::write_sentence_training_examples(SentencePtr sentence,
															   TrainingFiles training_files)
{
	std::set<std::string> attributes_met;
	boost::shared_ptr<Wccl::TagRuleSequence> tag_rules =
			this->layers_->get_tag_rules();

	this->preprocess_unknown_tokens(sentence);
	if(tag_rules)
		tag_rules->execute_once(sentence);

	BOOST_FOREACH(const LayerPtr layer, layers_->get_layers()) {
		std::string attribute_name = layer->get_attribute_name();
		boost::shared_ptr<std::ofstream> tfile = training_files[attribute_name];

		bool got_data = write_sentence_layer_examples(sentence, layer, tfile);

		*tfile << std::endl;
		if(got_data)
			attributes_met.insert(attribute_name);
	}

	return attributes_met;
}

bool Tagger::write_sentence_layer_examples(SentencePtr sentence, const LayerPtr layer,
										   boost::shared_ptr<std::ofstream> tfile)
{
	const std::string attribute_name = layer->get_attribute_name();
	const Corpus2::Tag attribute_mask = layer->get_attribute_tag();
	const fun_op_ptr_v attribute_ops = layer->get_attribute_operators();
	bool got_data = false;

	Wccl::SentenceContext context = Wccl::SentenceContext(sentence);
	std::vector<Corpus2::Token*> sent_tokens = sentence->tokens();
	for(unsigned int tok_id = 0;tok_id < sent_tokens.size();++tok_id) {
		Corpus2::Token* tok = sent_tokens[tok_id];
		context.set_position(tok_id);

		Corpus2::Tag all_attr_vals = Corpus2::mask_token(*tok, attribute_mask, false);
		Corpus2::Tag disamb_attr_vals = Corpus2::mask_token(*tok, attribute_mask, true);

		std::vector<std::string> feat_vals;
		BOOST_FOREACH(fun_op_ptr_t op, attribute_ops) {
			std::string val = op->base_apply(context)->to_compact_string(tagset_);
			feat_vals.push_back(val);
		}
		std::string class_label = mask2text(tagset_, disamb_attr_vals);
		write_example_to_file(tfile, feat_vals, class_label);

		stats_.num_evals++;

		if(!disamb_attr_vals.is_null()) {
			got_data = true;
			Corpus2::disambiguate_equal(tok, all_attr_vals, disamb_attr_vals);
		}
	}

	return got_data;
}

void Tagger::train_classifiers(std::set<std::string> attributes_met)
{
	BOOST_FOREACH(const LayerPtr layer, layers_->get_layers()) {
		const std::string attribute_name = layer->get_attribute_name();

		if(this->is_verbose())
			std::cerr << "Training tagger for " << attribute_name << std::endl;

		if(attributes_met.find(attribute_name) != attributes_met.end())
			train_classifier_and_save(attribute_name, this->tagger_conf_);

		if(this->is_verbose())
			std::cerr << "Done\n";
	}
}

void Tagger::load_unknown_tags()
{
	std::string unknown_tags_filename =
			tagger_conf_.get_model_filename(CONFIG_EXT_UNKTAGS);

	std::ifstream unknown_tags_file(unknown_tags_filename.c_str(), std::ifstream::in);
	std::string line;
	while(getline(unknown_tags_file, line)) {
		boost::algorithm::trim(line);
		if(!line.empty()) {
			Corpus2::Tag new_unknown_tag = this->tagset_.parse_simple_tag(line);
			this->unknown_tags_.push_back(new_unknown_tag);
		}
	}
}

void Tagger::preprocess_unknown_tokens(SentencePtr sentence)
{
	const bool lower_lemmas =
			tagger_conf_.get_config_section_option(
				CONFIG_S_UNKNOWN, CONFIG_O_UNKLOWER, true);
	BOOST_FOREACH(Corpus2::Token *token, sentence->tokens()) {
		if(this->is_unknown_token(token)) {
			UnicodeString lemma = token->orth();
			if(lower_lemmas) {
				lemma.toLower();
			}
			Corpus2::Tag disamb = token->get_preferred_lexeme(this->tagset_).tag();

			BOOST_FOREACH(Corpus2::Tag unknown_tag, this->unknown_tags_) {
				if(unknown_tag != disamb)
					token->add_lexeme(Corpus2::Lexeme(lemma, unknown_tag));
			}

			token->remove_duplicate_lexemes();
		}
	}
}

bool Tagger::is_unknown_token(Corpus2::Token *token)
{
	Corpus2::Tag unknown_mask = this->tagset_.make_ign_tag();
	Corpus2::Tag token_unknown_mask = Corpus2::mask_token(*token, unknown_mask, false);

	return token_unknown_mask == unknown_mask;
}

void Tagger::disambiguate_sentence(SentencePtr sentence)
{
	boost::shared_ptr<Wccl::TagRuleSequence> tag_rules =
			layers_->get_tag_rules();
	if(tag_rules && !(tag_rules->empty()))
		tag_rules->execute_once(sentence);

	BOOST_FOREACH(const LayerPtr layer, layers_->get_layers()) {
		CRFTaggerPtr crf_tagger = this->model_[layer->get_attribute_name()];
		if(crf_tagger) {
			this->feed_tagger(crf_tagger, sentence, layer);
			this->classify_sentence(crf_tagger, sentence, layer);
		}
	}

	this->select_preferred_tags(sentence);

	this->stats_.num_sentences++;
	this->stats_.num_tokens += sentence->tokens().size();
	if(is_verbose())
		this->stats_.report();
}

void Tagger::feed_tagger(CRFTaggerPtr crf_tagger, SentencePtr sentence, const LayerPtr layer)
{
	// prepare the classifier to eat a sentence-long list of feature vectors
	Wccl::SentenceContext context = Wccl::SentenceContext(sentence);
	classifier_open_sentence(crf_tagger);

	for(unsigned int tok_id = 0; tok_id < sentence->size(); ++tok_id) {
		context.set_position(tok_id);
		// get feature values of token at tok_id and its context
		std::vector<fun_op_ptr_t> operators = layer->get_attribute_operators();
		std::vector<std::string> featured_vals;
		for(unsigned int j = 0;j < operators.size();++j) {
			fun_op_ptr_t op = operators[j];

			if(op) {
				std::string val = op->base_apply(context)->to_compact_string(this->tagset_);
				featured_vals.push_back(val);
			}
		}
		eat_token(crf_tagger, featured_vals);
	}

	// tell the classifier the feeding is over
	classifier_close_sentence(crf_tagger);
}

void Tagger::classify_sentence(CRFTaggerPtr crf_tagger, SentencePtr sentence, const LayerPtr layer)
{
	// feeding with feature vectors is over and we'll be asking about decisions
	// concerning each fed token
	for(unsigned int tok_id = 0;tok_id < sentence->tokens().size();++tok_id) {
		std::string class_label = classify_token(crf_tagger, tok_id);

		bool success = false;

		Corpus2::Token *token = sentence->tokens()[tok_id];
		// this mask represents all possible values of the attribute that
		// is represented by this layer
		Corpus2::Tag all_attribute_vals = Corpus2::mask_token(
					*token, layer->get_attribute_tag(), false);
		// this mask represents the value of the attribute that is predicted by
		// the classifier; if the value encodes all possible values of the
		// attribute, it means that no value should be given for it
		Corpus2::Tag wanted_attribute_vals = text2mask(this->tagset_, class_label);

		if(!wanted_attribute_vals.is_null()) {
			// will succeed if an interpretation with wanted_attribute_vals
			// exists -- such interpretation will be selected
			success = Corpus2::disambiguate_equal(
						token, all_attribute_vals, wanted_attribute_vals);
			this->stats_.num_evals++;
			this->stats_.layer_gets[layer->get_attribute_name()]++;
			if(not success)
				this->stats_.layer_fails[layer->get_attribute_name()]++;
		}
	}
}

void Tagger::switch_verbose(bool verbose)
{
	tagger_conf_.set_config_section_option<bool>(
				CONFIG_S_USER, CONFIG_O_VERBOSE, verbose);
}

void Tagger::switch_preserve_ambiguity(bool ambiguity)
{
	tagger_conf_.set_config_section_option<bool>(
				CONFIG_S_USER, CONFIG_O_AMBOUT, ambiguity);
}

void Tagger::switch_paragraph_processing(bool paragraphs)
{
	tagger_conf_.set_config_section_option<bool>(
				CONFIG_S_USER, CONFIG_O_PARA, paragraphs);
}

void Tagger::select_preferred_tags(SentencePtr sentence)
{
	for(unsigned int i = 0;i < sentence->tokens().size();++i) {
		Corpus2::Token *token = sentence->tokens()[i];
		Corpus2::select_singular_tags(this->tagset_, token);
		Corpus2::select_preferred_tag(this->tagset_, token);
	}
}

const std::string Tagger::get_maca_config() const
{
	return tagger_conf_.get_config_section_option<std::string>(
				CONFIG_S_GLOBAL, CONFIG_O_MACACFG);
}

void Tagger::set_maca_config(const std::string &maca_cfg)
{
	tagger_conf_.set_config_section_option<std::string>(
				CONFIG_S_GLOBAL, CONFIG_O_MACACFG, maca_cfg);
}

void Tagger::set_wccl_config(const std::string &wccl_cfg)
{
	tagger_conf_.set_config_section_option<std::string>(
				CONFIG_S_GLOBAL, CONFIG_O_WCCLCFG, wccl_cfg);
}

bool Tagger::is_verbose()
{
	return tagger_conf_.get_config_section_option<bool>(
				CONFIG_S_USER, CONFIG_O_VERBOSE, false);
}

bool Tagger::is_processing_paragraphs()
{
	return tagger_conf_.get_config_section_option<bool>(
				CONFIG_S_USER, CONFIG_O_PARA, true);
}

bool Tagger::is_preserving_ambiguity()
{
	return tagger_conf_.get_config_section_option<bool>(
				CONFIG_S_USER, CONFIG_O_AMBOUT, false);
}

bool Tagger::is_guessing_unknown()
{
	return tagger_conf_.get_config_section_option<bool>(
					CONFIG_S_UNKNOWN, CONFIG_O_UNKGUESS, false);
}

/*** STATISTICS ***/

Statistics::Statistics()
{
	this->clear();
}

void Statistics::clear()
{
	layer_gets.clear();
	layer_fails.clear();

	this->num_tokens = 0;
	this->num_sentences = 0;
	this->num_evals = 0;
}

void Statistics::dump(std::ostream& output_stream)
{
	output_stream << "Tokens processed: " << this->num_tokens << std::endl;
	output_stream << "Sentences processed: " << this->num_sentences << std::endl;
	output_stream << "Classifier calls: " << this->num_evals << std::endl;
	output_stream << "Layers gets/fails:" << std::endl;
	for(std::map<std::string, int>::iterator it = layer_gets.begin();
			it != layer_gets.end();++it) {
		std::string attribute_name = it->first;
		int gets = it->second;
		int fails = (layer_fails.find(attribute_name) != layer_fails.end()) ?
					 layer_fails.find(attribute_name)->second : 0;
		if(gets > 0) {
			output_stream << attribute_name << " "
						  << gets << "/" << fails << std::endl;
		}
	}
}

void Statistics::report(std::ostream& output_stream, int sents)
{
	if(sents == 0 || (sents > 0 && this->num_sentences % sents == 0))
		output_stream << this->num_tokens << " tokens, "
					  << this->num_sentences << " sentences..." << std::endl;
}
} /* namespace Wcrft */
