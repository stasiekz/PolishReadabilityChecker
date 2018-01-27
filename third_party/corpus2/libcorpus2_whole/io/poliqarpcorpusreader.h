/*
	Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski, Paweł Kędzia
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

#ifndef LIBCORPUS2_WHOLE_POLIQARPCORPUSREADER_H
#define LIBCORPUS2_WHOLE_POLIQARPCORPUSREADER_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <libcorpus2_whole/corpus.h>
#include <libcorpus2_whole/io/poliqarpdocumentreader.h>
#include <libcorpus2_whole/io/reader_i.h>

namespace Corpus2 {
namespace whole {

class PoliqarpCorpusReader : public CorpusReaderI
{
public:
	PoliqarpCorpusReader(const Tagset& tagset);

	boost::shared_ptr<Corpus> read(const std::string& corpus_file);

private:
	const Tagset& tagset_;
	boost::shared_ptr<PoliqarpDocumentReader> pq_doc_reader_;
};

} // whole ns
} // Corpus2 ns

#endif // LIBCORPUS2_WHOLE_POLIQARPCORPUSREADER_H
