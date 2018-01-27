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

#ifndef LIBWCCL_VALUES_MATCHVECTOR_H
#define LIBWCCL_VALUES_MATCHVECTOR_H

#include <libwccl/values/matchdata.h>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace Wccl {

class Match;
class MatchData;
class MatchVector;
class TokenMatch;
class AnnotationMatch;

class MatchVector : public MatchData
{
public:

	MatchVector()
	{
	}

	/// MatchData override. A MatchVector is empty if it contains no sub-matches,
	/// or if they are all empty.
	bool empty() const;

	/// MatchData override.
	int first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const;

	/// MatchData override.
	int last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const;

	/// MatchData override
	std::string to_raw_string() const;

	/// Append a sub-match
	void append(const boost::shared_ptr<Match>& m);
	void append(const boost::shared_ptr<MatchData>& m);
	void append(const boost::shared_ptr<MatchVector>& m);
	void append(const boost::shared_ptr<TokenMatch>& m);
	void append(const boost::shared_ptr<AnnotationMatch>& m);

	/// Size (number of direct sub-matches)
	size_t size() const {
		return matches_.size();
	}

	/**
	 * Submatch accessor with bounds checking, throws if out of bounds.
	 * @note Indexing is assumed from 1.
	 */
	const boost::shared_ptr<Match>& submatch(size_t idx);

	/**
	 * Submatch accessor with bounds checking, throws if out of bounds.
	 * Const version.
	 * @note Indexing is assumed from 1.
	 */
	const boost::shared_ptr<const Match> submatch(size_t idx) const;

	void clear() {
		matches_.clear();
	}

protected:
	MatchVector* clone_internal() const {
		return new MatchVector(*this);
	}

private:
	std::vector< boost::shared_ptr<Match> > matches_;
};

} /* end ns Wccl */

#endif // LIBWCCL_VALUES_MATCHVECTOR_H
