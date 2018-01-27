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

#ifndef POLIQARP_CONFIG_H
#define POLIQARP_CONFIG_H 1

#include <poliqarp-config.h>

#include <stdio.h>

#include <sakura/exception.h>

/** 
 * Read one line, trim whitespace and comments.
 * @return a string allocated by malloc(), or NULL in case of error or EOF
 */
char *poliqarp_fetch_line(FILE *stream);

/** @todo */
struct poliqarp_variable {
   char *name;
   char *value;
};

/**
 * Parse a name = value pair.
 * @return 0 on sucess, -1 on failure
 * */
int poliqarp_parse_variable(struct poliqarp_variable *this, char *text);

/** @todo */
void poliqarp_free_variable(struct poliqarp_variable *this);

typedef int (*poliqarp_config_handler)(void *extra, int section,
   char *text, struct poliqarp_error *error);

struct poliqarp_config_section {
   const char *name;
   int id;
};

int poliqarp_parse_config_file(void *extra, const char *path,
   const struct poliqarp_config_section *sections,
   poliqarp_config_handler handler, struct poliqarp_error *error);

#endif /* POLIQARP_CONFIG_H */
