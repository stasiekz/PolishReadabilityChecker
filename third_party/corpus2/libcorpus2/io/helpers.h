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

#ifndef LIBSORPUS2_IO_HELPERS_H
#define LIBCORPUS2_IO_HELPERS_H

#include <libcorpus2/chunk.h>
#include <libcorpus2/tagset.h>

namespace Corpus2 {

std::vector<boost::shared_ptr<Chunk> > read_chunks_from_utf8_string(
	const std::string& data,
	const Tagset& tagset,
	const std::string& format);

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_HELPERS_H
