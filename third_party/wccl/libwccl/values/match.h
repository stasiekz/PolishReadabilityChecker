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

#ifndef LIBWCCL_VALUES_MATCH_H
#define LIBWCCL_VALUES_MATCH_H

#include <libwccl/values/position.h>
#include <libcorpus2/ann/annotatedsentence.h>
#include <libwccl/values/matchvector.h>
#include <libwccl/values/annotationmatch.h>
#include <libwccl/values/tokenmatch.h>

namespace Wccl {

class Match : public Value
{
public:
	WCCL_VALUE_PREAMBLE

	typedef MatchData value_type;

	/**
	 * The default data held is an empty MatchVector
	 */
	Match()
		: match_(new MatchVector())
	{
	}

	Match(const boost::shared_ptr<MatchData>& data)
		: match_(data)
	{
	}

	Match(const boost::shared_ptr<TokenMatch>& data)
		: match_(data)
	{
	}

	Match(const boost::shared_ptr<AnnotationMatch>& data)
		: match_(data)
	{
	}

	Match(const boost::shared_ptr<MatchVector>& data)
		: match_(data)
	{
	}

	Match(const MatchData& data)
		: match_(data.clone())
	{
	}

	Match(const Match& match)
		: match_(match.match_->clone())
	{
	}

	Match& operator=(const Match& match) {
		match_ = match.match_->clone();
		return *this;
	}

	const MatchData& get_value() const {
		return *match_;
	}

	MatchData& get_value() {
		return *match_;
	}

	void set_value(const MatchData& m) {
		match_ = m.clone();
	}

	/**
	 * Check if the match is empty (matches nothing). Match objects themselves
	 * are by definition empty, child classes are sometimes or always non-empty.
	 */
	bool empty() const {
		return match_->empty();
	}

	/**
	 * Getter for the first token matched. If the match is empty, must return
	 * Position::Nowhere.
	 */
	int first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const {
		return match_->first_token(s);
	}

	/**
	 * Getter for the last token matched. If the match is empty, must return
	 * Nowhere.
	 */
	int last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const {
		return match_->last_token(s);
	}

	/// Value override
	std::string to_raw_string() const {
		return match_->to_raw_string();
	}

private:
	boost::shared_ptr<MatchData> match_;
};

} /* end ns Wccl */

#endif // LIBWCCL_VALUES_MATCH_H
