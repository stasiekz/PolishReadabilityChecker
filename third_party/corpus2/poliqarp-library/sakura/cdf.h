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
 * @file cdf.h
 * @brief Corpus Description File -- reading, parsing, writing.
 *
 * A corpus description file (CDF in short; this is also the preferred
 * extension for such files) contains information about the binary corpus 
 * that should not be changed by the user, such as presence and granularity 
 * of indices. Just like the .cfg file, it consists of lines of the form
 * 'name = value'.
 */ 

#ifndef POLIQARP_CDF_H
#define POLIQARP_CDF_H

#include <stdlib.h>

/**
 * Endianness of an architecture.
 */
enum poliqarp_endianness {
   ENDIAN_LE, /**< Little endian. */
   ENDIAN_BE  /**< Big endian.    */
};

/**
 * Information contained in CDF files. 
 */
struct poliqarp_cdf_info {
   int version;                /**< Version of binary format. */
   int indices;                /**< Inverted indices built for this corpus. */
   size_t granularity;         /**< Granularity of the indices. */
   enum poliqarp_endianness endianness; /**< Endianness of the architecture that this
                                             corpus was built on. */
};

/**
 * Creates a CDF structure with the current corpus format.
 * @param cdf The destination structure.
 */
void poliqarp_cdf_create(struct poliqarp_cdf_info *cdf);

/**
 * Reads and parses a CDF file. If some fields are not given, assumes some
 * sensible defaults.
 * @param name Base name of the corpus.
 * @param cdf The destination structure.
 * @return 0 if the file was successfully read, -1 otherwise.
 * @note Even if -1 is returned, the destination structure will contain 
 * sensible default when this function returns.
 */
int poliqarp_cdf_read(const char *name, struct poliqarp_cdf_info *cdf);

/**
 * Writes a structure to a CDF file.
 * @param name Base name of the corpus.
 * @param cdf Structure to be written.
 * @return 0 if the structure was successfully written, -1 otherwise.
 */
int poliqarp_cdf_write(const char *name, const struct poliqarp_cdf_info *cdf);
   
#endif /* CDF_H */
