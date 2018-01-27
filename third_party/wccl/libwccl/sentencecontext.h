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

#ifndef LIBWCCL_SENTENCECONTEXT_H
#define LIBWCCL_SENTENCECONTEXT_H

#include <libcorpus2/sentence.h>
#include <libwccl/values/position.h>
#include <boost/shared_ptr.hpp>

namespace Wccl {

/**
 * A wrapper for a Corpus2 Sentence that adds a "current position"
 * and several convenience functions.
 *
 * Copying contexts is cheap and safe since the sentence is kept through
 * a shared pointer, use duplicate() or clone() to get a copy with a separate
 * underlying sentence (which is slower).
 */
class SentenceContext
{
public:
	/// Constructor, wraps the Sentence and sets position to 0
	explicit SentenceContext(const boost::shared_ptr<Corpus2::Sentence>& s);

	/// Returns a copy of this with a cloned underlying sentence.
	SentenceContext duplicate() const;

	/// Cloning -- clones the underlying sentence as well
    /// Note: slower than duplicate() and less useful since you should
    /// prefer to keep SentenceContexts as values, not pointers
	SentenceContext* clone() const;

	/// Underlying sentence accessor
	Corpus2::Sentence& get_sentence() {
		return *sentence_;
	}

	/// Underlying sentence accessor, const
	const Corpus2::Sentence& get_sentence() const {
		return *sentence_;
	}

	/// Underlying sentence shared pointer accesor
	const boost::shared_ptr<Corpus2::Sentence>& get_sentence_ptr() const {
		return sentence_;
	}

	/// Shorthand to get_sentence().size()
	int size() const {
		return sentence_->size();
	}

	/// Position accessor
	int get_position() const {
		return position_;
	}

	/// Checks if the current position is valid (within the sentence bounds)
	bool is_current_inside() const {
		return is_inside(position_);
	}

	/// Checks if the the given absolute position is valid (in sentence bounds)
	bool is_inside(int abs_pos) const {
		return abs_pos >= 0 && abs_pos < size();
	}

	/// Checks if the the given position is valid (in sentence bounds)
	bool is_inside(const Position& pos) const {
		return is_inside(get_abs_position(pos));
	}

	/// Checks if the the given absolute position is outside sentence, including nowhere
	bool is_outside(int abs_pos) const {
		return !is_inside(abs_pos);
	}

	/// Checks if the the given position is outside sentence, including nowhere
	bool is_outside(const Position& pos) const {
		return !is_inside(get_abs_position(pos));
	}

	/**
	 * Checks if range is valid, i.e. non-empty and not outside of sentence,
	 * and at the same time finds absolute values of passed Positions
	 * @param left Position specyfing left extreme of the range
	 * @param right Position specyfing right extreme of the range
	 * @param abs_left reference to int value that will hold absolute
	 * value for left position
	 * @param abs_right reference to int value that will hold absolute
	 * value for right position
	 * @param may_cross set to true if crossing begin and end is admissible
	 * @returns true if range is valid; in this case abs_left and abs_right
	 * are set to absolute positions values for left and right Position.
	 * False is returned otherwise; in this case abs_left and abs_right
	 * are set to Nowhere
	 */
	bool validate_range(
			const Position& left,
			const Position& right,
			int& abs_left,
			int& abs_right,
			bool may_cross = false) const;

	/// Position setter
	void set_position(int new_position) {
		position_ = new_position;
	}

	/// Position advance shorthand
	void advance() {
		++position_;
	}
	
	/// Position recede shorthand
	void recede() {
		--position_;
	}

	/// Reset position to point to the first token
	void goto_start() {
		position_ = 0;
	}

	/// Token access convenience function - const.
	/// Will return NULL if the passed position is not valid in this Sentence
	const Corpus2::Token* at(int abs_pos) const {
		if (is_inside(abs_pos)) {
			return get_sentence()[abs_pos];
		} else {
			return NULL;
		}
	}

	/// Token access const function overload that takes a Position object
	const Corpus2::Token* at(const Position& position) const {
		return at(get_abs_position(position));
	}

	/// Token access convenience function.
	/// Will return NULL if the passed position is not valid in this Sentence
	Corpus2::Token* at(int abs_pos) {
		if (is_inside(abs_pos)) {
			return get_sentence()[abs_pos];
		} else {
			return NULL;
		}
	}

	/// Token access function overload that takes a Position object
	Corpus2::Token* at(const Position& position) {
		return at(get_abs_position(position));
	}

	/// Current token access shorthand, const. @see at
	const Corpus2::Token* current() const {
		return at(position_);
	}

	/// Current token access shorthand. @see at
	Corpus2::Token* current() {
		return at(position_);
	}

    /// Translate a Position into an absolute index into
    /// the Sentence wrapped by this SentenceContext.
    /// This takes proper care of the special `begin', 
    /// `end' and `nowhere' Positions.
	int get_abs_position(const Position& position) const;

    /// Translate a Position into an relative index into
    /// the Sentence wrapped by this SentenceContext.
    /// (Relative to the current position).
    /// This takes proper care of the special `begin', 
    /// `end' and `nowhere' Positions.
	int get_rel_position(const Position& position) const;

    /// Helper function for translatin special Positions
    /// (`begin', `end' and `nowhere')
	int translate_special_position(int pos) const {
		switch (pos) {
		case Position::Begin:
			return 0;
		case Position::End:
			return sentence_->size() - 1;
		case Position::Nowhere:
			return Position::Nowhere;
		default:
			return position_ + pos;
		}
	}

private:
	/// The wrapped sentence
	boost::shared_ptr<Corpus2::Sentence> sentence_;

	/// Current position in the sentence
	int position_;
};


inline
bool SentenceContext::validate_range(
	const Position& left,
	const Position& right,
	int& abs_left,
	int& abs_right,
	bool may_cross) const
{
	abs_left = get_abs_position(left);

	if (abs_left == Position::Nowhere) {
		abs_right = Position::Nowhere;
		return false;
	}
	abs_right = get_abs_position(right);
	if (abs_right == Position::Nowhere) {
		abs_left = Position::Nowhere;
		return false;
	}
	// Trim range to sentence boundaries
	if (abs_left < 0) {
		abs_left = 0;
	}
	if (abs_right >= size()) {
		abs_right = size() - 1;
	}

	// is range valid? this covers "crossed" range, an empty sentence,
	// and range outside boundaries of sentence
	if (!may_cross && (abs_left > abs_right)) {
		abs_left = Position::Nowhere;
		abs_right = Position::Nowhere;
		return false;
	}
	return true;
}

// TODO ConstSentenceContext ?

} /* end ns Wccl */

#endif // LIBWCCL_SENTENCECONTEXT_H
