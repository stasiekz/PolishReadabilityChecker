/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

/* Code to display version info and greeting message when the .so is run */

#ifdef __GNUG__
#ifdef __linux__

#ifndef LIBMACA_INTERP
#ifdef __LP64__
#define LIBMACA_INTERP "/lib/ld-linux-x86-64.so.2";
#else
#define LIBMACA_INTERP "/lib/ld-linux.so.2";
#endif
#endif

#ifdef LIBMACA_INTERP
extern "C"
const char my_interp[] __attribute__((section(".interp"))) = LIBMACA_INTERP;
#endif

#ifdef HAVE_CONFIG_D_H
#include <libmaca/config_d.h>
#endif
#include <cstdlib>
#include <iostream>
#include <boost/algorithm/string.hpp>

#ifndef LIBMACA_VERSION
#define LIBMACA_VERSION "?"
#endif

extern "C"
int LIBMACA_entry_()
{
	std::cout << "This is libmaca-" LIBMACA_VERSION ", a configurable morphological analysis library.\n";
	std::cout << "Data dir configured as: " << LIBMACA_DATA_DIR << "\n";
#ifdef HAVE_MORFEUSZ
	std::cout << "Built with Morfeusz support.\n";
#else
	std::cout << "Built without Morfeusz support.\n";
#endif
#ifdef HAVE_SFST
	std::cout << "Built with SFST support.\n";
#else
	std::cout << "Built without SFST support.\n";
#endif
	exit(0);
}

#endif
#endif
