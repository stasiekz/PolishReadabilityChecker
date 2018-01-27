#This file has been taken from libgd. Its license goes:

#Portions copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001,
#2002 by Cold Spring Harbor Laboratory. Funded under Grant
#P41-RR02188 by the National Institutes of Health.
#
#Portions copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002 by
#Boutell.Com, Inc.
#
#Portions relating to GD2 format copyright 1999, 2000, 2001, 2002
#Philip Warner.
#
#Portions relating to PNG copyright 1999, 2000, 2001, 2002 Greg
#Roelofs.
#
#Portions relating to gdttf.c copyright 1999, 2000, 2001, 2002 John
#Ellson (ellson@lucent.com).
#
#Portions relating to gdft.c copyright 2001, 2002 John Ellson
#(ellson@lucent.com).
#
#Portions copyright 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007
#Pierre-Alain Joye (pierre@libgd.org).
#
#Portions relating to JPEG and to color quantization copyright 2000,
#2001, 2002, Doug Becker and copyright (C) 1994, 1995, 1996, 1997,
#1998, 1999, 2000, 2001, 2002, Thomas G. Lane. This software is
#based in part on the work of the Independent JPEG Group. See the
#file README-JPEG.TXT for more information.
#
#Portions relating to WBMP copyright 2000, 2001, 2002 Maurice
#Szmurlo and Johan Van den Brande.
#
#Permission has been granted to copy, distribute and modify gd in
#any context without fee, including a commercial application,
#provided that this notice is present in user-accessible supporting
#documentation.
#
#This does not affect your ownership of the derived work itself, and
#the intent is to assure proper credit for the authors of gd, not to
#interfere with your productive use of gd. If you have questions,
#ask. "Derived works" includes all programs that utilize the
#library. Credit must be given in user-accessible documentation.
#
#This software is provided "AS IS." The copyright holders disclaim
#all warranties, either express or implied, including but not
#limited to implied warranties of merchantability and fitness for a
#particular purpose, with respect to this code and accompanying
#documentation.
#
#Although their code does not appear in gd, the authors wish to thank
#David Koblas, David Rowley, and Hutchison Avenue Software Corporation
#for their prior contributions.

message(STATUS "Checking whether system has ANSI C header files")
INCLUDE (CheckPrototypeExists)
INCLUDE (CheckIncludeFiles)

check_include_files("dlfcn.h;stdint.h;stddef.h;inttypes.h;stdlib.h;strings.h;string.h;float.h" StandardHeadersExist)
if(StandardHeadersExist)
	check_prototype_exists(memchr string.h memchrExists)
	if(memchrExists)

		check_prototype_exists(free stdlib.h freeExists)
		if(freeExists)
			message(STATUS "ANSI C header files - found")
			set(STDC_HEADERS 1 CACHE INTERNAL "System has ANSI C header files")
			set(HAVE_STRINGS_H 1)
			set(HAVE_STRING_H 1)
			set(HAVE_FLOAT_H 1)
			set(HAVE_STDLIB_H 1)
			set(HAVE_STDDEF_H 1)
			set(HAVE_STDINT_H 1)
			set(HAVE_INTTYPES_H 1)
			set(HAVE_DLFCN_H 1)
		endif(freeExists)
	endif(memchrExists)
endif(StandardHeadersExist)

if(NOT STDC_HEADERS)
	message(STATUS "ANSI C header files - not found")
	set(STDC_HEADERS 0 CACHE INTERNAL "System has ANSI C header files")
endif(NOT STDC_HEADERS)


