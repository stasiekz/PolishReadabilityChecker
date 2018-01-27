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

#include <libcorpus2/lexeme.h>

namespace Corpus2 {

Lexeme::Lexeme()
	: lemma_(), tag_(), disamb_(false)
{
}

Lexeme::Lexeme(const UnicodeString &lemma, const Tag &tag)
	: lemma_(lemma), tag_(tag), disamb_(false)
{
}

Lexeme Lexeme::create(const UnicodeString& lemma, const Tag& tag)
{
	return Lexeme(lemma, tag);
}

Lexeme Lexeme::create_utf8(const std::string& lemma_utf8, const Tag& tag)
{
	return Lexeme(UnicodeString::fromUTF8(lemma_utf8), tag);
}


bool Lexeme::is_null() const
{
	return lemma().length() == 0 || tag().is_null();
}

bool Lexeme::operator<(const Lexeme& other) const
{
	return lemma_ < other.lemma_
			|| (lemma_ == other.lemma_
				&& (tag_ < other.tag_
					|| (tag_ == other.tag_
						&& disamb_ && !other.disamb_)));
}

bool Lexeme::operator==(const Lexeme& other) const
{
	return lemma_ == other.lemma_ && tag_ == other.tag_ &&
			disamb_ == other.disamb_;
}

bool Lexeme::DisamblessComparator::operator()(const Lexeme& l, const Lexeme& other) const
{
	return l.lemma_ == other.lemma_	&& l.tag_ == other.tag_;
}

} /* end ns Corpus2 */
