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

#ifndef __DVD_FILE_H__
#define __DVD_FILE_H__

#include <stdint.h>

#define DVD_ENTRY_TYPE(A, B, C, D) \
    (D << 24 | C << 16 | B << 8 | A << 0)
/**
 *
 */
#define DVD_ENTRY_TYPE_UNKN 0
#define DVD_ENTRY_TYPE_MISC DVD_ENTRY_TYPE('M','I','S','C')
#define DVD_ENTRY_TYPE_BGND DVD_ENTRY_TYPE('B','G','N','D')
#define DVD_ENTRY_TYPE_MOVE DVD_ENTRY_TYPE('M','O','V','E')
#define DVD_ENTRY_TYPE_SGHT DVD_ENTRY_TYPE('S','G','H','T')
#define DVD_ENTRY_TYPE_MASK DVD_ENTRY_TYPE('M','A','S','K')
#define DVD_ENTRY_TYPE_WAYS DVD_ENTRY_TYPE('W','A','Y','S')
#define DVD_ENTRY_TYPE_ELEM DVD_ENTRY_TYPE('E','L','E','M')
#define DVD_ENTRY_TYPE_FXBK DVD_ENTRY_TYPE('F','X','B','K')
#define DVD_ENTRY_TYPE_MSIC DVD_ENTRY_TYPE('M','S','I','C')
#define DVD_ENTRY_TYPE_SND  DVD_ENTRY_TYPE('S','N','D',' ')
#define DVD_ENTRY_TYPE_PAT  DVD_ENTRY_TYPE('P','A','T',' ')
#define DVD_ENTRY_TYPE_BOND DVD_ENTRY_TYPE('B','O','N','D')
#define DVD_ENTRY_TYPE_MAT  DVD_ENTRY_TYPE('M','A','T',' ')
#define DVD_ENTRY_TYPE_LIFT DVD_ENTRY_TYPE('L','I','F','T')
#define DVD_ENTRY_TYPE_AI   DVD_ENTRY_TYPE('A','I',' ',' ')
#define DVD_ENTRY_TYPE_BUIL DVD_ENTRY_TYPE('B','U','I','L')
#define DVD_ENTRY_TYPE_SCRP DVD_ENTRY_TYPE('S','C','R','P')
#define DVD_ENTRY_TYPE_JUMP DVD_ENTRY_TYPE('J','U','M','P')
#define DVD_ENTRY_TYPE_CART DVD_ENTRY_TYPE('C','A','R','T')
#define DVD_ENTRY_TYPE_DLGS DVD_ENTRY_TYPE('D','L','G','S')

struct dvd_entry_unknown {
    uint32_t type;
};

struct dvd_entry_misc {
    uint32_t type;
    int version;
    int size;
};

struct dvd_entry_bgnd {
    uint32_t type;
};

struct dvd_entry_move {
    uint32_t type;
};

struct dvd_entry_sght {
    uint32_t type;
};

struct dvd_entry_mask {
    uint32_t type;
};

struct dvd_entry_ways {
    uint32_t type;
};

struct dvd_entry_elem {
    uint32_t type;
};

struct dvd_entry_fxbk {
    uint32_t type;
};

struct dvd_entry_msic {
    uint32_t type;
};

struct dvd_entry_snd {
    uint32_t type;
};

struct dvd_entry_pat {
    uint32_t type;
};

struct dvd_entry_bond {
    uint32_t type;
};

struct dvd_entry_mat {
    uint32_t type;
};

struct dvd_entry_lift {
    uint32_t type;
};

struct dvd_entry_ai {
    uint32_t type;
};

struct dvd_entry_buil {
    uint32_t type;
};

struct dvd_entry_scrp {
    uint32_t type;
};

struct dvd_entry_jump {
    uint32_t type;
};

struct dvd_entry_cart {
    uint32_t type;
};

struct dvd_entry_dlgs {
    uint32_t type;
};

union dvd_entry {
    uint32_t type;
    struct dvd_entry_unknown unknown;
    struct dvd_entry_misc misc;
    struct dvd_entry_move move;
};

struct dvd_file;

int
dvd_file_has_next(struct dvd_file *file);

int
dvd_file_get_next(struct dvd_file *file,
                  union dvd_entry *entry);

void
dvd_entry_done(union dvd_entry *entry);

struct dvd_file *
dvd_file_open(char *file_name, int *err_out);

int
dvd_file_close(struct dvd_file *file);

#endif /* __DVD_FILE_H__ */
