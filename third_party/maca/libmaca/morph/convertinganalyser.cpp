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

#include <libmaca/morph/convertinganalyser.h>
#include <libmaca/util/settings.h>
#include <fstream>

namespace Maca {

const char* ConvertingAnalyser::identifier = "wrap_convert";

bool ConvertingAnalyser::registered =
		MorphAnalyser::register_analyser<ConvertingAnalyser>();

ConvertingAnalyser::ConvertingAnalyser(MorphAnalyser *ma,
		Conversion::TagsetConverter *conv)
	: MorphAnalyser(&conv->tagset_to()), wrapped_(ma), converter_(conv)
{
	Corpus2::require_matching_tagsets(converter_->tagset_from(), *wrapped_,
			"Converting analyser creation (from)");
	Corpus2::require_matching_tagsets(converter_->tagset_to(), *this,
			"Converting analyser creation (to)");
}

ConvertingAnalyser::ConvertingAnalyser(const Config::Node &cfg)
	: MorphAnalyser(cfg), wrapped_(NULL), converter_(NULL)
{
	std::string wrapped_id = cfg.get("wrapped_class", "");
	Config::Node cfg2 = cfg;
	cfg2.put("tagset", cfg.get("wrapped_tagset", ""));
	try {
		wrapped_.reset(MorphAnalyser::create(wrapped_id, cfg2));
	} catch (MorphAnalyserFactoryException&) {
		if (cfg.get("plugin_autoload", true)) {
			if (MorphAnalyser::load_plugin(wrapped_id, false)) {
				try {
					wrapped_.reset(MorphAnalyser::create(wrapped_id, cfg2));
				} catch (MorphAnalyserFactoryException&) {
					throw MacaError("Unknown analyser type: " + wrapped_id +
							" (plugin found but create failed)");
				}
			} else {
				throw MacaError("Unknown analyser type: " + wrapped_id +
						" (plugin not found)");
			}
		} else {
			throw MacaError("Unknown analyser type: " + wrapped_id);
		}
	}

	std::string fn = cfg.get("wrapped_converter", "");
	std::ifstream ifs;
	Path::Instance().open_stream_or_throw(fn, ifs, "converter");
	Config::Node conv_cfg = Config::from_stream(ifs);
	converter_.reset(new Conversion::TagsetConverter(conv_cfg));

	Corpus2::require_matching_tagsets(converter_->tagset_from(), *wrapped_,
			"Converting analyser creation (from)");
	Corpus2::require_matching_tagsets(converter_->tagset_to(), *this,
			"Converting analyser creation (to)");
}

ConvertingAnalyser* ConvertingAnalyser::clone() const
{
	std::auto_ptr<ConvertingAnalyser> copy;
	copy.reset(new ConvertingAnalyser(wrapped_->clone(), converter_->clone()));
	return copy.release();
}

bool ConvertingAnalyser::process_functional(const Toki::Token &t,
		boost::function<void (Corpus2::Token *)> sink)
{
	std::vector<Corpus2::Token*> tv = wrapped_->process(t);
	if (tv.empty()) return false;
	converter_->convert_simple(tv, sink);
	return true;
}



} /* end ns Maca */
