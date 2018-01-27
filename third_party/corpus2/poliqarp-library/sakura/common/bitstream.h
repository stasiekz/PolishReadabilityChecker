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

/**
 * @file bitstream.h
 * @brief Input/output streams of bits.
 *
 * The streams declared herein are implemented in an object-oriented way.
 * A stream object consists of a backend (which can be just about anything)
 * and a function taking that backend as an argument. This allows for 
 * polymorphic implementation of functions that encode integers as sequences
 * of bits.
 */

#ifndef POLIQARP_BITSTREAM_H
#define POLIQARP_BITSTREAM_H

#include <poliqarp-config.h>

/** 
 * Output bitstream.
 */
struct obs {
   void *data;                   /**< The underlying backend. */
   int (*add_bit)(void *, int);  /**< Emits a single bit to the stream. */
};

/**
 * Input bitstream.
 */
struct ibs {
   void *data;              /**< The underlying backend. */
   int (*next_bit)(void *); /**< Retrieves next bit from the stream.
                                 Returns a non-zero value iff it is 1. */
};

/** 
 * Encodes a number in unary code.
 * @param bs the bitstream to use.
 * @param x the number to encode.
 */
void encode_unary(struct obs *bs, uint32_t x);

/** 
 * Encodes a number in binary code.
 * @param bs the bitstream to use.
 * @param x the number to encode.
 * @param bits number of bits to use. 
 */
void encode_binary(struct obs *bs, uint32_t x, int bits);

/** 
 * Encodes a number in gamma code.
 * @param bs the bitstream to use.
 * @param x the number to encode.
 */
void encode_gamma(struct obs *bs, uint32_t x);

/** 
 * Encodes a number in delta code.
 * @param bs the bitstream to use.
 * @param x the number to encode.
 */
void encode_delta(struct obs *bs, uint32_t x);

/** 
 * Encodes a number in Golomb code.
 * @param bs the bitstream to use.
 * @param x the number to encode.
 * @param b parameter of the Golomb code.
 */
void encode_golomb(struct obs *bs, uint32_t x, uint32_t b);

/** 
 * Reads an unary-coded number from the stream.
 * @param bs the bitstream to use.
 * @return the decoded number.
 */
uint32_t decode_unary(struct ibs *bs);

/** 
 * Reads a binary-coded number from the stream.
 * @param bs the bitstream to use.
 * @param bits number of bits that the number consists of.
 * @return the decoded number.
 */
uint32_t decode_binary(struct ibs *bs, int bits);

/** 
 * Reads a gamma-coded number from the stream.
 * @param bs the bitstream to use.
 * @return the decoded number.
 */
uint32_t decode_gamma(struct ibs *bs);

/** 
 * Reads a delta-coded number from the stream.
 * @param bs the bitstream to use.
 * @return the decoded number.
 */
uint32_t decode_delta(struct ibs *bs);

uint32_t get_golomb_parameter(uint32_t n, uint32_t m);
/**
 * Calculates the parameter for the Glomb code.
 * @ return an approximation of ceil(ln(2) * n / m).
 */

/** 
 * Reads a Golomb-coded number from the stream.
 * @param bs the bitstream to use.
 * @param b parameter of the Golomb code.
 * @return the decoded number.
 */
uint32_t decode_golomb(struct ibs *bs, uint32_t b);

/**
 * Returns ceil(log2(x)), where log2(x) is the binary (base 2) logarithm
 * of x.
 */
static inline int clog2(uint32_t x)
{
   int res = 0;
   x--;
   while (x) x >>= 1, res++;
   return res;
}

/**
 * Returns floor(log2(x)) with log2 declared as above.
 */
static inline int flog2(uint32_t x)
{
   return clog2(x + 1) - 1;
}

#endif /* POLIQARP_BITSTREAM_H */
