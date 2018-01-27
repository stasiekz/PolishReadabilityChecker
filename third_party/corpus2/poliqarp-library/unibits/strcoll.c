/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2009 by Instytut Podstaw Informatyki Polskiej
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

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

static inline size_t unibits_mbstowcs(wchar_t *dest, const char *src, size_t n)
{
#ifdef _WIN32
   int rc;
   if (dest == NULL)
      n = 0;
   else if (n == 0)
      return 0;
   rc = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, src, -1, dest, n);
   if (rc == 0) {
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
         errno = ENOMEM;
      else
         errno = EILSEQ;
   }
   return (size_t)(rc - 1);
#else
   return mbstowcs(dest, src, n);
#endif
}

int unibits_validate_utf8(void)
{
   if (
      unibits_mbstowcs(NULL, "a", 0) == 1 &&
      unibits_mbstowcs(NULL, "\xce\xb1" /* GREEK SMALL LETTER ALPHA */, 0) == 1 &&
      unibits_mbstowcs(NULL, "\xe0\xa4\x85" /* DEVANAGARI LETTER A */, 0) == 1
      /* No check for anything beyond BMP, as Windows does not support that. */
   )
      /* The condition is expected to be true for UTF-8 encoding *only*. */
      return 0;
   if (errno == EILSEQ)
      return 1;
   else
      return -1;
}

static inline wchar_t* unibits_wcsrev(wchar_t *w)
{
#ifdef _WIN32
   return wcsrev(w);
#else
   wchar_t *end = wcsrchr(w, L'\0');
   if (end == w)
      return w;
   end--;
   while (w < end) {
      wchar_t tmp = *w;
      *w = *end;
      *end = tmp;
      w++;
      end--;
   }
   return w;
#endif
}

static inline int unibits_wcsicoll(wchar_t *w1, wchar_t *w2)
{
#ifdef _WIN32
   return wcsicoll(w1, w2);
#else
   size_t i;
   for (i = 0; w1[i] != L'\0'; i++)
      w1[i] = towlower(w1[i]);
   for (i = 0; w2[i] != L'\0'; i++)
      w2[i] = towlower(w2[i]);
   return wcscoll(w1, w2);
#endif
}

static inline int unibits_strcoll(const char *s1, const char *s2, bool atergo, bool case_sensitive)
{
   wchar_t *w1 = NULL, *w2 = NULL;
   size_t n1 = unibits_mbstowcs(NULL, s1,  0) + 1;
   size_t n2 = unibits_mbstowcs(NULL, s2,  0) + 1;
   if (n1 == 0 || n2 == 0)
      goto error;
   w1 = malloc(n1 * sizeof (wchar_t));
   if (w1 == NULL)
      goto error;
   w2 = malloc(n2 * sizeof (wchar_t));
   if (w2 == NULL)
      goto error;
   if (unibits_mbstowcs(w1, s1, n1) + 1 != n1)
      goto error;
   if (unibits_mbstowcs(w2, s2, n2) + 1 != n2)
      goto error;
   if (atergo) {
      unibits_wcsrev(w1);
      unibits_wcsrev(w2);
   }
   int rc;
   if (case_sensitive)
      rc = wcscoll(w1, w2);
   else
      rc = unibits_wcsicoll(w1, w2);
   free(w1);
   free(w2);
   return rc;
error:
   free(w1);
   free(w2);
   return INT_MIN;
}

int unibits_afronte_strcoll(const char *s1, const char *s2)
{
   return unibits_strcoll(s1, s2, false, true);
}

int unibits_afronte_stricoll(const char *s1, const char *s2)
{
   return unibits_strcoll(s1, s2, false, false);
}

int unibits_atergo_strcoll(const char *s1, const char *s2)
{
   return unibits_strcoll(s1, s2, true, true);
}

int unibits_atergo_stricoll(const char *s1, const char *s2)
{
   return unibits_strcoll(s1, s2, true, false);
}
