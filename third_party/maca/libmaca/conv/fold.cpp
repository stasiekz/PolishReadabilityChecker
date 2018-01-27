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

#include <libmaca/conv/fold.h>
#include <boost/foreach.hpp>
#include <libcorpus2/token.h>

namespace Maca { namespace Conversion {

size_t find_shortest(const std::vector<std::vector<Corpus2::Token *> >& v,
		size_t& min_len_path)
{
	size_t min_len = v[0].size();
	min_len_path = 0;
	for (size_t pi = 0; pi < v.size(); ++pi) {
		if (v[pi].size() < min_len) {
			min_len = v[pi].size();
			min_len_path = pi;
		}
	}
	return min_len;
}

bool try_fold_paths(const std::vector< std::vector<Corpus2::Token*> >& v,
		boost::function<void (Corpus2::Token*)> sink)
{
	size_t min_len = v[0].size();
	size_t max_len = v[0].size();
	for (size_t pi = 0; pi < v.size(); ++pi) {
		if (v[pi].size() < min_len) {
			min_len = v[pi].size();
		} else if (v[pi].size() > max_len) {
			max_len = v[pi].size();
		}
	}
	// folding is only possible if the paths all have the same length
	if (max_len != min_len) return false;
	for (size_t ti = 0; ti < min_len; ++ti) {
		Corpus2::Token* t = v[0][ti];
		for (size_t pi = 1; pi < v.size(); ++pi) {
			// if orths vary, merging is not possible
			if (t->orth() != v[pi][ti]->orth()) return false;
		}
	}
	// actual merging
	for (size_t ti = 0; ti < min_len; ++ti) {
		Corpus2::Token* t = v[0][ti];
		for (size_t pi = 1; pi < v.size(); ++pi) {
			BOOST_FOREACH(const Corpus2::Lexeme& lex, v[pi][ti]->lexemes()) {
				t->add_lexeme(lex);
			}
			delete v[pi][ti];
		}
		t->remove_duplicate_lexemes();
		sink(t);
	}
	return true;
}

std::vector<Corpus2::Token*> choose_path(
		const std::vector< std::vector<Corpus2::Token*> >& v, size_t n)
{
	assert(n < v.size());
	for (size_t i = 0; i < v.size(); ++i) {
		if (i != n) {
			BOOST_FOREACH(Corpus2::Token* t, v[i]) {
				delete t;
			}
		}
	}
	return v[n];
}

void choose_path(const std::vector< std::vector<Corpus2::Token*> >& v, size_t n,
		boost::function<void (Corpus2::Token*)> sink)
{
	assert(n < v.size());
	for (size_t i = 0; i < v.size(); ++i) {
		if (i != n) {
			BOOST_FOREACH(Corpus2::Token* t, v[i]) {
				delete t;
			}
		}
	}
	BOOST_FOREACH(Corpus2::Token* t, v[n]) {
		sink(t);
	}
}

std::vector<Corpus2::Token*> choose_shortest_path(
		const std::vector< std::vector<Corpus2::Token*> >& v)
{
	size_t min_len_path;
	find_shortest(v, min_len_path);
	return choose_path(v, min_len_path);
}

void choose_shortest_path(const std::vector< std::vector<Corpus2::Token*> >& v,
		boost::function<void (Corpus2::Token*)> sink)
{
	size_t min_len_path;
	find_shortest(v, min_len_path);
	choose_path(v, min_len_path, sink);
}

} /* end ns Conversion */
} /* end ns Maca */
