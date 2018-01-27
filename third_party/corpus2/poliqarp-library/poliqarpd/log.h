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
 * Public interface of the logging subsystem.
 */

#ifndef POLIQARPD_LOG_H
#define POLIQARPD_LOG_H

#include <foostring/foostring.h>
#include <stdarg.h>

#include "configuration.h"

/** 
 * Initializes the logging subsystem. 
 * @return 0 upon successful initialization.
 * @return -1 if an error occurred. 
 */
int init_log();

/** 
 * Deallocates all resources associated with the logging subsystem. 
 */
void done_log();

/** 
 * Writes a given message to the log. 
 */
void log_entry(const char *fmt, ...);

#endif /* POLIQARPD_LOG_H */
