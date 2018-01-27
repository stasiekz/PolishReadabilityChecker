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

#ifndef LIBSORPUS2_IO_PLAINREADER_H
#define LIBCORPUS2_IO_PLAINREADER_H

#include <libcorpus2/io/reader.h>
#include <boost/scoped_ptr.hpp>

namespace Corpus2 {

class PlainReader : public BufferedSentenceReader
{
public:
	PlainReader(const Tagset& tagset, std::istream& is);

	PlainReader(const Tagset& tagset, const std::string& filename);

	std::istream& is() {
		return *is_;
	}

	void set_option(const std::string& option);

	std::string get_option(const std::string& option) const;

	static bool registered;

protected:
	/// BufferedSentenceReader override
	Sentence::Ptr actual_next_sentence();

	std::istream* is_;
	boost::scoped_ptr<std::istream> is_owned_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_PLAINREADER_H
