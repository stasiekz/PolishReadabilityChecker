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

#ifndef LIBCORPUS2_WHOLE_DOCREADER_H
#define LIBCORPUS2_WHOLE_DOCREADER_H

#include <libcorpus2/io/cclreader.h>
#include <libcorpus2_whole/document.h>
#include <libcorpus2_whole/io/baserelreader.h>

#include <boost/shared_ptr.hpp>

namespace Corpus2 {
namespace whole {

/**
 * A reader for whole documents. Note that a whole document is read into memory
 * before any processing may take place.
 */
class CclRelReader : public BaseRelReader {
public:
	/**
	 * Reads a whole document, using the two given path: the morphosyntax and
	 * chunk-style annotations are read from annot_path, while relations
	 * between chunk-style annotations are read from rela_path.
	 * Both path may in particular point to the same path.
	 * @param tagset Tagset to use
	 * @param annot_path Path to file with morphosyntax and chunk-style annotations
	 * @param rela_path  path to file with relations
	 */
	CclRelReader(const Tagset& tagset,
				   const std::string &annot_path,
				   const std::string &rela_path);

	/**
	 * Reads document stored in given file(s), in file with morphosyntax and
	 * chunk-style annotations and from file with relations.
	 * @return Pointer to readed Document
	 */
	boost::shared_ptr<Document> read();

	/**
	 * Sets options for readers (relation reader and/or ccl reader).
	 * Available options:
	 *  - autogen_sent_id -- for automatically generation identifiers of sentences
	 */

	/**
	 * @return option
	 */
	std::string get_option(const std::string& option) const;

private:
	/**
	 * Makes CclReader and RelationReader for given paths to files.
	 * @param annot_path Path to file with morphosyntax and chunk-style annotations
	 * @param tagset Tagset to use in CclReader
	 * @param rela_path  path to file with relations
	 */
	void make_readers(
			const Tagset& tagset,
			const std::string &annot_path,
			const std::string &rela_path);
};

} // whole ns
} // Corpus2 ns

#endif // LIBCORPUS2_WHOLE_DOCREADER_H
