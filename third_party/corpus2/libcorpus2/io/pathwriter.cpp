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

#include <libcorpus2/io/pathwriter.h>

namespace Corpus2 {

PathWriter::PathWriter(const boost::shared_ptr<TokenWriter>& underlying,
		const boost::shared_ptr<std::ostream>& os)
	: TokenWriter(*os, underlying->tagset(), string_range_vector()),
	 os_(os), underlying_(underlying)
{
    assert(&underlying_->os() == os_.get());
}

PathWriter::~PathWriter()
{
}

void PathWriter::write_token(const Token& t)
{
	underlying_->write_token(t);
}

void PathWriter::write_sentence(const Sentence& s)
{
	underlying_->write_sentence(s);
}

void PathWriter::write_chunk(const Chunk& p)
{
	underlying_->write_chunk(p);
}

} /* end ns Corpus2 */
