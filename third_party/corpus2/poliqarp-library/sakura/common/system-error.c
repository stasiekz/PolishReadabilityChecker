/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2008-2009 by Instytut Podstaw Informatyki Polskiej
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

#ifdef _WIN32
#include <windows.h>
#include <winerror.h>
#endif
#include <errno.h>

void set_errno_from_last_error()
{
#ifdef _WIN32
   int last_error = GetLastError();
   switch (last_error)
   {
#define MAP(x, y) case ERROR_##x: errno  = y; break;
      MAP(ACCESS_DENIED, EACCES)
      MAP(ALREADY_EXISTS, EEXIST)
      MAP(BAD_DEVICE, ENODEV)
      MAP(DIRECTORY, ENOTDIR)
      MAP(DISK_FULL, ENOSPC)
      MAP(FILE_EXISTS, EEXIST)
      MAP(FILE_INVALID, ENXIO)
      MAP(FILE_NOT_FOUND, ENOENT)
      MAP(INVALID_HANDLE, EBADF)
      MAP(NOACCESS, EFAULT)
      MAP(NOT_ENOUGH_MEMORY, ENOMEM)
      MAP(OPEN_FAILED, EIO)
      MAP(OUTOFMEMORY, ENOMEM)
      MAP(PATH_NOT_FOUND, ENOENT)
      MAP(SHARING_VIOLATION, EBUSY)
      MAP(TOO_MANY_OPEN_FILES, EMFILE)
      MAP(WRITE_PROTECT, EROFS)
      MAP(DISK_CORRUPT, EIO)
      MAP(FILE_CORRUPT, EIO)
      MAP(HANDLE_DISK_FULL, ENOSPC)
      MAP(INVALID_NAME, ENOENT)
      default:
         errno = 0;
         return;
   }
#undef MAP
#else
   errno = 0;
   return;
#endif
}
