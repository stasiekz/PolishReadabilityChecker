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

#ifndef LIBWCCL_VALUES_MATCHDATA_H
#define LIBWCCL_VALUES_MATCHDATA_H

#include <libwccl/exception.h>
#include <libcorpus2/ann/annotatedsentence.h>

namespace Wccl {

class Match;
/**
 * Base abstract class for data held by a Match Value
 * - VectorMatch, TokenMatch or AnnotationMatch.
 * (empty VectorMatch should be default option)
 */
class MatchData
{
public:

	/**
	 * Check if the match is empty (matches nothing). Match objects themselves
	 * are by definition empty, child classes are sometimes or always non-empty.
	 */
	virtual bool empty() const = 0;
	/**
	 * Getter for the first token matched. If the match is empty, must return
	 * Position::Nowhere.
	 */
	virtual int first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>&) const = 0;

	/**
	 * Getter for the last token matched. If the match is empty, must return
	 * Position::Nowhere.
	 */
	virtual int last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>&) const = 0;

	/**
	 * Getter for a submatch at given index (indexing starts from 1).
	 */
	virtual const boost::shared_ptr<const Match> submatch(size_t) const {
		throw WcclError("Getting a submatch is possible only for a MatchVector.");
	}

	/**
	 * Getter for a submatch at given index (indexing starts from 1).
	 */
	virtual const boost::shared_ptr<Match>& submatch(size_t) {
		throw WcclError("Getting a submatch is possible only for a MatchVector.");
	}

	boost::shared_ptr<MatchData> clone() const {
		return boost::shared_ptr<MatchData>(clone_internal());
	}

	virtual std::string to_raw_string() const = 0;

	virtual ~MatchData() {}

protected:
	virtual MatchData* clone_internal() const = 0;
};

}
#endif // LIBWCCL_VALUES_MATCHDATA_H
