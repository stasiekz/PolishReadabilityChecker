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

#ifndef LIBCORPUS2_WHOLE_POLIQARPDOCUMENTREADER_H
#define LIBCORPUS2_WHOLE_POLIQARPDOCUMENTREADER_H

#include <poliqarp/pqreader.h>
#include <libcorpus2_whole/document.h>
#include <libcorpus2_whole/io/reader_i.h>

namespace Corpus2 {
namespace whole {

/**
 * Wrapper for PoliqarpReader.
 * Can be used as "document" reader for Poliqarp corpus. Method read() retuns
 * document from given corpus path. Behavior of this method is similar to
 * get_next_document() from Poliqarp Client. First call of read() gives first
 * document in corpus, nth call of read() gives nth document from corpus...
 */
class PoliqarpDocumentReader : public DocumentReaderI
{
public:
	PoliqarpDocumentReader(const Tagset& tagset, const std::string& corpus_path, const std::string& corpus_reader);
	PoliqarpDocumentReader(const Tagset& tagset, const std::string& corpus_path);
	/**
	 * Semantic of this methd is similar to get_next_document from Poliqarp Client
	 * @return nth readed document
	 */
	boost::shared_ptr<Document> read();

	/**
	 * Sets options for readers (relation reader and/or ccl reader).
	 * Available options:
	 *  - autogen_sent_id -- for automatically generation identifiers of sentences
	 */
	void set_option(const std::string& option);

private:
	/// Poliqarp reader used for reading Poliqarp corp
        Corpus2::TokenReader::TokenReaderPtr pqr_;
};

} // whole ns
} // Corpus2 ns

#endif // LIBCORPUS2_WHOLE_POLIQARPDOCUMENTREADER_H
