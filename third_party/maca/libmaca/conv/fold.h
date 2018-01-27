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

#ifndef LIBMACA_CONV_FOLD_H
#define LIBMACA_CONV_FOLD_H

#include <vector>
#include <boost/function.hpp>

namespace Corpus2 {
	class Token;
}

namespace Maca {
namespace Conversion {

/**
 * Helper function to find the shortest path -- the shortest vector
 * among the vectors passed.
 * @returns the shortest vector length
 * @param v the path vector
 * @param min_len_path output parameter where the index of the shortest
 *                     path is put
 */
size_t find_shortest(const std::vector<std::vector<Corpus2::Token *> >& v,
		size_t& min_len_path);

/**
 * Attempt path folding.
 *
 * Paths can be folded if:
 * - all have the same length
 * - all tokens at respective positions in each path have the same orth
 * If this is not the case, the function returns false and the input
 * vector is not modified. Otherwise all paths are merged tokenwise and
 * the resulting tokens are fed to the sink function object. All tokens
 * not fed to the sink are disposed of in that case.
 */
bool try_fold_paths(const std::vector< std::vector<Corpus2::Token*> >& v,
					boost::function<void (Corpus2::Token*)> sink);

/**
 * Discard all paths' tokens except for the path with the given index,
 * and feed the remaining path's tokens to the sink function object.
 */
std::vector<Corpus2::Token*> choose_path(
		const std::vector< std::vector<Corpus2::Token*> >& v, size_t n);

/**
 * Discard all paths' tokens except for the path with the given index,
 * and return the remaining path.
 */
void choose_path(const std::vector< std::vector<Corpus2::Token*> >& v, size_t n,
		boost::function<void (Corpus2::Token*)> sink);

/**
 * Find the shortest pathin the paths vector and return it, and dispose
 * of the other paths' tokens.
 */
std::vector<Corpus2::Token*> choose_shortest_path(
		const std::vector< std::vector<Corpus2::Token*> >& v);

/**
 * Find the shortest pathin the paths vector and feed its tokens to the
 * sink function, and delete other paths' tokens
 */
void choose_shortest_path(const std::vector< std::vector<Corpus2::Token*> >& v,
		boost::function<void (Corpus2::Token*)> sink);

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_CONV_FOLD_H
