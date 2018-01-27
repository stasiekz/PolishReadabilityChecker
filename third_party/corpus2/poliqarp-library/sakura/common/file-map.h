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
 * @file file-map.h
 * @brief Cross-platform memory mapping routines.
 *
 * This file defines structures and functions that provide a consistent wrapper
 * API around mmap (Linux) or MapViewOfFileEx (Win32).
 */

#ifndef FILE_MAP_H
#define FILE_MAP_H

#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

/**
 * Portable mmap replacement/wrapper.
 * Allows to map a file if such a function is provided by the OS.
 */
struct file_map {
#ifdef _WIN32
   HANDLE hfile;   /**< Windows file handle.      */
   HANDLE hmap;    /**< Windows file map handle.  */
#endif
   void *image;    /**< File image.               */
   size_t size;    /**< File size.                */
   int fd;         /**< File descriptor.          */
   int cleanup;    /**< Cleanup mode.             */
};

/**
 * Initialize file map so that it's safe to destroy it.
 * It doesn't contain anything, any method other than destroy will produce
 * undefined behaviour.
 */
void init_file_map(struct file_map *this);

/**
 * Create read-only view of a file.
 * The file has to exist and be readable.
 *
 * @return 0 on success, some undefined error code on failure.
 */
int create_file_map(struct file_map *this, const char *pathname);

/**
 * Destroy file map.
 */
void destroy_file_map(struct file_map *this);

/**
 * Return pointer to contents of mapped file.
 */
const void *get_file_image(const struct file_map *this);

/**
 * Return the size of mapped file.
 */
size_t get_file_size(const struct file_map *this);

#endif /* FILE_MAP_H */
