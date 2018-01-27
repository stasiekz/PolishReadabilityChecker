/*
    Copyright (C) 2011 Adam Wardyński, Tomasz Śniatowski, Paweł Kędzia,
    Adam Radziszewski, Bartosz Broda
    Part of the WCCL project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBWCCL_LEXICON_LEXICONS_H
#define LIBWCCL_LEXICON_LEXICONS_H

#include <libwccl/lexicon/lexicon.h>
#include <boost/shared_ptr.hpp>

namespace Wccl {

class Lexicons : boost::noncopyable
{
public:
	typedef boost::unordered_map<std::string, boost::shared_ptr<Lexicon> > map_t;

	Lexicons()
		: lexicons_()
	{
	}

	bool has_lexicon(const std::string& name) const {
		return lexicons_.find(name) != lexicons_.end();
	}

	const Lexicon& get(const std::string& name) const;

	boost::shared_ptr<const Lexicon> get_ptr(const std::string& name) const;

	void insert(const boost::shared_ptr<Lexicon>& lexicon);

	const map_t& get_lexicons() const {
		return lexicons_;
	}

private:
	map_t lexicons_;
};

} /* end ns Wccl */

#endif // LIBWCCL_LEXICON_LEXICONS_H
