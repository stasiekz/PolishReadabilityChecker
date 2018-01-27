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

#include <libwccl/lexicon/lexiconparser.h>
#include "ANTLRLexiconParser.hpp"
#include "ANTLRLexiconLexer.hpp"

#include <libwccl/lexicon/lexicon.h>
#include <libwccl/exception.h>

#include <fstream>

namespace Wccl {

boost::shared_ptr<Lexicon> LexiconParser::parse_lexicon(
	const PwrNlp::PathSearcherBase& search_path,
	const std::string& lexicon_name,
	const std::string& filename)
{
	std::ifstream is;
	search_path.open_stream_or_throw(filename, is, "lexicon");
	if (!is.good()) {
		throw Wccl::FileNotFound(filename, "", __FUNCTION__);
	}

	ANTLRLexiconLexer lexer(is);
	ANTLRLexiconParser parser(lexer);
	return parser.parse_lexicon_file(lexicon_name, filename);
}

} /* end ns Wccl */
