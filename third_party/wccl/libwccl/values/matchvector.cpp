/*
    Copyright (C) 2011 Adam Wardyński, Tomasz Śniatowski, Paweł Kędzia,
    Adam Radziszewski, Bartosz Broda
    Part of the WCCL project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <libwccl/values/matchvector.h>
#include <libwccl/values/match.h>
#include <boost/foreach.hpp>
#include <sstream>
#include <libwccl/exception.h>

namespace Wccl {

std::string MatchVector::to_raw_string() const
{
	std::stringstream ss;
	ss << "MATCH(";
	bool comma = false;
	BOOST_FOREACH (const boost::shared_ptr<Match>& m, matches_) {
		if (comma) {
			ss << ",";
		}
		ss << m->to_raw_string();
		comma = true;
	}
	ss << ")";
	return ss.str();
}

int MatchVector::first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const
{
	if (matches_.empty()) {
		return Position::Nowhere;
	} else {
		// Negative positions are invalid, including specials like Nowhere,
		// so we can't just find minimum value but minimum *non-negative* value.
		// Note: yes, the code assumes the special values like Nowhere are indeed negative.
		int p = matches_.front()->first_token(s);
		size_t i = 1;
		while ((p < 0) && (i < matches_.size())) {
			p = matches_[i]->first_token(s);
			++i;
		}
		while (i < matches_.size()) {
			int c = matches_[i]->first_token(s);
			if ((c >= 0) && (c < p)) {
				p = c;
			}
			++i;
		}
		return p >= 0 ? p : Position::Nowhere;
	}
}

int MatchVector::last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const
{
	if (matches_.empty()) {
		return Position::Nowhere;
	} else {
		int p = matches_.front()->last_token(s);
		for (size_t i = 1; i < matches_.size(); ++i) {
			int c = matches_[i]->last_token(s);
			if (c > p) {
				p = c;
			}
		}
		return p >= 0 ? p : Position::Nowhere;
	}
}

bool MatchVector::empty() const
{
	BOOST_FOREACH (const boost::shared_ptr<Match>& m, matches_) {
		if (!m->empty()) {
			return false;
		}
	}
	return true;
}

void MatchVector::append(const boost::shared_ptr<Match> &m)
{
	matches_.push_back(m);
}

void MatchVector::append(const boost::shared_ptr<MatchVector> &m)
{
	matches_.push_back(boost::shared_ptr<Match>(new Match(m)));
}

void MatchVector::append(const boost::shared_ptr<TokenMatch> &m)
{
	matches_.push_back(boost::shared_ptr<Match>(new Match(m)));
}

void MatchVector::append(const boost::shared_ptr<AnnotationMatch> &m)
{
	matches_.push_back(boost::shared_ptr<Match>(new Match(m)));
}

void MatchVector::append(const boost::shared_ptr<MatchData> &m)
{
	matches_.push_back(boost::shared_ptr<Match>(new Match(m)));
}

const boost::shared_ptr<const Match> MatchVector::submatch(size_t idx) const {
	if ((idx < matches_.size() + 1) || (idx == 0)) {
		return matches_[idx - 1];
	} else {
		throw Wccl::WcclError("Match vector index out of range.");
	}
}

const boost::shared_ptr<Match>& MatchVector::submatch(size_t idx)
{
	if ((idx < matches_.size() + 1) || (idx == 0)) {
		return matches_[idx - 1];
	} else {
		throw Wccl::WcclError("Match vector index out of range.");
	}
}

} /* end ns Wccl */
