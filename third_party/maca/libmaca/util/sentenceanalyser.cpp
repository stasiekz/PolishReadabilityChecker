/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

#include <libmaca/util/sentenceanalyser.h>
#include <libmaca/util/settings.h>
#include <libtoki/tokenizer/layertokenizer.h>
#include <libmaca/morph/dispatchanalyser.h>
#include <boost/scoped_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>

namespace Maca {

SentenceAnalyser::SentenceAnalyser(
		const boost::shared_ptr<Toki::Tokenizer>& tok,
		const boost::shared_ptr<MorphAnalyser>& ma)
	: UnicodeSink(), tok_(tok), sp_(*tok_), ma_(ma)
{
}

SentenceAnalyser::SentenceAnalyser(const Config::Node &cfg)
	: UnicodeSink()
	, tok_(Toki::LayerTokenizer::create_from_named_config(
		cfg.get("general.toki-config", "")))
	, sp_(*tok_)
	, ma_(new DispatchAnalyser(cfg))
{
}

SentenceAnalyser::SentenceAnalyser(const Config::Node &cfg,
		const Toki::Config::Node &toki_config_override)
	: UnicodeSink()
	, tok_(new Toki::LayerTokenizer(toki_config_override))
	, sp_(*tok_)
	, ma_(new DispatchAnalyser(cfg))
{
}

boost::shared_ptr<SentenceAnalyser>
SentenceAnalyser::create_from_named_config(
		const std::string& config_name)
{
	const Config::Node& cfg = get_named_config(config_name);
	return boost::make_shared<SentenceAnalyser>(cfg);
}

boost::shared_ptr<SentenceAnalyser>
SentenceAnalyser::create_from_named_config(
		const std::string& config_name,
		const std::string& toki_config_override)
{
	const Config::Node& cfg = get_named_config(config_name);
	const Toki::Config::Node& toki_cfg = Toki::get_named_config(
			toki_config_override);
	return boost::make_shared<SentenceAnalyser>(cfg, toki_cfg);
}

std::string SentenceAnalyser::available_configurations()
{
	return boost::algorithm::join(Path::Instance().list_files(".ini"), " ");
}

Corpus2::Sentence::Ptr SentenceAnalyser::get_next_sentence()
{
	if (sp_.has_more()) {
		boost::scoped_ptr<Toki::Sentence> toki_sentence(
				sp_.get_next_sentence());
		assert(toki_sentence);
		assert(!toki_sentence->empty());
		return ma_->process(*toki_sentence);
	} else {
		return Corpus2::Sentence::Ptr();
	}
}

void SentenceAnalyser::new_input_source()
{
	tok_->restart();
	tok_->set_input_source(get_input_source());
}

bool SentenceAnalyser::process(sentence_sink_t sink)
{
	bool had_sentences = false;
	while (Corpus2::Sentence::Ptr s = get_next_sentence()) {
		sink(s);
		had_sentences = true;
	}
	return had_sentences;
}

} /* end ns Maca */
