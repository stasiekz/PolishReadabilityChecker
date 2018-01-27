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
 * @file getline.h
 * @brief getline() replacement for systems that lack it, taken from
 * uClibc.
 */

#ifndef GETLINE_H
#define GETLINE_H

#include <poliqarp-config.h>

#include <sys/types.h>
#include <stdio.h>

/**
 * getdelim() with delimiter hardwired to '\n'.
 *
 * @see getdelim()
 */
ssize_t getline(char **linebuf, size_t *n, FILE *file);

/** 
 * Reads up to (and including) a TERMINATOR from STREAM into *LINEPTR
 * (and null-terminate it). *LINEPTR is a pointer returned from malloc (or
 * NULL), pointing to *N characters of space.  It is realloc'd as
 * necessary.
 * 
 * @return the number of characters read (not including the null delimiter), 
 * or -1 on error or EOF.  
 */ 
ssize_t getdelim(char **linebuf, size_t *linebufsz, int delimiter, FILE *file);

#endif /* GETLINE_H */
