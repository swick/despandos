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

#ifndef __FILE_FILE_H__
#define __FILE_FILE_H__

struct mmap_file;

struct mmap_file *
mmap_file_open(const char *file_name, int *err_out);

int
mmap_file_close(struct mmap_file *file);

const char *
mmap_file_name(struct mmap_file *file);

void *
mmap_file_ptr_offset(struct mmap_file *file,
                     unsigned int offset,
                     unsigned int size);

#endif /* __FILE_FILE_H__ */
