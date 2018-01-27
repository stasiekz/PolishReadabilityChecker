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

#include <libmaca/morph/morphanalyser.h>
#include <libmaca/util/settings.h>
#include <libcorpus2/tagsetmanager.h>

#include <boost/foreach.hpp>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <libpwrutils/plugin.h>

namespace Maca {

MorphAnalyser::MorphAnalyser(const Corpus2::Tagset *tagset)
	: tagset_(tagset)
{
}

MorphAnalyser::MorphAnalyser(const Config::Node& cfg)
	: tagset_(&Corpus2::get_named_tagset(cfg.get<std::string>("tagset")))
{
}

MorphAnalyser::~MorphAnalyser()
{
}

void MorphAnalyser::process(const Toki::Token &t, std::vector<Corpus2::Token*>& vec)
{
	process_functional(t,
			boost::bind(
				static_cast<void (std::vector<Corpus2::Token*>::*)(Corpus2::Token* const&)>(&std::vector<Corpus2::Token*>::push_back),
				&vec,
				_1));
}

std::vector<Corpus2::Token*> MorphAnalyser::process(const Toki::Token &t)
{
	std::vector<Corpus2::Token*> v;
	process(t, v);
	return v;
}

Corpus2::Sentence::Ptr MorphAnalyser::process(const Toki::Sentence &s)
{
	Corpus2::Sentence::Ptr ss = boost::make_shared<Corpus2::Sentence>();
	BOOST_FOREACH(Toki::Token* t, s.tokens()) {
		process(*t, ss->tokens());
	}
	return ss;
}

std::vector<Corpus2::Token*> MorphAnalyser::process_dispose(
		const std::vector<Toki::Token*>& t)
{
	std::vector<Corpus2::Token*> v;
	process_dispose(t, v);
	return v;
}

void MorphAnalyser::process_dispose(const std::vector<Toki::Token*>& t,
		std::vector<Corpus2::Token*>& v)
{
	BOOST_FOREACH(Toki::Token* tt, t) {
		process(*tt, v);
		delete tt;
	}
}

Corpus2::Sentence::Ptr MorphAnalyser::process_dispose(Toki::Sentence* t)
{
	Corpus2::Sentence::Ptr s = boost::make_shared<Corpus2::Sentence>();
	process_dispose(t, s);
	return s;
}

void MorphAnalyser::process_dispose(Toki::Sentence* t, Corpus2::Sentence::Ptr v)
{
	BOOST_FOREACH(Toki::Token* tt, t->tokens()) {
		process(*tt, v->tokens());
		delete tt;
	}
	t->tokens().clear();
}

Corpus2::Token* create_from_toki(const Toki::Token &t)
{
	Corpus2::Token* tt = new Corpus2::Token(t.orth(), t.preceeding_whitespace());
	return tt;
}

MorphAnalyser* MorphAnalyser::create(std::string class_id,
		const Config::Node& props)
{
	return MorphAnalyserFactory::Instance().CreateObject(class_id, props);
}

std::vector<std::string> MorphAnalyser::available_analyser_types()
{
	return MorphAnalyserFactory::Instance().RegisteredIds();
}

size_t MorphAnalyser::available_analyser_count()
{
	return MorphAnalyserFactory::Instance().RegisteredIds().size();
}

bool MorphAnalyser::load_plugin(const std::string& name, bool quiet)
{
	return PwrNlp::Plugin::load_check("maca", name,
			quiet || !Path::Instance().get_verbose(),
		&MorphAnalyser::available_analyser_count, "morph analyser");
}

} /* end ns Maca */
