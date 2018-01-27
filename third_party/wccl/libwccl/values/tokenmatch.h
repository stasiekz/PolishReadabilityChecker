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

#ifndef LIBWCCL_VALUES_TOKENMATCH_H
#define LIBWCCL_VALUES_TOKENMATCH_H

#include <libwccl/values/matchdata.h>

namespace Wccl {

class TokenMatch : public MatchData
{
public:

	explicit TokenMatch(int abs_pos)
		: abs_pos_(abs_pos)
	{
		BOOST_ASSERT(abs_pos_ >= 0);
	}

	/// MatchData override.
	bool empty() const {
		return false;
	}

	/// MatchData override.
	int first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>&) const {
		return abs_pos_;
	}

	/// MatchData override.
	int last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>&) const {
		return abs_pos_;
	}

	/// MatchData override
	std::string to_raw_string() const;

protected:
	/// MatchData override
	TokenMatch* clone_internal() const {
		return new TokenMatch(*this);
	}

private:
	int abs_pos_;
};

} /* end ns Wccl */

#endif // LIBWCCL_VALUES_TOKENMATCH_H
