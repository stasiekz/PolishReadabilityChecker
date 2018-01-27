/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2009 by Instytut Podstaw Informatyki Polskiej
 * Akademii Nauk (IPI PAN; Institute of Computer Science, Polish
 * Academy of Sciences; cf. www.ipipan.waw.pl).  All rights reserved.
 * 
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file gpl.txt included in the packaging
 * of this file.  (See http://www.gnu.org/licenses/translations.html for
 * unofficial translations.)
 * 
 * A commercial license is available from IPI PAN (contact
 * Michal.Ciesiolka@ipipan.waw.pl or ipi@ipipan.waw.pl for more
 * information).  Licensees holding a valid commercial license from IPI
 * PAN may use this file in accordance with that license.
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.
 */

#ifndef SAKURA_SINGLE_THREADED
#include <pthread.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif
#include <sys/time.h>

#include "random.h"

/* The following code is based on the small noncryptographic pseudorandom
 * number generator code[1] by Bob Jenkins, which is in the public domain.
 * [1] http://burtleburtle.net/bob/rand/talksmall.html
 */

static inline uint32_t rot(uint32_t x, uint32_t k)
{
   return (x << k) | (x >> (32 - k));
}

uint32_t poliqarp_random(struct poliqarp_random_state *state)
{
   uint32_t e = state->a - rot(state->b, 27);
   state->a = state->b ^ rot(state->c, 17);
   state->b = state->c + state->d;
   state->c = state->d + e;
   state->d = e + state->a;
   return state->d;
}

void poliqarp_srandom(struct poliqarp_random_state *state, uint32_t seed)
{
   uint32_t i;
   state->a = 0xf1ea5eed;
   state->b = state->c = state->d = seed;
   for (i = 0; i < 20; ++i)
      poliqarp_random(state);
}

void poliqarp_srandom_time(struct poliqarp_random_state *state)
{
   uint32_t seed;
   struct timeval tv;
   gettimeofday(&tv, NULL);
   seed = 0;
   seed += (uint32_t) tv.tv_sec;
   seed += (uint32_t) tv.tv_usec;
#ifndef SAKURA_SINGLE_THREADED
#ifdef WIN32
   seed += (uint32_t) pthread_getw32threadhandle_np(pthread_self());
#else
   /* FIXME: IEEE Std 1003.1-2001/Cor 2-2004, item XBD/TC2/D6/26 is applied,
    * adding pthread_t to the list of types that are not required to be
    * arithmetic types, thus allowing pthread_t to be defined as a structure.
    * http://www.opengroup.org/onlinepubs/009695399/basedefs/sys/types.h.html#tag_13_67_10
    */
   seed += (uint32_t) pthread_self();
#endif
#else
   seed += (uint32_t) getpid();
#endif
   poliqarp_srandom(state, seed);
}
