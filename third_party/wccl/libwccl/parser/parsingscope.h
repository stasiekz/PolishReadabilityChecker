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

#ifndef LIBWCCL_PARSER_PARSINGSCOPE_H
#define LIBWCCL_PARSER_PARSINGSCOPE_H

#include <libwccl/variables.h>
#include <libwccl/lexicon/lexicons.h>
#include <libwccl/wcclfile.h>

namespace Wccl {

class ParsingScope
{
public:
	ParsingScope(const boost::shared_ptr<const Corpus2::Tagset> tagset, const Lexicons& lexicons)
		: tagset_(tagset),
		  variables_(new Variables()),
		  lexicons_(lexicons)
	{
	}

	explicit ParsingScope(const WcclFile& file)
		: tagset_(file.tagset()),
		  variables_(new Variables()),
		  lexicons_(file.get_lexicons())
	{
	}

	const boost::shared_ptr<const Corpus2::Tagset> tagset() const {
		return tagset_;
	}

	Variables& variables() {
		return *variables_;
	}

	const Variables& variables() const {
		return *variables_;
	}

	const Lexicons& lexicons() const {
		return lexicons_;
	}

	void reset_variables() {
		variables_->reset_values();
	}
private:
	const boost::shared_ptr<const Corpus2::Tagset> tagset_;
	boost::shared_ptr<Variables> variables_;
	const Lexicons& lexicons_;
};

} /* end ns Wccl */

#endif // LIBWCCL_PARSER_PARSINGSCOPE_H
