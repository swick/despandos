/*
 * Copyright (C) 2014 Sebastian Wick <sebastian@sebastianwick.net>
 *
 * This file is part of despandos.
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

#ifndef __DVF_READER_H__
#define __DVF_READER_H__

struct dvf_file;
struct dvf_container;

struct dvf_file *
dvf_file_open(char *file_name, int *err);

int
dvf_file_close(struct dvf_file *file);

struct dvf_container *
dvf_container_create(struct dvf_file *file, int *err);

int
dvf_container_destroy(struct dvf_container *container);

#endif /* __DVF_READER_H__ */
