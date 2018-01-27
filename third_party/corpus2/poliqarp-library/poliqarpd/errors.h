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
 * Definitions of errors that can be returned by various functions. 
 */

#ifndef POLIQARPD_ERRORS_H
#define POLIQARPD_ERRORS_H

/* PE stands for Poliqarp Error */

#define PE_GENERIC     -1 /* Generic error */
#define PE_NOMEM       -2 /* Not enough memory */
#define PE_LIMITEXC    -3 /* Hard limit exceeded */
#define PE_INVSID      -4 /* Invalid session ID */
#define PE_SIDUSED     -5 /* Session ID already used */
#define PE_INVUID      -6 /* Invalid user ID */
#define PE_INVOPT      -7 /* Incorrect session option name */
#define PE_INVVAL      -8 /* Invalid session option value */

/* PPE stands for Poliqarp Protocol Error */

#define PPE_ARGCOUNT   1   /* Incorrect number of arguments */
#define PPE_NOSESSION  3   /* No session opened */
#define PPE_SESSBOUND  4   /* Cannot create a session for a connection that
                              is already bound */
#define PPE_NOMEM      5   /* Not enough memory */
#define PPE_INVSID     6   /* Invalid session ID */
#define PPE_SIDUSED    7   /* Session with this ID is already bound */
#define PPE_INVUID     8   /* Session user ID does not match the argument
                              of RECONNECT */
#define PPE_CORPUSALR  10  /* Session already has an open corpus */
#define PPE_SYSOPEN    12  /* System error while opening the corpus */
#define PPE_NOCORPUS   13  /* No corpus opened */
#define PPE_INVJID     14  /* Invalid job ID */
#define PPE_BUSY       15  /* A job is already in progress */
#define PPE_INVQUERY   16  /* Incorrect query */
#define PPE_INVRANGE   17  /* Invalid result range */
#define PPE_INVOPT     18  /* Incorrect session option */
#define PPE_INVVAL     19  /* Invalid session option value */
#define PPE_INVCRIT    20  /* Invalid sorting criteria */

#endif /* POLIQARPD_ERRORS_H */
