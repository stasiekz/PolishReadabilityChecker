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

#ifndef LIBMACA_UTIL_SENTENCEANALYSER_H
#define LIBMACA_UTIL_SENTENCEANALYSER_H

#include <libtoki/tokenizer/tokenizer.h>
#include <libtoki/sentencesplitter.h>
#include <libmaca/morph/morphanalyser.h>
#include <libmaca/util/confignode.h>

namespace Maca {

class SentenceAnalyser : public Toki::UnicodeSink
{
public:
	SentenceAnalyser(const boost::shared_ptr<Toki::Tokenizer>& tok,
		const boost::shared_ptr<MorphAnalyser>& ma);

	SentenceAnalyser(const Config::Node& cfg);

	SentenceAnalyser(const Config::Node& cfg,
			const Toki::Config::Node& toki_config_override);

	static boost::shared_ptr<SentenceAnalyser> create_from_named_config(
			const std::string& config_name);

	static boost::shared_ptr<SentenceAnalyser> create_from_named_config(
			const std::string& config_name,
			const std::string& toki_config_override);

	static std::string available_configurations();

	Corpus2::Sentence::Ptr get_next_sentence();

	typedef boost::function<void (const Corpus2::Sentence::Ptr&)> sentence_sink_t;

	/**
	 * Convenience function to process all sentences that can be gathered
	 * and feed them one by one to the sink function.
	 * @return true if the sink was called at least once, false otherwise
	 *         (meaning there were no sentences)
	 */
	bool process(sentence_sink_t sink);

	const Toki::Tokenizer& tokenizer() const {
		return *tok_;
	}

	Toki::Tokenizer& tokenizer() {
		return *tok_;
	}

	const MorphAnalyser& analyser() const {
		return *ma_;
	}

	MorphAnalyser& analyser() {
		return *ma_;
	}

	const Corpus2::Tagset& tagset() const {
		return ma_->tagset();
	}

protected:
	void new_input_source();

private:
	boost::shared_ptr<Toki::Tokenizer> tok_;

	Toki::SentenceSplitter sp_;

	boost::shared_ptr<MorphAnalyser> ma_;
};

} /* end ns Maca */

#endif // LIBMACA_UTIL_SENTENCEANALYSER_H
