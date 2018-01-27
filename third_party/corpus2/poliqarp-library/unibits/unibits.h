/*
 * This header file combines bits of Tcl's ``tcl.h``, ``tclInt.h`` and
 * ``tclDecl.h`` files.
 *
 * Copyright (c) 1987-1994 The Regents of the University of California.
 * Copyright (c) 1993-1997 Lucent Technologies.
 * Copyright (c) 1994-1998 Sun Microsystems, Inc.
 * Copyright (c) 1998-2000 by Scriptics Corporation.
 * Copyright (c) 2001-2002 by Kevin B. Kenny.  All rights reserved.
 * Copyright (c) 2007 Daniel A. Steffen <das@users.sourceforge.net>
 *
 * See the file ``doc/COPYING.Tcl``  for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifndef UNIBITS_UNIBITS_H
#define UNIBITS_UNIBITS_H

#include <unibits/unibits-extra.h>

/*
 * The structure defined below is used to hold dynamic strings. The only
 * fields that clients should use are string and length, accessible via the
 * macros Tcl_DStringValue and Tcl_DStringLength.
 */

#define TCL_DSTRING_STATIC_SIZE 200
typedef struct Tcl_DString {
    char *string;		/* Points to beginning of string: either
				 * staticSpace below or a malloced array. */
    int length;			/* Number of non-NULL characters in the
				 * string. */
    int spaceAvl;		/* Total number of bytes available for the
				 * string and its terminating NULL char. */
    char staticSpace[TCL_DSTRING_STATIC_SIZE];
				/* Space to use in common case where string is
				 * small. */
} Tcl_DString;

#define Tcl_DStringLength(dsPtr) ((dsPtr)->length)
#define Tcl_DStringValue(dsPtr) ((dsPtr)->string)

/*
 *----------------------------------------------------------------
 * Macro used by the Tcl core get a unicode char from a utf string. It checks
 * to see if we have a one-byte utf char before calling the real
 * Tcl_UtfToUniChar, as this will save a lot of time for primarily ascii
 * string handling. The macro's expression result is 1 for the 1-byte case or
 * the result of Tcl_UtfToUniChar. The ANSI C "prototype" for this macro is:
 *
 * MODULE_SCOPE int     TclUtfToUniChar(const char *string, Tcl_UniChar *ch);
 *----------------------------------------------------------------
 */

#define TclUtfToUniChar(str, chPtr) \
        ((((unsigned char) *(str)) < 0xC0) ? \
            ((*(chPtr) = (Tcl_UniChar) *(str)), 1) \
            : Tcl_UtfToUniChar(str, chPtr))


EXTERN int              Tcl_UniCharIsAlnum (int ch);
EXTERN int              Tcl_UniCharIsAlpha (int ch);
EXTERN int              Tcl_UniCharIsDigit (int ch);
EXTERN int              Tcl_UniCharIsLower (int ch);
EXTERN int              Tcl_UniCharIsSpace (int ch);
EXTERN int              Tcl_UniCharIsUpper (int ch);
EXTERN Tcl_UniChar      Tcl_UniCharToLower (int ch);
EXTERN Tcl_UniChar      Tcl_UniCharToTitle (int ch);
EXTERN Tcl_UniChar      Tcl_UniCharToUpper (int ch);

EXTERN char *		Tcl_UniCharToUtfDString (CONST Tcl_UniChar * uniStr, 
				int uniLength, Tcl_DString * dsPtr);
EXTERN Tcl_UniChar *    Tcl_UtfToUniCharDString (CONST char * src,
                                int length, Tcl_DString * dsPtr);

EXTERN void             Tcl_DStringFree (Tcl_DString * dsPtr);
EXTERN void             Tcl_DStringInit (Tcl_DString * dsPtr);
EXTERN void             Tcl_DStringSetLength (Tcl_DString * dsPtr,
                                int length);
EXTERN int              Tcl_UtfToLower (char * src);

#endif
