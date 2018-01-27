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

#include <libcorpus2/io/helpers.h>
#include <libcorpus2/io/reader.h>
#include <sstream>
namespace Corpus2 {

std::vector<boost::shared_ptr<Chunk> > read_chunks_from_utf8_string(
        const std::string& data,
        const Tagset& tagset,
        const std::string& format)
{
	std::stringstream ss;
	ss << data;
	boost::shared_ptr<TokenReader> reader = TokenReader::create_stream_reader(
		format, tagset, ss);
	std::vector<boost::shared_ptr<Chunk> > chunks;
	while (boost::shared_ptr<Chunk> c = reader->get_next_chunk()) {
		chunks.push_back(c);
	}
	return chunks;
}

} /* end ns Corpus2 */
