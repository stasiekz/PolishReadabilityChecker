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

#ifndef LIBWCCL_VALUES_POSITION_H
#define LIBWCCL_VALUES_POSITION_H

#include <libwccl/values/value.h>
#include <cstdlib>
#include <boost/integer_traits.hpp>

namespace Wccl {

class SentenceContext;

/**
 * A Position Value, which is essentially an integer offset with three special
 * values: Begin, End and Nowhere, corresponding to the beginning of the
 * processed sentence, its end and a general "outside" position.
 *
 * Note that means evaluating a position generally makes sense only with
 * the context of some sentence.
 *
 * The default value is Nowhere.
 */
class Position : public Value
{
public:
	WCCL_VALUE_PREAMBLE

	typedef int value_type;

	enum Enum {
		Nowhere = boost::integer_traits<int>::const_min,
		Begin = boost::integer_traits<int>::const_min + 1,
		End = boost::integer_traits<int>::const_max
	};

	explicit Position(int v = Nowhere)
		: val_(v)
	{
	}

	const int& get_value() const {
		return val_;
	}

	void set_value(int v) {
		val_ = v;
	}

	/// Value override
	std::string to_raw_string() const;

	/**
	 * @returns True if underlying position values are equal, false otherwise.
	 * @note This version does not take into account sentence context, only
	 * the raw value of position. In practice it means that the special positions
	 * Begin, End, Nowhere are equal only to a coresponding special position.
	 */
	bool equals(const Position& other) const {
		return val_ == other.val_;
	}

	/**
	 * @returns True if positions are equal in context of a sentence, false otherwise.
	 * @note The equality is determined by absolute value of the Position in context
	 * of a sentence (this means pointing to the same absolute position regardless
	 * if it lies inside or outside of the sentence).
	 * Nowhere is only equal to another Nowhere, but Begin or End may be equal to
	 * a nonspecial position depending on the value of current position in the context.
	 */
	bool equals(const Position& other, const SentenceContext& context) const;

private:
	int val_;
};


} /* end ns Wccl */

#endif // LIBWCCL_POSITION_H
