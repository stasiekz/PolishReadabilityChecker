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

#ifndef LIBWCCL_OPS_TAGRULESEQUENCE_H
#define LIBWCCL_OPS_TAGRULESEQUENCE_H

#include <libwccl/ops/tagrule.h>

namespace Wccl {

/**
 * Represents a sequence of parsed WCCL tag rules. It's a conveniency wrapper around
 * vector of TagRule objects, that allows automatic execution of all contained TagRules
 * for all positions of a Sentence.
 * @note The class methods are not thread-safe
 */
class TagRuleSequence : public std::vector<TagRule>, public Expression
{
public:
	TagRuleSequence(const std::vector<TagRule>& rules);

	TagRuleSequence();

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
	Bool operator()(const boost::shared_ptr<Corpus2::Sentence>& sentence);

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
	Bool execute_once(const boost::shared_ptr<Corpus2::Sentence>& sentence);

	/**
	 * Executes all contained Rules sequentially, for each position
	 * starting from 0 to given sentence's end. Repeats until there are no changes,
	 * or specified number of iterations have been executed.
	 * @returns Number of full iterations that have been executed until there were no changes,
	 * or until max_iter has been reached while changes were still reported.
	 * @param sentence Sentence to execute on.
	 * @param max_iter Maximum number of iterations (mostly to safeguard from an infinite loop)
	 * @see \link operator()() operator() \endlink - Executes Rules but only once
	 * @see execute_once() - Executes Rules but only once
	 */
	int execute_until_done(const boost::shared_ptr<Corpus2::Sentence>& sentence, int max_iter = 1000);

	std::string to_string(const Corpus2::Tagset& tagset) const;
protected:
	std::ostream& write_to(std::ostream& os) const;
};



//
//--- implementation details ---
//
inline
TagRuleSequence::TagRuleSequence(const std::vector<TagRule>& rules)
	: std::vector<TagRule>(rules) {
}

inline
TagRuleSequence::TagRuleSequence()
	: std::vector<TagRule>() {
}

inline
Bool TagRuleSequence::operator()(const boost::shared_ptr<Corpus2::Sentence>& sentence) {
	return execute_once(sentence);
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_TAGRULESEQUENCE_H
