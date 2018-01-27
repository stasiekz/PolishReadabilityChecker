/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENCE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef PWRNLP_SENTENCE_H
#define PWRNLP_SENTENCE_H

#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <vector>

namespace PwrNlp {

	/**
	 * A simple sentence wrapper template, a template not a plain class since
	 * it is intended for use with various types of tokens.
	 *
	 * A sentence owns its Tokens.
	 */
	template<typename TT>
	class SentenceTemplate : private boost::noncopyable
	{
	public:
		/// Empty constructor
		SentenceTemplate()
			: tokens_()
		{
		}

		/// Range constructor
		template <typename T>
		explicit SentenceTemplate(const T& range)
			: tokens_()
		{
			std::copy(range.begin(), range.end(), std::back_inserter(tokens_));
		}

		SentenceTemplate* clone() const;

		/// Destructor
		~SentenceTemplate()
		{
			BOOST_FOREACH(TT* t, tokens_) {
				delete t;
			}
		}

		bool empty() const {
			return tokens_.empty();
		}

		/// Size accessor
		size_t size() const {
			return tokens_.size();
		}

		/// Token accessor
		TT* operator[](size_t idx) {
			return tokens_[idx];
		}

		/// Token accessor, const
		const TT* operator[](size_t idx) const {
			return tokens_[idx];
		}

		/// Underlying vector accessor, const
		const std::vector<TT*>& tokens() const {
			return tokens_;
		}

		/// Underlying vector accessor
		std::vector<TT*>& tokens() {
			return tokens_;
		}

		/// Helper function for appending tokens
		void append(TT* t) {
			tokens_.push_back(t);
		}

		/// convenience first token accessor
		const TT* first_token() const {
			assert(!tokens_.empty());
			return tokens_[0];
		}

	private:
		/// The tokens this sentence contains and owns
		std::vector<TT*> tokens_;
	};

	template<class TT>
	SentenceTemplate<TT>* SentenceTemplate<TT>::clone() const
	{
		SentenceTemplate<TT>* s = new SentenceTemplate<TT>;
		BOOST_FOREACH(const TT* t, tokens_) {
			s->append(t->clone());
		}
		return s;
	}

} /* end ns PwrNlp */

#endif // PWRNLP_SENTENCE_H
