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

/* FIXME: Way too many #ifdefs in here! */

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#if defined _POSIX_MAPPED_FILES
#include <sys/mman.h>
#endif
#include <sakura/common/file-map.h>
#include <sakura/common/system-error.h>

/** Cleanup mode used by destroy_file_map. */
enum file_map_cleanup_mode {
   CLEANUP_NONE,
   CLEANUP_MMAP,
   CLEANUP_WINMAP
};

void init_file_map(struct file_map *this)
{
   this->cleanup = CLEANUP_NONE;
}

static int prepare_mapped_file(struct file_map *this, const char *pathname)
{
#if defined _WIN32
   this->hfile = CreateFile(pathname, GENERIC_READ, FILE_SHARE_READ, NULL, 
      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if (this->hfile == INVALID_HANDLE_VALUE)
   {
      set_errno_from_last_error();
      goto err;
   }
   this->size = GetFileSize(this->hfile, NULL);
   return 0;
#elif defined _POSIX_MAPPED_FILES
   struct stat stat_data;
   int fd;
   int flags = O_RDONLY;

   if ((fd = open(pathname, flags)) < 0)
      goto err;
   if (fstat(fd, &stat_data) != 0)
      goto err_stat;
   this->size = stat_data.st_size;
   this->fd = fd;
   return 0;

err_stat:
   close(fd);
#endif
err:
   return -1;
}

static int file_map__mmap(struct file_map *this)
{
#ifndef _WIN32
#if defined _POSIX_MAPPED_FILES
   void *image;

   image = mmap(NULL, this->size, PROT_READ, MAP_SHARED, this->fd, 0L);
   if (image == MAP_FAILED)
      goto err;

   this->image = image;
   this->cleanup = CLEANUP_MMAP;
   return 0;

   /* cleanup code */

err:
   this->cleanup = CLEANUP_NONE;
#else
#warning "Not using memory mapping interface of any kind!"
#endif
#endif
   return -1;
}

static int file_map__windows(struct file_map *this, const char *pathname)
{
#ifdef _WIN32
   HANDLE hmap;
   void *image;

   hmap = CreateFileMapping(this->hfile, NULL, PAGE_READONLY, 0, this->size, 
      NULL);
   if (!hmap)
   {
      set_errno_from_last_error();
      goto err_file;
   }

   image = MapViewOfFileEx(hmap, FILE_MAP_READ, (DWORD) 0, (DWORD) 0, 0, NULL);
   if (!image)
   {
      set_errno_from_last_error();
      goto err_file_map;
   }

   this->hmap = hmap;
   this->image = image;
   this->cleanup = CLEANUP_WINMAP;
   return 0;

   /* cleanup code */

err_file_map:
   CloseHandle(hmap);
err_file:
   CloseHandle(this->hfile);
   this->cleanup = CLEANUP_NONE;
#endif
   return -1;
}

int create_file_map(struct file_map *this, const char *pathname)
{
   if (prepare_mapped_file(this, pathname) != 0)
      goto err;
   if (file_map__mmap(this) == 0 || file_map__windows(this, pathname) == 0)
      return 0;
#ifdef _POSIX_MAPPED_FILES
   {
      int my_errno;
      my_errno = errno;
      close(this->fd);
      errno = my_errno;
   }
#endif
err:
   this->image = NULL;
   this->size = 0;
   return -1;
}

void destroy_file_map(struct file_map *this)
{
   if (this->cleanup == CLEANUP_NONE)
      return;
   switch (this->cleanup) {
   case CLEANUP_MMAP:
#ifdef _POSIX_MAPPED_FILES
      munmap(this->image, this->size);
      close(this->fd);
#endif
      break;
   case CLEANUP_WINMAP:
#ifdef _WIN32
      UnmapViewOfFile(this->image);
      CloseHandle(this->hmap);
      CloseHandle(this->hfile);
#endif
      break;
   default:
      abort(); /* Should not happen. */
      break;
   }
   this->cleanup = CLEANUP_NONE;
}

const void *get_file_image(const struct file_map *this)
{
   return this->image;
}

size_t get_file_size(const struct file_map * this)
{
   return this->size;
}
