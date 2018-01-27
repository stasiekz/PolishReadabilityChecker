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

#include <libmaca/util/tokentimer.h>
#include <iostream>

#include <signal.h>

namespace Maca {

TokenTimer::TokenTimer()
	: tokens_(0), slice_tokens_(0), sentences_(0)
	, start_(clock()), slice_start_(start_)
{
}

void TokenTimer::restart()
{
	tokens_ = slice_tokens_ = sentences_ = 0;
	start_ = slice_start_ = clock();
}

bool TokenTimer::check_slice(int s /*= 1*/)
{
	clock_t now_clock = clock();
	if (now_clock - CLOCKS_PER_SEC * s > slice_start_) {
		double slice_elapsed = ((double)now_clock - slice_start_) /
				CLOCKS_PER_SEC;
		double elapsed = ((double)now_clock - start_) / CLOCKS_PER_SEC;
		double slice_rate = slice_tokens_ / slice_elapsed;
		double avg_rate = tokens_ / elapsed;
		slice_tokens_ = 0;
		slice_start_ = now_clock;
		std::cerr << "\r" << "Processed " << sentences_ << " sentences, "
				<< tokens_ << " tokens, "
				<< "rate " << slice_rate << " t/s, "
				<< "avg " << avg_rate << " t/s    ";
		return true;
	} else {
		return false;
	}
}

void TokenTimer::stats()
{
	clock_t now_clock = clock();
	double elapsed = ((double)now_clock - start_) / CLOCKS_PER_SEC;
	double avg_rate = tokens_ / elapsed;
	std::cerr << "\r" << "Processed " << sentences_ << " sentences, "
			<< tokens_ << " tokens, "
			<< "avg rate " << avg_rate << " t/s                    \n";
}

namespace {
	void handler(int /*signal*/) {
		global_timer().stats();
	}
}

void TokenTimer::register_signal_handler()
{
#ifdef __unix__
	struct sigaction s;
	memset(&s, 0, sizeof(s));
	s.sa_handler = &handler;
	if (sigaction(SIGUSR1, &s, 0) != 0) {
		std::cerr << "Signal handler registration error\n";
	}
#endif
}

} /* end ns Maca */
