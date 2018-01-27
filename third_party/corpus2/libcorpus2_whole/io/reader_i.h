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

#ifndef READERI_H
#define READERI_H

#include <string>
#include <boost/shared_ptr.hpp>

#include <libcorpus2_whole/document.h>
#include <libcorpus2_whole/corpus.h>

namespace Corpus2 {
namespace whole {

/**
 * Reader interface,
 * Contains method to get reader type (such as document, poliqarp).
 * Method for reading read must by implemented!
 */
class DocumentReaderI
{
public:
	DocumentReaderI(const std::string type) : type_(type) {}

	const std::string& type() const { return type_; }

	virtual boost::shared_ptr<Document> read() = 0;

protected:
	const std::string type_;
};

class CorpusReaderI
{
public:
	virtual boost::shared_ptr<Corpus> read(const std::string& corpus_file) = 0;
};

} // whole ns
} // Corpus2 ns

#endif // READERI_H
