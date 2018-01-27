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

#ifndef LIBWCCL_VALUES_BOOL_H
#define LIBWCCL_VALUES_BOOL_H

#include <libwccl/values/value.h>

namespace Wccl {

/**
 * A Value subtype which is a thin wrapper around a Bool.
 *
 * The default value of a Bool is false.
 */
class Bool : public Value
{
public:
	WCCL_VALUE_PREAMBLE

	typedef bool value_type;

	explicit Bool(bool v = false)
		: val_(v)
	{
	}

	const bool& get_value() const {
		return val_;
	}

	void set_value(bool v) {
		val_ = v;
	}

	bool equals(const Bool& other) const {
		return val_ == other.val_;
	}

	/// Value override
	std::string to_raw_string() const {
		return val_ ? "True" : "False";
	}

	typedef void (Bool::*safe_bool)() const;
	void safe_bool_helper() const {}

	operator safe_bool() const {
		return val_ ? &Bool::safe_bool_helper : 0;
	}

	bool operator!() const {
		return !val_;
	}

private:
	bool val_;
};

} /* end ns Wccl */

#endif // LIBWCCL_VALUES_BOOL_H
