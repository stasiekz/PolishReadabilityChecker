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

#ifndef LIBMACA_UTIL_TOKENTIMER_H
#define LIBMACA_UTIL_TOKENTIMER_H

#include <ctime>
#include <libcorpus2/chunk.h>

#include <loki/Singleton.h>

namespace Maca {

/**
 * Utility class for token counting and timing, useful in progress info.
 */
class TokenTimer
{
public:
	/// Constructor
	TokenTimer();

	/// count one token
	void count_token() {
		++tokens_;
		++slice_tokens_;
	}

	/// count a sentence whose tokens are counted separetely
	void count_sentence() {
		++sentences_;
	}

	/// count a sentence with tokens
	void count_sentence(const Corpus2::Sentence& s) {
		++sentences_;
		tokens_ += s.size();
		slice_tokens_ += s.size();
	}

	/// count a chunk with sentences with tokens
	void count_chunk(const Corpus2::Chunk& c) {
		for (size_t i = 0; i < c.sentences().size(); ++i) {
			count_sentence(*c.sentences()[i]);
		}
	}

	/// reset the timer
	void restart();

	/// check if at least s seconds have elapsed since the last
	/// slice began and if yes, display progress info and start a new slice
	bool check_slice(int s = 1);

	/// display current stats
	void stats();


	void register_signal_handler();

private:
	size_t tokens_;
	size_t slice_tokens_;
	size_t sentences_;
	clock_t start_;
	clock_t slice_start_;
};


typedef Loki::SingletonHolder< TokenTimer > TokenTimerSingleton;

inline TokenTimer& global_timer() {
	return TokenTimerSingleton::Instance();
}


} /* end ns Maca */

#endif // LIBMACA_UTIL_TOKENTIMER_H
