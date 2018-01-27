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
 * @file bs-file-backend.h
 * @brief File-based backend for an output bit stream.
 *
 * This backend is used for sequential output to a file. Bytes (each of them
 * comprising eight bits) are not actually written until the internal buffer
 * is filled. The buffer has fixed size, but can be halved and flushed to disk
 * at any given time.
 */

#ifndef POLIQARP_BS_FILE_BACKEND
#define POLIQARP_BS_FILE_BACKEND

#include <poliqarp-config.h>

#include <stdio.h>

#include <sakura/common/bitstream.h>

/**
 * The file backend.
 */
struct file_obs {
   size_t block_size;   /**< Size of the internal buffer. */
   size_t bits_used;    /**< Number of used bits in the buffer. */
   size_t blocks_written; /**< Number of blocks written so far. */
   uint8_t *data;       /**< The internal buffer. */
   FILE *file;          /**< File associated with this backend. */
};

/**
 * Creates a backend.
 * @param bs the structure to be filled.
 * @param block_size initial size of the buffer.
 * @param f file to be associated with this backend.
 *
 * @return 0 on success, -1 on error.
 */
int file_obs_create(struct file_obs *bs, size_t block_size, FILE *f);

/**
 * Associates a backend with the bitstream.
 * @param obs the bitstream to be initialized.
 * @param backend the backend.
 */
void file_obs_set_backend(struct obs *obs, struct file_obs *backend);

/**
 * Frees all resources allocated for a backend.
 * @param bs the backend to be destroyed.
 */
void file_obs_destroy(struct file_obs *bs);

/**
 * Flushes contents of a backend's internal buffer to the file associated
 * with it, emptying the buffer.
 *
 * @note If the number of bits in the buffer is > 0, the number of bytes
 * written is always equal to the size of the buffer, regardless of
 * whether the buffer was full or not.
 *
 * @param bs the backend to be flushed.
 *
 * @return 0 on success, -1 on error.
 *
 * @see file_obs_flush_partial
 */
int file_obs_flush(void *bs);

/**
 * Flushes contents of a backend's internal buffer to the file associated
 * with it, emptying the buffer.
 *
 * @note In contrast to file_obs_flush(), only the bits in the internal
 * buffer and at most 7 padding bits are written.
 *
 * @param bs the backend to be flushed.
 *
 * @return 0 on success, -1 on error.
 *
 * @see file_obs_flush
 */
int file_obs_flush_partial(void *bs);

/**
 * Cuts the size of the buffer by half. Can flush some data to a file
 * in case there were too much for the new size.
 * @param bs the backend to be shrunk.
 *
 * @return 0 on success, -1 on error.
 */
int file_obs_halve_block(struct file_obs *bs);

/**
 * Adds a single bit to this stream. Does not flush the buffer when it 
 * becomes full.
 * @note This function can be used as an implementation of #obs.add_bit.
 * @param bs The backend to write the bit to, cast to void *.
 * @param x Indicates whether to clear or set the bit.
 */
void file_obs_add_bit_noflush(void *bs, int x);

/**
 * Same as file_obs_add_bit_noflush(), except that this function flushes
 * the buffer when it becomes empty.
 * @note This function can be used as an implementation of #obs.add_bit
 * (and in fact is).
 * @param bs The backend to write the bit to, cast to void *.
 * @param x Indicates whether to clear or set the bit.
 *
 * @return 0 on success, -1 on error.
 */
int file_obs_add_bit(void *bs, int x);

#endif /* POLIQARP_BS_FILE_BACKEND */
