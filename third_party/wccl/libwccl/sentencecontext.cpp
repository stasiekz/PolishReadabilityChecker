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

#include <libwccl/sentencecontext.h>

namespace Wccl {

SentenceContext::SentenceContext(const boost::shared_ptr<Corpus2::Sentence>& s)
	: sentence_(s), position_(0)
{
}

SentenceContext SentenceContext::duplicate() const
{
	SentenceContext dup(*this);
	dup.sentence_ = sentence_->clone_shared();
	return dup;
}

SentenceContext* SentenceContext::clone() const
{
	return new SentenceContext(duplicate());
}

int SentenceContext::get_abs_position(const Position &position) const
{
	return translate_special_position(position.get_value());
}

int SentenceContext::get_rel_position(const Position &position) const
{
	return get_abs_position(position) - position_;
}

} /* end ns Wccl */
