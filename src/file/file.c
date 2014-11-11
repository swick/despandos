/*
 * Copyright (C) 2014 Sebastian Wick <sebastian@sebastianwick.net>
 *
 * This file is part of Despandos.
 *
 * Despandos is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Despandos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Despandos.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>
#include <endian.h>

#define DEBUG 0
#if DEBUG
  #define DEBUG_LOG(...) do { fprintf(stdout,  __VA_ARGS__ ); } while(0)
  #define DEBUG_ERROR(...) do { fprintf(stderr,  "error: " __VA_ARGS__ ); } while(0)
#else
  #define DEBUG_LOG(...)
  #define DEBUG_ERROR(...)
#endif

/**
 * memory mapped file
 */
struct mmap_file {
    /* file name */
    char *name;
    /* file size */
    off_t size;
    /* read only mmap mapping of the file */
    void *mapping;
    /* read only fd */ 
    int fd;
};

/**
 *
 */
void *
mmap_file_ptr_offset(struct mmap_file *file,
                      unsigned int offset,
                      unsigned int size)
{
    if (offset + size > file->size)
        return NULL;
    return (void *)((char *)file->mapping + offset);
}

/**
 *
 */
const char *
mmap_file_name(struct mmap_file *file)
{
    return file->name;
}

/**
 * mmap a file
 * 
 * returns a struct mmap_file on success, otherwise NULL and err gets set
 */
struct mmap_file *
mmap_file_open(const char *file_name, int *err_out)
{
    int err = 0;

    struct mmap_file *file = malloc(sizeof(*file));
    if (!file) {
        DEBUG_ERROR("out of memory\n");
        err = ENOMEM;
        goto error;
    }
    memset(file, 0, sizeof(*file));

    file->name = strdup(file_name);
    if (!file->name) {
        DEBUG_ERROR("out of memory\n");
        err = ENOMEM;
        goto error;
    }

    file->fd = open(file_name, O_RDONLY);
    if (file->fd < 0) {
        DEBUG_ERROR("open failed: %s (%d)\n", strerror(errno), errno);
        err = errno;
        goto error;
    }

    struct stat file_stat;
    if (fstat(file->fd, &file_stat) < 0) {
        DEBUG_ERROR("fstat failed: %s (%d)\n", strerror(errno), errno);
        close(file->fd);
        err = errno;
        goto error;
    }

    file->size = file_stat.st_size;
    file->mapping = mmap(NULL,
                         file_stat.st_size,
                         PROT_READ,
                         MAP_PRIVATE,
                         file->fd,
                         0);

    if (file->mapping == MAP_FAILED) {
        DEBUG_ERROR("mmaped failed: %s (%d)\n", strerror(errno), errno);
        close(file->fd);
        err = errno;
        goto error;
    }

    return file;

error:
    mmap_file_close(file);
    if (err_out)
        *err_out = err;
    return NULL;
}

/**
 * munmaps a file
 *
 * returns 0 on success
 */
int
mmap_file_close(struct mmap_file *file)
{
    if (!file)
        return 0;

    if (file->name)
        free(file->name);

    if (file->mapping) {
        assert(file->size > 0);
        munmap(file->mapping, file->size);
    }

    if(file->fd > 0)
        close(file->fd);

    free(file);

    return 0;
}

