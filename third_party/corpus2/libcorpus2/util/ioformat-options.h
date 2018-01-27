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

#ifndef LIBSORPUS2_UTIL_IOFORMAT_OPTIONS_H
#define LIBCORPUS2_UTIL_IOFORMAT_OPTIONS_H

#include <boost/program_options.hpp>
#include <libcorpus2/io/reader.h>
#include <libcorpus2/io/writer.h>

namespace Corpus2 {

void add_input_options(
	boost::program_options::options_description& desc,
	const std::string& default_format = "xces"
	);

void add_output_options(
	boost::program_options::options_description& desc,
	const std::string& default_format = "xces"
	);

boost::shared_ptr<Corpus2::TokenReader> create_reader(
	boost::program_options::variables_map& vm,
	const Corpus2::Tagset& tagset,
	const std::string& filename = "-"
	);

boost::shared_ptr<Corpus2::TokenWriter> create_writer(
	boost::program_options::variables_map& vm,
	const Corpus2::Tagset& tagset,
	const std::string& filename = "-"
	);

} /* end ns Corpus2 */

#endif // LIBCORPUS2_UTIL_IOFORMAT_OPTIONS_H
