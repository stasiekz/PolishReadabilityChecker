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

#ifndef LIBCORPUS2_WHOLE_DOCUMENT_H
#define LIBCORPUS2_WHOLE_DOCUMENT_H

#include <libcorpus2/chunk.h>
#include <libcorpus2_whole/relation.h>
#include <boost/shared_ptr.hpp>

namespace Corpus2 {
namespace whole {

/**
 * A whole document, consisting of consecutive paragraphs ("chunks"), being
 * sequences of sentences.
 * Usage of this class assumes that a whole document is read into memory before
 * any further takes place.
 */
class Document
{
public:
	/**
	 * Path to file, if not set, then default is empty
	 */
	Document(const std::string& path = "");
	~Document();

	/// Adds paragraphs to document
	void add_paragraph(const boost::shared_ptr<Chunk> para) {
		paragraphs_.push_back(para);
	}

	/// Adds relation to document relations
	void add_relation(const boost::shared_ptr<Relation> relation) {
		relations_.push_back(relation);
	}

	/// Paragraphs accessor
	const std::vector< boost::shared_ptr<Chunk> >& paragraphs() const {
		return paragraphs_;
	}

	/// Relations accessor
	const std::vector< boost::shared_ptr<Relation> >& relations() const {
		return relations_;
	}

	/// Returns path to the document
	const std::string& path() const {
		return path_;
	}

protected:
	/// Paragraphs in document
	std::vector<boost::shared_ptr<Chunk> > paragraphs_;

	/// Relations in document
	std::vector<boost::shared_ptr<Relation> > relations_;

	/// Path to the file (if it's not a file, then is empty)
	const std::string path_;
};

} // whole ns
} // Corpus2 ns

#endif // LIBCORPUS2_WHOLE_DOCUMENT_H
