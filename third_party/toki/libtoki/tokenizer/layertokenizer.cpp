/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <libtoki/exception.h>
#include <libtoki/token.h>
#include <libtoki/layers/tokenlayer.h>
#include <libtoki/tokenizer/layertokenizer.h>
#include <libtoki/tokenizer/whitespacetokenizer.h>
#include <boost/foreach.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#include <unicode/uchar.h>
#include <unicode/ucnv.h>

#include <fstream>
#include <iostream>
#include <sstream>

namespace Toki {

	LayerTokenizer::LayerTokenizer(const Config::Node &cfg)
		: Tokenizer(cfg)
		, input_tokenizer_(new WhitespaceTokenizer(
				cfg.get_child("input", Config::Node())))
		, error_stream_(Toki::get_default_error_stream())
	{
		apply_configuration(cfg);
		new_input_source();
	}
	
	boost::shared_ptr<LayerTokenizer> LayerTokenizer::create_from_named_config(
			const std::string& config_name)
	{
		const Config::Node& cfg = config_name.empty() ?
				default_config() : get_named_config(config_name);
		return boost::make_shared<LayerTokenizer>(cfg);
	}

	LayerTokenizer::LayerTokenizer(UnicodeSource *input,
			const Config::Node &cfg)
		: Tokenizer(input, cfg)
		, input_tokenizer_(
				new WhitespaceTokenizer(
						cfg.get_child("input", Config::Node())))
		, error_stream_(Toki::get_default_error_stream())
	{
		apply_configuration(cfg);
		new_input_source();
	}

	LayerTokenizer::LayerTokenizer(
			const boost::shared_ptr<UnicodeSource>& input,
			const Config::Node &cfg)
		: Tokenizer(input, cfg)
		, input_tokenizer_(
				new WhitespaceTokenizer(
						cfg.get_child("input", Config::Node())))
		, error_stream_(Toki::get_default_error_stream())
	{
		apply_configuration(cfg);
		new_input_source();
	}

	LayerTokenizer::LayerTokenizer(std::istream &is, const Config::Node &cfg)
		: Tokenizer(is, cfg)
		, input_tokenizer_(
				new WhitespaceTokenizer(
						cfg.get_child("input", Config::Node())))
		, error_stream_(Toki::get_default_error_stream())
	{
		apply_configuration(cfg);
		new_input_source();
	}

	LayerTokenizer::LayerTokenizer(const UnicodeString &s,
			const Config::Node &cfg)
		: Tokenizer(s, cfg)
		, input_tokenizer_(
				new WhitespaceTokenizer(
						cfg.get_child("input", Config::Node())))
		, error_stream_(Toki::get_default_error_stream())
	{
		apply_configuration(cfg);
		new_input_source();
	}

	LayerTokenizer::~LayerTokenizer()
	{
		for (size_t i = 0; i < layers_.size(); ++i) {
			delete layers_[i];
		}
	}

	std::string LayerTokenizer::layers_info() const
	{
		std::stringstream ss;
		ss << "LTok:" << layers_.size() << ": ";
		for (size_t i = 0; i < layers_.size(); ++i) {
			ss << layers_[i]->info() << " ";
		}
		if (input_tokenizer_->has_srx()) {
			ss << "[SRX]";
		}
		return ss.str();
	}

	std::string LayerTokenizer::layers_long_info(const std::string &sep) const
	{
		std::stringstream ss;
		for (size_t i = 0; i < layers_.size(); ++i) {
			ss << layers_[i]->long_info() << sep;
		}
		return ss.str();
	}

	void LayerTokenizer::new_input_source()
	{
		input_tokenizer_->set_input_source(get_input_source());
	}


	void LayerTokenizer::apply_configuration(const Config::Node &cfg)
	{
		using boost::property_tree::ptree;

		const ptree* ltp = NULL;
		try {
			ltp = &cfg.get_child("layers");
		} catch (boost::property_tree::ptree_error& e) {
			throw TokiError("No layers in LayerTokenizer config");
		}
		const ptree& layers_tree = *ltp;

		std::vector<std::string> layer_ids;

		BOOST_FOREACH (const Config::Node::value_type &v, layers_tree) {
			if (v.first == "layer") {
				layer_ids.push_back(v.second.data());
			}
		}

		TokenSource* previous = input_tokenizer_.get();
		BOOST_FOREACH (const std::string& id, layer_ids) {
			try {
				std::string layer_class;
				layer_class = cfg.get("layer:" + id + ".class", "");
				TokenLayer* layer;
				try {
					Config::Node layer_cfg = cfg.get_child("layer:" + id);
					layer_cfg.put("id", id);
					layer = TokenLayer::create(layer_class, previous,
							layer_cfg);
					previous = layer;
					layers_.push_back(layer);
				} catch (TokenLayerFactoryException) {
					if (error_stream_) {
						(*error_stream_) << "Bad layer class ID "
							<< layer_class << ". "
							<< "Layer with id " << id << " ignored.\n";
					}
				}
			} catch (boost::property_tree::ptree_error& e) {
				if (error_stream_) {
					(*error_stream_) << "Error while processing configuration "
						<< "for layer with id " << id << ". "
						<< e.what() << "\n";
				}
			}
		}

		if (layers_.empty()) {
			std::cerr << "No valid layers in layer tokenizer!\n";
			throw TokiError("No valid layers in LayerTokenizer");
		}
	}

	Token* LayerTokenizer::get_next_token()
	{
		return layers_.back()->get_next_token();
	}

	void LayerTokenizer::restart()
	{
		for (size_t i = 0; i < layers_.size(); ++i) {
			layers_[i]->restart();
		}
		input_tokenizer_->restart();
	}

	void LayerTokenizer::set_error_stream(std::ostream *os)
	{
		error_stream_ = os;
		for (size_t i = 0; i < layers_.size(); ++i) {
			layers_[i]->set_error_stream(os);
		}
	}

} /* end namespace Toki */
