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

#include <libmaca/morph/mapanalyser.h>

namespace Maca
{
template<>
const char* StdMapAnalyser::identifier = "map-case";

template<>
const char* StdMapCaselessAnalyser::identifier = "map";

template<>
const char* HashMapAnalyser::identifier = "hashmap";

template<>
const char* HashMapCaselessAnalyser::identifier = "hashmap-case";

template<>
bool StdMapAnalyser::registered =
		MorphAnalyser::register_analyser<StdMapAnalyser>();

template<>
bool StdMapCaselessAnalyser::registered =
		MorphAnalyser::register_analyser<StdMapCaselessAnalyser>();

template<>
bool HashMapAnalyser::registered =
		MorphAnalyser::register_analyser<HashMapAnalyser>();

template<>
bool HashMapCaselessAnalyser::registered =
		MorphAnalyser::register_analyser<HashMapCaselessAnalyser>();
}
