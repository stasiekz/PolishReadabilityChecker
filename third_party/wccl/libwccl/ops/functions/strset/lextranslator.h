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

#ifndef LIBWCCL_OPS_FUNCTIONS_STRSET_LEXTRANSLATOR_H
#define LIBWCCL_OPS_FUNCTIONS_STRSET_LEXTRANSLATOR_H

#include <libwccl/values/strset.h>
#include <libwccl/ops/function.h>
#include <libwccl/lexicon/lexicon.h>

namespace Wccl {

/**
 * WCCL functional operator "lex", translates strings via a lexicon.
 */ 
class LexTranslator : public Function<StrSet>
{
public:
	LexTranslator(
		const boost::shared_ptr<Function<StrSet> >& strset_expr,
		const boost::shared_ptr<const Lexicon>& lexicon)
		: strset_expr_(strset_expr),
		  lexicon_(lexicon)
	{
		BOOST_ASSERT(strset_expr_);
		BOOST_ASSERT(lexicon_);
	}

	/**
	 * @returns String representation of the function in the form of
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * @returns Name of the function
	 */
	std::string raw_name() const {
		return "lex";
	}
protected:
	std::ostream& write_to(std::ostream& os) const;

	/**
	 * Get a string set from the argument expression and return a copy,
	 * translated using the lexicon given at construction. Strings that don't
	 * exist in the lexicon are removed from output, whereas strings that
	 * exist as keys are replaced with corresponding values.
	 */
	BaseRetValPtr apply_internal(const FunExecContext& context) const;

private:
	const boost::shared_ptr<Function<StrSet> > strset_expr_;
	const boost::shared_ptr<const Lexicon> lexicon_;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNCTIONS_STRSET_LEXTRANSLATOR_H
