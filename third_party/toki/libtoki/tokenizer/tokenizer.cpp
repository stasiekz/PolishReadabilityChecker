/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <libtoki/token.h>
#include <libtoki/tokenizer/tokenizer.h>


#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <ostream>

namespace Toki {

	Tokenizer::Tokenizer(const Config::Node& cfg)
		: TokenSource(), UnicodeSink()
	{
		apply_configuration(cfg);
	}

	Tokenizer::Tokenizer(UnicodeSource *input, const Config::Node& cfg)
		: TokenSource(), UnicodeSink(input)
	{
		apply_configuration(cfg);
	}

	Tokenizer::Tokenizer(const boost::shared_ptr<UnicodeSource>& input,
			const Config::Node &cfg)
		: TokenSource(), UnicodeSink(input)
	{
		apply_configuration(cfg);
	}

	Tokenizer::Tokenizer(std::istream &is, const Config::Node &cfg)
		: TokenSource()
		, UnicodeSink(is, cfg.get<int>("input_buffer_size", 1000))
	{
		apply_configuration(cfg);
	}

	Tokenizer::Tokenizer(const UnicodeString &s, const Config::Node &cfg)
		: TokenSource(), UnicodeSink(s)
	{
		apply_configuration(cfg);
	}

	Tokenizer::~Tokenizer()
	{
	}


	void Tokenizer::restart()
	{
	}

	void Tokenizer::apply_configuration(const Config::Node &/*cfg*/)
	{
	}

} /* end namespace Toki */
