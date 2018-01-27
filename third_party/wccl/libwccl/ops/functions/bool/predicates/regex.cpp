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

#include <libwccl/ops/functions/bool/predicates/regex.h>
#include <boost/foreach.hpp>

#include <libpwrutils/util.h>

namespace Wccl {

RegexParseError::RegexParseError(
	const UnicodeString& pattern,
	const UErrorCode& status_code,
	const UParseError& parse_error)
	: WcclError("Could not parse regular expression."),
	  pattern(pattern),
	  status(u_errorName(status_code)),
	  upe(parse_error)
{
}

std::string RegexParseError::info() const
{
	std::stringstream ss;
	ss << "Could not parse regular expression at line " << upe.line
		<< " offset " << upe.offset << ". Status: " << status
		<< ". Error: " << PwrNlp::to_utf8(UnicodeString(upe.postContext))
	    << ". Expression was: " << PwrNlp::to_utf8(pattern);
	return ss.str();
}

RegexParseError::~RegexParseError() throw()
{
}

boost::shared_ptr<const RegexPattern> compile_regex(const UnicodeString &pat_str)
{
	UParseError error;
	memset(&error, 0, sizeof(error));
	UErrorCode status;
	memset(&status, 0, sizeof(status));
	boost::shared_ptr<const RegexPattern> pattern(RegexPattern::compile(pat_str, error, status));
	if(status != U_ZERO_ERROR)
	{
		throw RegexParseError(pat_str, status, error);
	}
	return pattern;
}

Regex::Regex(const Regex::StrSetFunctionPtr &strset_expr, const UnicodeString &patstr)
	: strset_expr_(strset_expr),
	  patstr_(patstr),
	  pattern_(compile_regex(patstr))
{
	BOOST_ASSERT(strset_expr_);
	BOOST_ASSERT(pattern_);
}

std::string Regex::to_string(const Corpus2::Tagset& tagset) const
{
	std::stringstream ss;
	ss << name(tagset) << "(" << strset_expr_->to_string(tagset)
		<< ", \"" << PwrNlp::to_utf8(patstr_) << "\")"; //TODO: utf escaping?
	return ss.str();
}

std::ostream& Regex::write_to(std::ostream& os) const
{
	//TODO: utf escaping?
	return os << raw_name() << "(" 
			<< *strset_expr_ << ", \"" << PwrNlp::to_utf8(patstr_) << "\")";
}

Regex::BaseRetValPtr Regex::apply_internal(const FunExecContext& context) const
{
	const boost::shared_ptr<const StrSet>& set = strset_expr_->apply(context);
	if(set->empty()) {
		return Predicate::False(context);
	}
	BOOST_FOREACH (const UnicodeString& s, set->contents()) {
		UErrorCode status = U_ZERO_ERROR;
		boost::scoped_ptr<RegexMatcher> matcher(pattern_->matcher(s, status));
		if(status != U_ZERO_ERROR) {
			BOOST_ASSERT(status == U_ZERO_ERROR);
			return Predicate::False(context);
		}
		bool matched = matcher->matches(status);
		if(status != U_ZERO_ERROR) {
			BOOST_ASSERT(status == U_ZERO_ERROR);
			return Predicate::False(context);
		}
		if(!matched) {
			return Predicate::False(context);
		}
	}
	return Predicate::True(context);
}

} /* end ns Wccl */
