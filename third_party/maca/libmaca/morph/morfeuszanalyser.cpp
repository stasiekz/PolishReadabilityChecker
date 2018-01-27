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

#include <libmaca/morph/morfeuszanalyser.h>
#include <libcorpus2/token.h>
#include <libmaca/util/settings.h>
#include <boost/foreach.hpp>
#include <libpwrutils/util.h>

#include <morfeusz.h>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/mutex.hpp>
#include <memory>

#ifndef __unix__
#define LIBMACA_STATIC_MORFEUSZ
#endif

#ifndef LIBMACA_STATIC_MORFEUSZ
#include <dlfcn.h>
#endif

namespace Maca {

const char* MorfeuszAnalyser::identifier = "morfeusz";

bool MorfeuszAnalyser::registered =
		MorphAnalyser::register_analyser<MorfeuszAnalyser>();

static boost::mutex morfeusz_tagger_mutex;

namespace {
	static const int morfeusz_option_encoding = 1;
	static const int morfeusz_option_encoding_utf8 = 8;
}

MorfeuszEdge::MorfeuszEdge(const MorfeuszData& m)
	: node_from(m.node_from), node_to(m.node_to)
	, orth(UnicodeString::fromUTF8(m.orth))
	, lemma(m.lemma ? UnicodeString::fromUTF8(m.lemma) : UnicodeString())
	, tag_string(m.tag_string ? m.tag_string : std::string()), token(NULL)
{
}

std::vector<MorfeuszEdge> morfeusz_preprocess(MorfeuszData* pmorf)
{
	std::vector<MorfeuszEdge> v;
	int i = 0;
	while (pmorf[i].node_from != -1) {
		v.push_back(MorfeuszEdge(pmorf[i]));
		++i;
	}
	return v;
}

std::vector<MorfeuszEdge> morfeusz_safe_analyse(const UnicodeString& orth)
{
	std::string s = PwrNlp::to_utf8(orth);
	return morfeusz_preprocess(0);
}

MorfeuszInitError::MorfeuszInitError(const std::string& error,
		const std::string& extra, const std::string& name)
	: MacaError("Morfeusz init error: " + error)
	, error(error), extra(extra), name(name)
{
}

MorfeuszInitError::~MorfeuszInitError() throw()
{
}

std::string MorfeuszInitError::info() const
{
	std::stringstream ss;
	ss << what();
	ss << " (" << name << ")";
	if (!extra.empty()) {
		ss << ":\n" << extra << "\n";
	}
	return ss.str();
}

MorfeuszError::MorfeuszError(const std::string& error,
		const std::string input,
		const std::vector<MorfeuszEdge>& interp)
	: MacaError("Morfeusz error: " + error), error(error), input(input)
	, interp(interp)
{
}

MorfeuszError::~MorfeuszError() throw()
{
}

std::string MorfeuszError::info() const
{
	std::stringstream ss;
	ss << what();
	if (!input.empty()) {
		ss << " for input '" << input << "'";
	}
	return ss.str();
}

MorfeuszAnalyser::MorfeuszAnalyser(const Corpus2::Tagset* tagset,
		Conversion::TagsetConverter* conv, const std::string& libname,
		const std::string& require_version)
	: MorphAnalyser(tagset), conv_(conv), warn_on_fold_failure_(false)
	, morfeusz_library_(libname), require_version_(require_version)
{
	require_matching_tagsets(conv_->tagset_to(), *tagset,
		"Morfeusz analyser creation");
	load_morfeusz_library();
}

void MorfeuszAnalyser::load_morfeusz_library()
{
#ifndef LIBMACA_STATIC_MORFEUSZ
	morfeusz_lib_handle_ = dlopen(morfeusz_library_.c_str(), RTLD_NOW | RTLD_NOLOAD);
	if(morfeusz_lib_handle_ == NULL){
		morfeusz_lib_handle_ = dlopen(morfeusz_library_.c_str(), RTLD_NOW | RTLD_LOCAL);
		if (morfeusz_lib_handle_ == NULL) {
			const char* dle = dlerror();
			if (dle != NULL) {
				throw MorfeuszInitError("Error opening library (no handle)",
						dle, morfeusz_library_);
			}
		}
	}

	dlerror();
	morfeusz_analyse_handle_ = reinterpret_cast<morfeusz_func_t>(
			dlsym(morfeusz_lib_handle_, "morfeusz_analyse"));
	const char* dle = dlerror();
	if (dle != NULL) {
		dlclose(morfeusz_lib_handle_);
		throw MorfeuszInitError(
			"Error opening library (no morfeusz_analyse symbol)", dle,
			morfeusz_library_);
	}

	dlerror();
	typedef char*(*about_func_t)();
	about_func_t about_func = reinterpret_cast<about_func_t>(
			dlsym(morfeusz_lib_handle_, "morfeusz_about"));
	if (dle != NULL) {
		dlclose(morfeusz_lib_handle_);
		throw MorfeuszInitError(
			"Error opening library (no morfeusz_about symbol)",
			dle, morfeusz_library_);
	}
	std::string about = about_func();
	if (about.find(require_version_) == about.npos) {
		dlclose(morfeusz_lib_handle_);
		throw MorfeuszInitError("Invalid Morfeusz version. Requested " +
			require_version_ + ", got ", about, morfeusz_library_);
	}

	dlerror();
	typedef int (*opt_func_t)(int, int);
	opt_func_t opt_func = reinterpret_cast<opt_func_t>(
			dlsym(morfeusz_lib_handle_, "morfeusz_set_option"));
	if (dle != NULL) {
		dlclose(morfeusz_lib_handle_);
		throw MorfeuszInitError(
			"Error opening library (no morfeusz_set_option symbol)",
			dle, morfeusz_library_);
	}
	if (opt_func != NULL) {
		opt_func(morfeusz_option_encoding, morfeusz_option_encoding_utf8);
	}
#else
	std::string about = morfeusz_about();
	if (about.find(require_version_) == about.npos) {
		throw MorfeuszInitError("Invalid Morfeusz version. Requested " +
			require_version_ + ", got ", about, morfeusz_library_);
	}
	morfeusz_set_option(morfeusz_option_encoding, morfeusz_option_encoding_utf8);
#endif
}

MorfeuszAnalyser* MorfeuszAnalyser::clone() const
{
	MorfeuszAnalyser* copy = new MorfeuszAnalyser(&tagset(),
			conv_->clone(), morfeusz_library_, require_version_);
	copy->ign_tag_ = ign_tag_;
	copy->warn_on_ign_ = warn_on_ign_;
	copy->warn_on_fold_failure_ = warn_on_fold_failure_;
	return copy;
}

MorfeuszAnalyser::MorfeuszAnalyser(const Config::Node& cfg)
	: MorphAnalyser(cfg), conv_(NULL), ign_tag_(), warn_on_ign_(false)
{
	std::string fn = cfg.get("converter", "");
	std::ifstream ifs;
	Path::Instance().open_stream_or_throw(fn, ifs, "converter");

	Config::Node conv_cfg = Config::from_stream(ifs);
	std::auto_ptr<Conversion::TagsetConverter> c(
			new Conversion::TagsetConverter(conv_cfg));

	require_matching_tagsets(c->tagset_to(), *this,
			"Morfeusz analyser creation");
	conv_ = c.release();

	std::string ign_tag_string = cfg.get("ign_tag", "ign");
	ign_tag_ = conv_->tagset_from().parse_simple_tag(ign_tag_string);
	warn_on_ign_ = cfg.get("warn_on_ign", false);
	warn_on_fold_failure_ =  cfg.get("warn_on_fold_failure", false);

	morfeusz_library_ = cfg.get("library", "libmorfeusz.so");
	require_version_ = cfg.get("require_version", "Morfeusz SIAT");
	load_morfeusz_library();
}

MorfeuszAnalyser::~MorfeuszAnalyser()
{
#ifndef LIBMACA_STATIC_MORFEUSZ
	dlclose(morfeusz_lib_handle_);
#endif
	delete conv_;
}

bool MorfeuszAnalyser::process_functional(const Toki::Token &t,
		boost::function<void(Corpus2::Token *)>sink)
{
	std::string s = PwrNlp::to_utf8(t.orth());
	std::vector<MorfeuszEdge> pmorf;
	{
		boost::mutex::scoped_lock lock(morfeusz_tagger_mutex);
		// Morfeusz demands a nonconst char*
	#ifndef LIBMACA_STATIC_MORFEUSZ
		MorfeuszData *ppmorf = morfeusz_analyse_handle_(
				const_cast<char*>(s.c_str()));
	#else
		MorfeuszData *ppmorf = reinterpret_cast<MorfeuszData*>(morfeusz_analyse(
				const_cast<char*>(s.c_str())));
	#endif
	 	pmorf = morfeusz_preprocess(ppmorf);
	}
	if (pmorf.empty()) { // no analyses
		return false;
	} else if (pmorf.size() == 1) { // only one analysis
		if (pmorf[0].lemma.length() > 0) {
			std::vector<Corpus2::Token*> vec;
			vec.push_back(make_token(t, pmorf[0]));
			flush_convert(vec, sink);
			return true;
		} else {
			return false;
		}
	} else { // token was split, or there are multiple analyses (lemmas)
		return process_complex_analysis(t, pmorf, sink);
	}
}

bool MorfeuszAnalyser::process_complex_analysis(const Toki::Token &t,
		std::vector<MorfeuszEdge>& pmorf,
		boost::function<void(Corpus2::Token *)>sink)
{
	int edge_count = pmorf.size();
	int node_count = 0;
	BOOST_FOREACH(const MorfeuszEdge& mri, pmorf) {
		node_count = std::max(node_count, mri.node_to);
	}
	// the numbering starts at 0 and we got the last valid node number
	++node_count;
	// build adjacency lists, folding lemma ambiguities
	std::vector< std::vector< int > > succ(node_count);
	std::vector< std::vector< int > > prec(node_count);
	for (int i = 0; i < edge_count; ++i) {
		MorfeuszEdge& edge = pmorf[i];
		int actual_edge_i = -1;
		BOOST_FOREACH(int out_edge, succ[edge.node_from]) {
			if (pmorf[out_edge].node_to == edge.node_to) {
				actual_edge_i = out_edge;
			}
		}
		if (actual_edge_i >= 0) {
			// duplicate edge -- simple lemma ambiguity
			morfeusz_into_token(pmorf[actual_edge_i].token, edge);
		} else {
			edge.token = make_token(t, edge);
			succ[edge.node_from].push_back(i);
			prec[edge.node_to].push_back(i);
		}
	}

	std::vector<Corpus2::Token*> unambiguous;
	int current_node = 0;
	while (current_node < node_count) {
		if (succ[current_node].size() > 1) {
			// complex case, segmentation ambiguity
			if (!unambiguous.empty()) {
				flush_convert(unambiguous, sink);
				unambiguous.clear();
			}

			int merge_node = -1;
			std::vector< std::vector< Corpus2::Token* > > paths;
			// follow all paths to the merge point
			BOOST_FOREACH(int tse, succ[current_node]) {
				paths.push_back(std::vector<Corpus2::Token*>());
				paths.back().push_back(pmorf[tse].token);
				int v = pmorf[tse].node_to;
				while (prec[v].size() == 1) {
					if (succ[v].size() != 1) {
						throw MorfeuszError("path splits twice",
								t.orth_utf8(), pmorf);
					}
					tse = *succ[v].begin();
					paths.back().push_back(pmorf[tse].token);
					v = pmorf[tse].node_to;
				}
				//assume this is the merge node, check for consistency
				if (merge_node != -1 && merge_node != v) {
					throw MorfeuszError("path merge node ambiguity",
							t.orth_utf8(), pmorf);
				}
				merge_node = v;
			}

			flush_convert(paths, sink);
			current_node = merge_node;
		} else if (!succ[current_node].empty()) {
			//simple case, only one interp
			int edge = *succ[current_node].begin();
			unambiguous.push_back(pmorf[edge].token);
			if (pmorf[edge].node_to != current_node + 1) {
				throw MorfeuszError(
						"simple path has non-consecutive nodes",
						t.orth_utf8(), pmorf);
			}
			++current_node;
		} else { //only the last node should have no successors
			if (current_node != node_count - 1) {
				throw MorfeuszError(
						"node without successors is not the last node",
						t.orth_utf8(), pmorf);
			}
			++current_node;
		}
	}
	if (!unambiguous.empty()) {
		flush_convert(unambiguous, sink);
	}
	return true;
}

Corpus2::Token* MorfeuszAnalyser::make_token(const Toki::Token& t,
		const MorfeuszEdge &m) const
{
	Corpus2::Token* tt = new Corpus2::Token();
	if (m.node_from == 0) {
		tt->set_wa(t.preceeding_whitespace());
	} else {
		tt->set_wa(PwrNlp::Whitespace::None);
	}
	morfeusz_into_token(tt, m);
	return tt;
}

void MorfeuszAnalyser::morfeusz_into_token(Corpus2::Token *tt,
		const MorfeuszEdge& m) const
{
	tt->set_orth(m.orth);
	if (!m.tag_string.empty()) {
		conv_->tagset_from().lexemes_into_token(*tt, m.lemma,
				m.tag_string);
	} else {
		Corpus2::Lexeme ign_lex(m.orth, ign_tag_);
		tt->add_lexeme(ign_lex);
		if (warn_on_ign_) {
			std::cerr << "Morfeusz: tagging as ign: "
					<< ign_lex.lemma_utf8() << "\n";
		}
	}
}

void MorfeuszAnalyser::flush_convert(std::vector<Corpus2::Token *> &vec,
		boost::function<void(Corpus2::Token *)>sink)
{
	conv_->convert_simple(vec, sink);
}

void MorfeuszAnalyser::flush_convert(
		std::vector<std::vector<Corpus2::Token *> > &vec,
		boost::function<void(Corpus2::Token *)>sink)
{
	conv_->convert_ambiguous(vec, sink, warn_on_fold_failure_);
}

} /* end ns Maca */
