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

#ifndef LIBCORPUS2_CHUNK_H
#define LIBCORPUS2_CHUNK_H

#include <libcorpus2/sentence.h>
#include <boost/utility.hpp>

namespace Corpus2 {

/**
 * A chunk groups sentences together, and also has some string attributes
 * in key-value form.
 */
class Chunk : boost::noncopyable
{
public:
	Chunk();

	template <typename T>
	explicit Chunk(const T& range);

	~Chunk();

	boost::shared_ptr<Chunk> clone_shared() const;

	bool empty() const {
		return sentences_.empty();
	}

	/// Size accessor
	size_t size() const {
		return sentences_.size();
	}

	bool has_attribute(const std::string& name) const;

	std::string get_attribute(const std::string& name) const;

	void set_attribute(const std::string& name, const std::string& value);

	std::vector< boost::shared_ptr<Sentence> >& sentences() {
		return sentences_;
	}

	const std::vector< boost::shared_ptr<Sentence> >& sentences() const {
		return sentences_;
	}

	void append(const boost::shared_ptr<Sentence>& s) {
		sentences_.push_back(s);
	}

	typedef std::map<std::string, std::string> attr_map_t;

	const attr_map_t& attributes() const {
		return attributes_;
	}

private:
	std::vector< boost::shared_ptr<Sentence> > sentences_;
	attr_map_t attributes_;
};

template <typename T>
Chunk::Chunk(const T &range)
	: sentences_(), attributes_()
{
	std::copy(range.begin(), range.end(), std::back_inserter(sentences_));
}

} /* end ns Corpus2 */

#endif // LIBCORPUS2_CHUNK_H
