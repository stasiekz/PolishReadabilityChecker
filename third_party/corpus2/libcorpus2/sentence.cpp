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

#include <libcorpus2/sentence.h>
#include <boost/make_shared.hpp>

namespace Corpus2 {

Sentence::Sentence(const std::string &id)
	: tokens_(), id_(id)
{
}

Sentence::~Sentence()
{
	BOOST_FOREACH(const Token* t, tokens_) {
		delete t;
	}
}

Sentence::Ptr Sentence::clone_shared() const
{
	Sentence::Ptr s = boost::make_shared<Sentence>(id_);
	BOOST_FOREACH(const Token* t, tokens_) {
		s->append(t->clone());
	}
	return s;
}

void Sentence::release_tokens()
{
	tokens_.clear();
}


} /* end ns Corpus2 */
