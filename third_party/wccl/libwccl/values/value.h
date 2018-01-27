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

#ifndef LIBWCCL_VALUES_VALUE_H
#define LIBWCCL_VALUES_VALUE_H

#include <libcorpus2/tagset.h>

#define WCCL_VALUE_PREAMBLE \
static const char* type_name; \
const char* get_type_name() const { return type_name; } \
static std::string var_repr(const std::string &var_name); \
std::string make_var_repr(const std::string &var_name) const { \
	return var_repr(var_name); \
}


namespace Wccl {

/**
 * Abstract base class for WCCL value types
 *
 * Value subtypes should use the WCCL_VALUE_PREAMBLE macro in the beginning
 * of their public: section and define the relevant static data members
 * and functions in the respective cpp file.
 *
 * Value subtypes, but not Value itself, should have a value_type typedef
 * and a const value_type& get_value() const member function defined
 * for interoperability with templates and convenience functions.
 *
 * Values should be default-constructible with a documented default state.
 */
class Value
{
public:
	static const char* type_name;

	virtual const char* get_type_name() const {
		return type_name;
	}

	virtual ~Value() {}

	virtual std::string make_var_repr(const std::string&) const = 0;

	/**
	 * String representation function, in general, a tagset is required,
	 * but some classes might not need that, so by default just forward
	 * to to_raw_string();
	 */
	virtual std::string to_string(const Corpus2::Tagset& /*tagset*/) const {
		return to_raw_string();
	}

	/**
	 * Unicode variant of to_string
	 */
	virtual UnicodeString to_string_u(const Corpus2::Tagset& tagset) const {
		return UnicodeString::fromUTF8(to_string(tagset));
	}

	/**
	 * String representation of the Value that does not require a tagset,
	 * might be incomplete and/or contain internal info.
	 *
	 * Prefer to_string(tagset).
	 */
	virtual std::string to_raw_string() const = 0;

	/**
	 * Unicode variant of to_raw_string
	 */
	virtual UnicodeString to_raw_string_u() const {
		return UnicodeString::fromUTF8(to_raw_string());
	}

	/**
	  * Compact string representation: sets are represented as hyphen-separated
	  * strings (sorted) with no brackets. The representation is suitable for
	  * generating compact output where some degree of ambiguity is allowed
	  * (note that type can't be unambiguously inferred from such strings).
	  */
	virtual std::string to_compact_string(const Corpus2::Tagset& /* tagset */)
			const {
		return to_raw_string();
	}

	virtual UnicodeString to_compact_string_u(const Corpus2::Tagset& tagset)
			const {
		return UnicodeString::fromUTF8(to_compact_string(tagset));
	}

protected:
	Value() {}
};

} /* end ns Wccl */

#endif // LIBWCCL_VALUE_H
