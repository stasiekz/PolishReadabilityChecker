/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2004-2009 by Instytut Podstaw Informatyki Polskiej
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

#include <poliqarp-config.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sakura/common/bitstream.h>

void encode_unary(struct obs *bs, uint32_t x)
{
   while (--x)
      bs->add_bit(bs->data, 1);
   bs->add_bit(bs->data, 0);
}

uint32_t decode_unary(struct ibs *bs)
{
   int x = 1;
   while (bs->next_bit(bs->data))
      x++;
   return x;
}

void encode_binary(struct obs *bs, uint32_t x, int bits)
{
   int i;
   for (i = 0; i < bits; i++) {
      bs->add_bit(bs->data, x & (1 << (bits - 1)));
      x <<= 1;
   }
}

uint32_t decode_binary(struct ibs *bs, int bits)
{
   uint32_t res = 0;
   while (bits--)
      if (bs->next_bit(bs->data))
         res += (1 << bits);
   return res;
}

void encode_gamma(struct obs *bs, uint32_t x)
{
   int nb = flog2(x);
   encode_unary(bs, 1 + nb);
   encode_binary(bs, x - (1 << nb), nb);
}

uint32_t decode_gamma(struct ibs *bs)
{
   int nb = decode_unary(bs) - 1;
   uint32_t res = decode_binary(bs, nb);
   return (1 << nb) + res;
}

void encode_delta(struct obs *bs, uint32_t x)
{
   int nb = flog2(x);
   encode_gamma(bs, 1 + nb);
   encode_binary(bs, x - (1 << nb), nb);
}

uint32_t decode_delta(struct ibs *bs)
{
   int nb = decode_gamma(bs) - 1;
   uint32_t res = decode_binary(bs, nb);
   return (1 << nb) + res;
}

uint32_t get_golomb_parameter(uint32_t n, uint32_t m)
{
   uint64_t ln2_shift32 = 2977044472UL; /* log(2.0) << 32 */
   uint64_t m_shift32 = ((uint64_t)m << 32) - 1;
   uint32_t b = ((ln2_shift32 * n + m_shift32) / m) >> 32;
   return b;
}

void encode_golomb(struct obs *bs, uint32_t x, uint32_t b)
{
   int q = (x - 1) / b, r = x - q * b - 1, cl = clog2(b), limit = (1 << cl) - b;
   encode_unary(bs, q + 1);
   if (r < limit)
      encode_binary(bs, r, cl - 1);
   else
      encode_binary(bs, r + limit, cl);
}

uint32_t decode_golomb(struct ibs *bs, uint32_t b)
{
   int q = decode_unary(bs) - 1, cl, limit, r;
   if (b == 1)
      return q + 1;
   cl = clog2(b);
   limit = (1 << cl) - b;
   r = decode_binary(bs, cl - 1);
   if (r >= limit) {
      r = 2 * r - limit;
      if (bs->next_bit(bs->data))
         r++;
   }
   return q * b + r + 1;
}


