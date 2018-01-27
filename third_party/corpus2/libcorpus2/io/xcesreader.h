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

#ifndef LIBCORPUS2_IO_XCESREADER_H
#define LIBCORPUS2_IO_XCESREADER_H

#include <libcorpus2/io/reader.h>
#include <libcorpus2/io/xces.h>
#include <libcorpus2/chunk.h>
#include <deque>
#include <boost/scoped_ptr.hpp>

namespace Corpus2 {

class XcesReaderImpl;

class XcesReader : public BufferedChunkReader
{
public:
	XcesReader(const Tagset& tagset, std::istream& is,
			bool disamb_only = false, bool disamb_sh = false);

	XcesReader(const Tagset& tagset, const std::string& filename,
			bool disamb_only = false, bool disamb_sh = false);

	~XcesReader();

	std::istream& is() {
		return *is_;
	}

	void set_option(const std::string& option);

	std::string get_option(const std::string& option) const;

	static bool registered;

protected:
	void ensure_more();

	// std::istream& is_;
	std::istream* is_;
	boost::scoped_ptr<std::istream> is_owned_;

	boost::scoped_ptr<XcesReaderImpl> impl_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_XCESREADER_H
