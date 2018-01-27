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

#ifndef LIBWCCL_VALUES_ANNOTATIONMATCH_H
#define LIBWCCL_VALUES_ANNOTATIONMATCH_H

#include <libwccl/values/matchdata.h>

namespace Wccl {

class AnnotationMatch : public MatchData
{
public:

	explicit AnnotationMatch(int abs_pos, const std::string& channel)
		: abs_pos_(abs_pos), channel_(channel)
	{
		BOOST_ASSERT(abs_pos_ >= 0);
	}

	/// MatchData override.
	bool empty() const {
		return false;
	}

	/// MatchData override.
	int first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const;

	/// MatchData override.
	int last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const;

	/// MatchData override
	std::string to_raw_string() const;

protected:
	/// MatchData override
	AnnotationMatch* clone_internal() const {
		return new AnnotationMatch(*this);
	}

private:
	int abs_pos_;
	std::string channel_;
};

} /* end ns Wccl */

#endif // LIBWCCL_VALUES_ANNOTATIONMATCH_H
