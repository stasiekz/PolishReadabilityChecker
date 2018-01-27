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

#ifndef LIBCORPUS2_WHOLE_CORPUSREADER_H
#define LIBCORPUS2_WHOLE_CORPUSREADER_H

#include <string>
#include <libcorpus2_whole/corpus.h>
#include <libcorpus2_whole/io/reader_i.h>

namespace Corpus2 {
namespace whole {

class CorpusReader
{
public:
	/**
	 * Constructor of corpus reader. Here must be set type of the corpus, which
	 * will be reading.
	 * @arg corpus_type may be:
	 *  - document (contains relations)
	 *  - poliqarp
	 */
	CorpusReader(const Tagset& tagset, const std::string& corpus_type);

	virtual ~CorpusReader();	

	/**
	 * Reads corpus from given path
	 * @arg corpus_file Path to file contains paths to corpus files.
	 *      Depend on corpus type, each line in this file should contains only
	 *      path to one document from corpus or path to (in particular DocReader)
	 *      relations and annotatons (in one line, first is path to annotations
	 *      and second are relations -- these paths, should be separated by semicolon)
	 * @return Readed corpus
	 */
	boost::shared_ptr<Corpus> read(const std::string& corpus_file);

private:
	/// Returns reader based on corpus type (poliqarp/document)
	boost::shared_ptr<CorpusReaderI> get_corpus_reader_by_type();

private:
	/// Type of corpus, sets only once in constructor
	const std::string corpus_type_;

	/// Tagset to use, sets only onece in constructor
	const Tagset& tagset_;
};

} // whole ns
} // Corpus2 ns

#endif // LIBCORPUS2_WHOLE_CORPUSREADER_H
