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

#ifndef LIBWCCL_LEXICON_LEXICONPARSER_H
#define LIBWCCL_LEXICON_LEXICONPARSER_H

#include <boost/shared_ptr.hpp>
#include <libpwrutils/pathsearch.h>

namespace Wccl {

class Lexicon;

class LexiconParser
{
public:
	static boost::shared_ptr<Lexicon> parse_lexicon(
		const PwrNlp::PathSearcherBase& search_path,
		const std::string& lexicon_name,
		const std::string& filename);
};
} /* end ns Wccl */

#endif // LIBWCCL_LEXICON_LEXICONPARSER_H
