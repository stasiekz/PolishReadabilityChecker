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

    See the LICENSE.CORPUS2, LICENSE.POLIQARP, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBCORPUS2_SENTENCE_H
#define LIBCORPUS2_SENTENCE_H

#include <libcorpus2/token.h>
#include <libpwrutils/sentence.h>
#include <boost/shared_ptr.hpp>

namespace Corpus2 {

/**
 * A Sentence is the primary structure passed around in Corpus2,
 * tokens are rarely processed outside of a Sentence.
 *
 * Sentences should always be kept as shared_ptrs and passed around
 * as shared pointers or const references. Derived classes of
 * Sentence may provide extra functionality.
 *
 * Tokens are kept as raw pointers. A Sentence owns the tokens 
 * it contains and will delete them upon destruction. Sometimes 
 * this is not desirable, in this case the release_tokens function 
 * can be used (carefully).
 */
class Sentence : private boost::noncopyable
{
public:
	/// Convenience typedef for a shared pointer to a Sentence
	typedef boost::shared_ptr<Sentence> Ptr;

	/// Convenience typedef for a shared pointer to a const Sentence
	typedef boost::shared_ptr<const Sentence> ConstPtr;

	/// Constructor with default (empty) sentence identifier. 
	/// Creates a Sentence with no tokens.
	Sentence(const std::string &id = "");

	/// Sentence cloning. All the tokens are duplicated.
	virtual Ptr clone_shared() const;

	/// Destructor. Deletes contained tokens.
	virtual ~Sentence();

	/// Releases ownership of all tokens, makes the sentence empty.
	/// Warning: This will cause an obvious memory leak if the tokens are
	/// not stored somewhere first (e.g. append()ed to another Sentence).
	void release_tokens();

	/// Convenience function to check if the sentence is empty
	bool empty() const {
		return tokens_.empty();
	}

	/// Size accessor
	size_t size() const {
		return tokens_.size();
	}

	/// Token accessor (no range checking)
	Token* operator[](size_t idx) {
		return tokens_[idx];
	}

	/// Token accessor, const (no range checking)
	const Token* operator[](size_t idx) const {
		return tokens_[idx];
	}

	/// Token accessor, non-operator (no range checking)
	Token* at(size_t idx) {
		return tokens_[idx];
	}

	/// Token accessor, const (no range checking)
	const Token* at(size_t idx) const {
		return tokens_[idx];
	}

	/// Underlying vector accessor, const
	const std::vector<Token*>& tokens() const {
		return tokens_;
	}

	/// Underlying vector accessor, nonconst (dangerous)
	std::vector<Token*>& tokens() {
		return tokens_;
	}

	/// Helper function for appending tokens. Prefer using this.
	/// Might be overriden in a child class to make adding a token keep
	/// extra invariants
	virtual void append(Token* t) {
		tokens_.push_back(t);
	}

	/// convenience first token accessor
	const Token* first_token() const {
		assert(!tokens_.empty());
		return tokens_[0];
	}

	/// Identifier getter
	const std::string id() const {
		return id_;
	}

	/// Identifier setter
	void set_id(const std::string &id) {
		id_ = id;
	}

protected:
	/// The tokens this sentence contains and owns
	std::vector<Token*> tokens_;

	/// The sentence identifer - default is empty
	std::string id_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_SENTENCE_H
