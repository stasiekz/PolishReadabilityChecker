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

#ifndef LIBWCCL_OPS_MATCHRULESEQUENCE_H
#define LIBWCCL_OPS_MATCHRULESEQUENCE_H

#include <libwccl/ops/matchrule.h>

namespace Wccl {
namespace Matching {

/**
 * Represents a sequence of parsed WCCL Match rules. It's a conveniency wrapper around
 * vector of MatchRule objects, that allows automatic execution of all contained MatchRules
 * one by one.
 * @note The class methods are not thread-safe
 */
class MatchRuleSequence : public std::vector<MatchRule>, public Expression
{
public:
	MatchRuleSequence(const std::vector<MatchRule>& rules);

	MatchRuleSequence();

	/**
	 * Executes all contained Rules sequentially, once for each position
	 * starting from 0 to given sentence's end.
	 * @returns True if any of the Rules made a change on any of the sentence
	 * positions, False otherwise.
	 * @param sentence Sentence to execute on.
	 * @see execute_once() - equivalent method; the \link operator()() operator() \endlink allows
	 * more convenient functional notation, however if you only have a pointer
	 * you might prefer the execute_once() method as shown below. The choice is yours.
	 * @see execute_until_done() - executes all Rules repeatedly, until there are no changes.
	 * \code
	 * Bool res;
	 * res = ruleseq(sentence);
	 * // versus
         * res = ruleseq.execute_once(sentence);
	 * // or if you have a pointer...
	 * res = (*ruleseq_ptr)(sentence);
	 * // versus
         * res = ruleseq_ptr->execute_once(sentence);
	 * \endcode
	 */
	void operator()(const boost::shared_ptr<Corpus2::AnnotatedSentence>& sentence);

	/**
	 * Executes all contained Rules sequentially, once for each position
	 * starting from 0 to given sentence's end.
	 * @returns True if any of the Rules made a change on any of the sentence's
	 * positions, False otherwise.
	 * @param sentence Sentence to execute on.
	 * @see \link operator()() operator() \endlink - an equivalent of this method that
	 * allows functional notation, treating RuleSeqence directly as a function object
	 * @see execute_until_done() - executes all Rules repeatedly, until there are no changes.
	 */
	void apply_all(const boost::shared_ptr<Corpus2::AnnotatedSentence>& sentence);

	/**
	 * A convenience wrapper that pretends the input is a standard
	 * Corpus::Sentence object and casts it internally as an AnnotatedSentence.
	 * @see \link apply_all \endlink - the same routine without wrapping.
	 */
	void apply_all_sentence_wrapper(
			const boost::shared_ptr<Corpus2::Sentence>& sentence);

	std::string to_string(const Corpus2::Tagset& tagset) const;
protected:
	std::ostream& write_to(std::ostream& os) const;
};



//
//--- implementation details ---
//
inline
MatchRuleSequence::MatchRuleSequence(const std::vector<MatchRule>& rules)
	: std::vector<MatchRule>(rules) {
}

inline
MatchRuleSequence::MatchRuleSequence()
	: std::vector<MatchRule>() {
}

inline
void MatchRuleSequence::operator()(const boost::shared_ptr<Corpus2::AnnotatedSentence>& sentence) {
	apply_all(sentence);
}

} /* end ns Matching */
} /* end ns Wccl */
#endif // LIBWCCL_OPS_MATCHRULESEQUENCE_H
