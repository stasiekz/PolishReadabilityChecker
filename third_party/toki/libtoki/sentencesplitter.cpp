/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <libtoki/sentencesplitter.h>

namespace Toki {

SentenceSplitter::SentenceSplitter(TokenSource& ts)
	: ts_(&ts), buf_(NULL)
{
}

SentenceSplitter::~SentenceSplitter()
{
	delete buf_;
}

void SentenceSplitter::set_source(TokenSource &ts)
{
	ts_ = &ts;
}

Token* SentenceSplitter::get_buf()
{
	Token* tmp = buf_;
	buf_ = NULL;
	return tmp;
}

bool SentenceSplitter::has_more()
{
	if (buf_ != NULL) return true;
	buf_ = source().get_next_token();
	return buf_ != NULL;
}

Sentence* SentenceSplitter::get_next_sentence()
{
	Sentence* sentence = new Sentence();
	if (buf_ == NULL) {
		buf_ = source().get_next_token();
	}
	if (buf_) {
		sentence->append(buf_);
		buf_ = source().get_next_token();
		while (buf_ != NULL && !buf_->begins_sentence()) {
			sentence->append(buf_);
			buf_ = source().get_next_token();
		}
	}
	return sentence;
}

} /* end ns Toki */
