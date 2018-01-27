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

#include <libtoki/unicode/sink.h>
#include <libtoki/unicode/nullsource.h>
#include <libtoki/unicode/icustringwrapper.h>
#include <libtoki/unicode/istreamwrapper.h>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>

namespace Toki {

UnicodeSink::UnicodeSink()
	: input_()
{
	set_null_input_source();
}

UnicodeSink::UnicodeSink(UnicodeSource *input)
	: input_()
{
	set_input_source(input);
}

UnicodeSink::UnicodeSink(const boost::shared_ptr<UnicodeSource>& input)
	: input_()
{
	set_input_source(input);
}

UnicodeSink::UnicodeSink(std::istream &is, int bufsize)
	: input_()
{
	set_input_source(is, bufsize);
}

UnicodeSink::UnicodeSink(const UnicodeString &s)
	: input_()
{
	set_input_source(s);
}

UnicodeSink::~UnicodeSink()
{
}

void UnicodeSink::set_null_input_source()
{
	set_input_source(boost::make_shared<NullUnicodeSource>());
}

void UnicodeSink::set_input_source(UnicodeSource *us)
{
	input_.reset(us);
	new_input_source();
}

void UnicodeSink::set_input_source(
		const boost::shared_ptr<UnicodeSource>& us)
{
	input_ = us;
	new_input_source();
}

void UnicodeSink::set_input_source(std::istream &is, int bufsize)
{
	set_input_source(boost::make_shared<UnicodeIstreamWrapper>(
				boost::ref(is), bufsize));
}

void UnicodeSink::set_input_source(const UnicodeString &s)
{
	set_input_source(boost::make_shared<UnicodeIcuStringWrapper>(s));
}


} /* end ns Toki */
