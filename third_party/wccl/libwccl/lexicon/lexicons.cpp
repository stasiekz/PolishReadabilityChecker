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

#include <libwccl/lexicon/lexicons.h>
#include <libwccl/exception.h>

namespace Wccl {

const Lexicon& Lexicons::get(const std::string& name) const
{
	map_t::const_iterator i = lexicons_.find(name);
	if (i == lexicons_.end()) {
		throw InvalidArgument("name", "No lexicon of given name: " + name);
	}
	return *i->second;
}

boost::shared_ptr<const Lexicon> Lexicons::get_ptr(const std::string& name) const
{
	map_t::const_iterator i = lexicons_.find(name);
	if (i == lexicons_.end()) {
		throw InvalidArgument("name", "No lexicon of given name: " + name);
	}
	return i->second;
}

void Lexicons::insert(const boost::shared_ptr<Lexicon>& lexicon)
{
	BOOST_ASSERT(lexicon);
	if (has_lexicon(lexicon->name())) {
		throw InvalidArgument(
			"lexicon",
			"Lexicon named \"" + lexicon->name() + "\" already added.");
	}
	lexicons_[lexicon->name()] = lexicon;
}

} /* end ns Wccl */
