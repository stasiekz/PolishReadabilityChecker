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

#ifndef LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_REGEX_H
#define LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_REGEX_H

#include <unicode/regex.h>

#include <libwccl/exception.h>
#include <libwccl/values/strset.h>
#include <libwccl/ops/functions/bool/predicate.h>

namespace Wccl {

/**
 * Operator that applies a regular expression to a set of
 * strings and returns true if all of the strings match the
 * expression
 */
class Regex : public Predicate {
public:
	typedef boost::shared_ptr<Function<StrSet> > StrSetFunctionPtr;

	Regex(const StrSetFunctionPtr& strset_expr, const UnicodeString& patstr);

	/**
	 * @returns String representation of the function in form of:
	 * "regex(strset_expr_string, regex_string)"
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function: "regex"
	 */
	std::string raw_name() const {
		return "regex";
	}

protected:	
	/**
	 * Get a string set from the argument expression,
	 * apply regular expression to each string one by one,
	 * return false if a string not matching expression is found.
	 * Return true if all strings matched the regular espression.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

	/**
	 * Writes raw string representation of the function in form of:
	 * "regex(strset_expr_raw_string, regex_string)"
	 * @note This version doesn't require a tagset, but may be incomplete
	 * and/or contain internal info.
	 * @returns Stream written to.
	 */
	std::ostream& write_to(std::ostream& os) const;

private:
	const StrSetFunctionPtr strset_expr_;
	const UnicodeString patstr_;
	const boost::shared_ptr<const RegexPattern> pattern_;
};

class RegexParseError : public WcclError
{
public:
	RegexParseError(
		const UnicodeString& pattern,
		const UErrorCode& status_code,
		const UParseError& parse_error);

	~RegexParseError() throw();
	
	std::string info() const;

	UnicodeString pattern;
	std::string status;
	UParseError upe;
};

} /* end ns Wccl */


#endif // LIBWCCL_OPS_FUNCTIONS_BOOL_PREDICATES_REGEX_H
