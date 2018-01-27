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

#ifndef LIBWCCL_OPS_MATCH_MATCHRESULT_H
#define LIBWCCL_OPS_MATCH_MATCHRESULT_H

#include <libwccl/values/bool.h>
#include <libwccl/values/match.h>

namespace Wccl {
namespace Matching {

/**
 * Class that represents a result of MatchCondition
 */
class MatchResult
{
public:
	MatchResult(const boost::shared_ptr<Match>& match);
	MatchResult(const boost::shared_ptr<MatchData>& match);
	MatchResult(const boost::shared_ptr<MatchVector>& match);
	MatchResult(const boost::shared_ptr<TokenMatch>& match);
	MatchResult(const boost::shared_ptr<AnnotationMatch>& match);
	MatchResult();
	boost::shared_ptr<Match> get_match() const;
	bool matched() const;
private:
	bool _matched;
	boost::shared_ptr<Match> _match;

};


// --- Implementation details ---

inline
MatchResult::MatchResult() : _matched(false), _match() {
}

inline
MatchResult::MatchResult(const boost::shared_ptr<Match>& match)
	: _matched(true),
	  _match(match) {
}

inline
MatchResult::MatchResult(const boost::shared_ptr<MatchData>& match)
	: _matched(true),
	  _match(new Match(match)) {
}

inline
MatchResult::MatchResult(const boost::shared_ptr<TokenMatch>& match)
	: _matched(true),
	  _match(new Match(match)) {
}

inline
MatchResult::MatchResult(const boost::shared_ptr<MatchVector>& match)
	: _matched(true),
	  _match(new Match(match)) {
}

inline
MatchResult::MatchResult(const boost::shared_ptr<AnnotationMatch>& match)
	: _matched(true),
	  _match(new Match(match)) {
}

inline
bool MatchResult::matched() const {
	return _matched;
}

inline
boost::shared_ptr<Match> MatchResult::get_match() const {
	return _match;
}

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_MATCHRESULT_H
