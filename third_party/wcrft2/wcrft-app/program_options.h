/* This file is part of WCRFT
  Copyright (C) 2014 Adam Radziszewski, Paweł Orłowicz.
  WCRFT is free software; you can redistribute and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option)
  any later version.

  WCRFT is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.

  See the LICENCE and COPYING files for more details
 */

namespace prog_opts = boost::program_options;

prog_opts::variables_map program_options_handler(int argc, char **argv, const std::string& description);

prog_opts::options_description create_options_description(const std::string& description);

prog_opts::positional_options_description create_positional_options_description();

prog_opts::variables_map parse_options(int argc, char **argv,
									   prog_opts::options_description desc,
									   prog_opts::positional_options_description positional_desc);
