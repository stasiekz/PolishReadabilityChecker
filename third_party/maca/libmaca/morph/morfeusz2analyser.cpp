/*
    Copyright (C) 2014 Radosław Warzocha, Adam Radziszewski
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

#include <fstream>
#include <boost/thread/locks.hpp>

#include <libpwrutils/util.h>
#include <libmaca/util/settings.h>

#include "morfeusz2analyser.h"

namespace Maca {

// statics
const char* Morfeusz2Analyser::identifier = "morfeusz2";

bool Morfeusz2Analyser::registered =
		MorphAnalyser::register_analyser<Morfeusz2Analyser>();

const morfeusz::Charset Morfeusz2Analyser::charset = morfeusz::UTF8;

// construct, copy, destruct
Morfeusz2Analyser::Morfeusz2Analyser(const Corpus2::Tagset* tagset,
								Conversion::TagsetConverter* conv)
: MorphAnalyser(tagset), conv_(conv), warn_on_fold_failure_(false)
{
	require_matching_tagsets(conv_->tagset_to(), *tagset,
							"Morfeusz analyser creation");

	morfeusz_instance = morfeusz::Morfeusz::createInstance(morfeusz::ANALYSE_ONLY);
	morfeusz_instance->setCharset(charset);
}

Morfeusz2Analyser::Morfeusz2Analyser(const Config::Node& cfg)
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

	morfeusz_instance = morfeusz::Morfeusz::createInstance(morfeusz::ANALYSE_ONLY);
	morfeusz_instance->setCharset(charset);
}

Morfeusz2Analyser* Morfeusz2Analyser::clone() const
{
	Morfeusz2Analyser* copy = new Morfeusz2Analyser(&tagset(), conv_->clone());
	copy->ign_tag_ = ign_tag_;
	copy->warn_on_ign_ = warn_on_ign_;
	copy->warn_on_fold_failure_ = warn_on_fold_failure_;
	return copy;
}

Morfeusz2Analyser::~Morfeusz2Analyser()
{
	delete morfeusz_instance;
	delete conv_;
}

// public methods
bool Morfeusz2Analyser::process_functional(const Toki::Token &t,
					boost::function<void(Corpus2::Token *)> sink)
{
	using namespace morfeusz;
	using namespace boost;

	std::string s = PwrNlp::to_utf8(t.orth());
	std::vector<details::Morfeusz2Edge> pmorf;

	// locks are here because Morfeusz2 is not thread-safe
	// those are boost locks
	lock_guard<mutex> *morfeusz_lock = new lock_guard<mutex>(morfeusz_mutex);
	ResultsIterator *res_iter = morfeusz_instance->analyse(s.c_str());
	delete morfeusz_lock;

	while(res_iter->hasNext())
		pmorf.push_back(details::Morfeusz2Edge(res_iter->next(), morfeusz_instance));
	delete res_iter;

	if(pmorf.size() == 1 && pmorf[0].lemma.length() > 0) { // only one analysis
		Corpus2::Token *tok = make_token(t, pmorf[0]);
		std::vector<Corpus2::Token*> vec(1, tok);
		flush_convert(vec, sink);
		return true;
	} else if(pmorf.size() > 1)
		return process_complex_analysis(t, pmorf, sink);
	else
		return false;
}

// private methods
bool Morfeusz2Analyser::process_complex_analysis(const Toki::Token &t,
							std::vector<details::Morfeusz2Edge>& pmorf,
							boost::function<void(Corpus2::Token *)>sink)
{
	adjacency_lists alists = build_adjacency_lists(t, pmorf);
	adj_list &succ = alists.first, &prec = alists.second;

	std::vector<Corpus2::Token*> unambiguous;
	int current_node = 0, node_count = succ.size();
	while (current_node < node_count) {
		if (succ[current_node].size() > 1) { // complex case, segmentation ambiguity
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
						throw Morfeusz2Error("path splits twice",
											t.orth_utf8(), pmorf);
					}
					tse = *succ[v].begin();
					paths.back().push_back(pmorf[tse].token);
					v = pmorf[tse].node_to;
				}
				//assume this is the merge node, check for consistency
				if (merge_node != -1 && merge_node != v) {
					throw Morfeusz2Error("path merge node ambiguity",
										t.orth_utf8(), pmorf);
				}
				merge_node = v;
			}

			flush_convert(paths, sink);
			current_node = merge_node;
		} else if (!succ[current_node].empty()) { //simple case, only one interp
			int edge = *succ[current_node].begin();
			unambiguous.push_back(pmorf[edge].token);
			if (pmorf[edge].node_to != current_node + 1)
				throw Morfeusz2Error("simple path has non-consecutive nodes",
									t.orth_utf8(), pmorf);
			++current_node;
		} else { //only the last node should have no successors
			if (current_node != node_count - 1)
				throw Morfeusz2Error("node without successors is not the last node",
									t.orth_utf8(), pmorf);
			++current_node;
		}
	}

	if (!unambiguous.empty()) {
		flush_convert(unambiguous, sink);
	}
	return true;
}

Morfeusz2Analyser::adjacency_lists
		Morfeusz2Analyser::build_adjacency_lists(const Toki::Token &t,
							std::vector<details::Morfeusz2Edge>& pmorf)
{
	int node_count = 0;
	BOOST_FOREACH(const details::Morfeusz2Edge& mri, pmorf) {
		node_count = std::max(node_count, mri.node_to);
	}
	++node_count; // the numbering starts at 0 and we got the last valid node number

	std::vector< std::vector< int > > succ(node_count), prec(node_count);

	for (unsigned int i = 0; i < pmorf.size(); ++i) {
		details::Morfeusz2Edge& edge = pmorf[i];
		int actual_edge_i = -1;

		BOOST_FOREACH(int out_edge, succ[edge.node_from]) {
			if (pmorf[out_edge].node_to == edge.node_to)
				actual_edge_i = out_edge;
		}
		if (actual_edge_i >= 0) // duplicate edge -- simple lemma ambiguity
			morfeusz_into_token(pmorf[actual_edge_i].token, edge);
		else {
			edge.token = make_token(t, edge);
			succ[edge.node_from].push_back(i);
			prec[edge.node_to].push_back(i);
		}
	}

	return make_pair(succ, prec);
}

void Morfeusz2Analyser::flush_convert(std::vector<Corpus2::Token*>& vec,
							boost::function<void(Corpus2::Token *)> sink)
{
	conv_->convert_simple(vec, sink);
}

void Morfeusz2Analyser::flush_convert(std::vector< std::vector<Corpus2::Token*> >& vec,
										boost::function<void(Corpus2::Token *)> sink)
{
	conv_->convert_ambiguous(vec, sink, warn_on_fold_failure_);
}

Corpus2::Token* Morfeusz2Analyser::make_token(const Toki::Token& t,
						   const details::Morfeusz2Edge& m) const
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

void Morfeusz2Analyser::morfeusz_into_token(Corpus2::Token* tt, const details::Morfeusz2Edge& m) const
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

Morfeusz2Error::Morfeusz2Error(const std::string& error,
		const std::string input,
		const std::vector<details::Morfeusz2Edge>& interp)
	: MacaError("Morfeusz2 error: " + error), error(error), input(input)
	, interp(interp)
{
}

Morfeusz2Error::~Morfeusz2Error() throw()
{
}

std::string Morfeusz2Error::info() const
{
	std::stringstream ss;
	ss << what();
	if (!input.empty()) {
		ss << " for input '" << input << "'";
	}
	return ss.str();
}

namespace details {
	Morfeusz2Edge::Morfeusz2Edge(const morfeusz::MorphInterpretation interp,
								const morfeusz::Morfeusz * morf)
	: node_from(interp.startNode), node_to(interp.endNode)
	, orth(UnicodeString::fromUTF8(interp.orth))
	, lemma(UnicodeString::fromUTF8(interp.lemma))
	, tag_string(morf->getIdResolver().getTag(interp.tagId)), token(NULL)
	{
	}
}

}
