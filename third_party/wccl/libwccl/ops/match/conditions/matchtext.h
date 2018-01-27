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

#ifndef LIBWCCL_OPS_MATCH_CONDITIONS_MATCHTEXT_H
#define LIBWCCL_OPS_MATCH_CONDITIONS_MATCHTEXT_H

#include <libwccl/ops/match/matchcondition.h>
#include <libwccl/ops/function.h>
#include <unicode/unistr.h>

namespace Wccl {
namespace Matching {
/**
 * text() match condition - matches a text
 */
class MatchText : public MatchCondition
{
public:
	MatchText(const UnicodeString& text)
		: text_(text) {
			BOOST_ASSERT(!text_.isEmpty());
	}
	/**
	 * @returns Name of the Condition.
	 */
	std::string name() const {
		return "text";
	}
	/**
	 * Applies the condition to the given execution context.
	 * If a match is found, the current sentence Position is increased
	 * by the amount of matched tokens.
	 */
	MatchResult apply(const ActionExecContext& context) const;

	/**
	 * @returns String representation of the Condition
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

protected:
	/**
	 * Writes the string representation of the Condition to
	 * an output stream.
	 * @returns Stream written to.
	 * @note May be incomplete and/or containt internal info.
	 */
	std::ostream& write_to(std::ostream& ostream) const;
private:
	const UnicodeString text_;
};

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_CONDITIONS_MATCHTEXT_H
