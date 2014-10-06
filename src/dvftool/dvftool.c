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

#include <stdio.h>
#include <errno.h>

#include "dvf_reader.h"

int
main(int argc, char **argv)
{
    int err = 0;
    char *file_name = NULL;
    struct dvf_file *file = NULL;

    if(argc < 2) {
        fprintf(stderr, "missing paramater\n");
        return EINVAL;
    }
    file_name = argv[1];

    file = dvf_file_open(file_name, &err);
    if (!file) {
        fprintf(stderr,
                "error: cannot open file %s: %s (%d)",
                file_name,
                strerror(err),
                err);
        return err;
    }

    struct dvf_container *container = dvf_container_create(file, &err);
    if (!container) {
        fprintf(stderr,
                "error: cannot init dvf container: %s (%d)\n",
                strerror(err),
                err);
        dvf_file_close(file);
        return err;
    }

    dvf_container_destroy(container);
    dvf_file_close(file);

    return 0;
}
