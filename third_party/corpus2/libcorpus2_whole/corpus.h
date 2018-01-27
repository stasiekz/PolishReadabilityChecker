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

#ifndef LIBCORPUS2_WHOLE_CORPUS_H
#define LIBCORPUS2_WHOLE_CORPUS_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <libcorpus2_whole/document.h>

namespace Corpus2 {
namespace whole {

/**
 * Represents Corpus
 */
class Corpus
{
public:
	/**
	 * Corpus constructor takes one parameter:
	 * @arg name Name of the corpus - default is empty
	 */
	Corpus(const std::string name = "");

	/**
	 * Adds one (readed) document to corpus
	 */
	void add_document(boost::shared_ptr<Document> document) {
		this->documents_.push_back(document);
	}

	/// Returns list of the documents from corpus
	const std::vector<boost::shared_ptr<Document> > documents() const {
		return this->documents_;
	}

	/// Next document in corpus
	boost::shared_ptr<Document> next_document() {
		static boost::shared_ptr<Document> _empty_doc_ptr;
		if (!this->set_) {
			if (this->documents_.size() == 0) {
				return _empty_doc_ptr;
			}
			this->set_ = true;
			this->current_document_ = documents_.begin();
		}
		else {
			current_document_++;
		}

		if (current_document_ == documents_.end()) {
			return _empty_doc_ptr;
		}

		return *current_document_;
	}

private:
	bool set_;

	/// Corpus name
	const std::string name_;

	/// List of the documents in corpus
	std::vector<boost::shared_ptr<Document> > documents_;

	/// Current document
	std::vector<boost::shared_ptr<Document> >::iterator current_document_;
};

} // whole ns
} // Corpus2 ns

#endif // LIBCORPUS2_WHOLE_CORPUS_H
