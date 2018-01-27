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
#include <libcorpus2/io/nonewriter.h>

namespace Corpus2 {

bool NoneWriter::registered = TokenWriter::register_writer<NoneWriter>(
	"none");

NoneWriter::NoneWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: TokenWriter(os, tagset, params)
{
}

void NoneWriter::write_token(const Token&)
{
}

void NoneWriter::write_sentence(const Sentence&)
{
}

void NoneWriter::write_chunk(const Chunk&)
{
}


} /* end ns Corpus2 */
