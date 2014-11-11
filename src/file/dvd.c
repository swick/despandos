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

/*
 * *.dvd file format
 * =================
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <endian.h>

#include "file.h"
#include "dvd.h"

#define DEBUG 1
#if DEBUG
  #define DEBUG_LOG(...) do { fprintf(stdout,  __VA_ARGS__ ); } while(0)
  #define DEBUG_ERROR(...) do { fprintf(stderr,  "error: " __VA_ARGS__ ); } while(0)
#else
  #define DEBUG_LOG(...)
  #define DEBUG_ERROR(...)
#endif

#define __SYM_EXPORT__ __attribute__ ((visibility ("default")))
#define __PACKED__ __attribute__ ((__packed__))


/**
 * dvf file
 * read only
 */
struct dvd_file {
    /* file */
    struct mmap_file *file;
    /* current offset */
    unsigned int offset;
};

/**
 * dvd entry header
 */
struct __PACKED__ dvd_entry_header {
    /* entry type */
    uint8_t type[4];
    /* entry size */
    uint32_t size;
};

/**
 * dvd MISC header
 */
struct __PACKED__ dvd_misc_header {
    /* version */
    uint32_t version;
};

/**
 * dvd BGND header
 */
struct __PACKED__ dvd_bgnd_header {
    /* version */
    uint32_t version;
    /* name size */
    uint16_t name_size;
    /* followed by the name */
};

/**
 * dvd BGND header part 2, probably the same as dvm files
 * continues after the name
 */
struct __PACKED__ dvd_bgnd_header_p2 {
    uint16_t map_width;
    uint16_t map_height;
    uint32_t bpp;
    uint32_t file_length;
};


/**
 * 
 */
int
dvd_entry_unknown_init(struct dvd_file *file,
                       struct dvd_entry_header *header,
                       struct dvd_entry_unknown *unknown)
{
    unknown->type = DVD_ENTRY_TYPE_UNKN;
    return 0;
}

/**
 * 
 */
int
dvd_entry_misc_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_misc *misc)
{
    misc->type = DVD_ENTRY_TYPE_MISC;
    misc->size = header->size;
    return 0;
}

/**
 * 
 */
int
dvd_entry_bgnd_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_bgnd *bgnd)
{
    bgnd->type = DVD_ENTRY_TYPE_BGND;
    return 0;
}

/**
 * 
 */
int
dvd_entry_move_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_move *move)
{
    move->type = DVD_ENTRY_TYPE_MOVE;

    unsigned int offs = 0;
    uint32_t *version = mmap_file_ptr_offset(file->file, file->offset + offs, sizeof(*version));
    printf("move version %d\n", *version);

    return 0;
}

/**
 * 
 */
int
dvd_entry_sght_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_sght *sght)
{
    sght->type = DVD_ENTRY_TYPE_SGHT;
    return 0;
}

/**
 * 
 */
int
dvd_entry_mask_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_mask *mask)
{
    mask->type = DVD_ENTRY_TYPE_MASK;
    return 0;
}

/**
 * 
 */
int
dvd_entry_ways_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_ways *ways)
{
    ways->type = DVD_ENTRY_TYPE_WAYS;
    return 0;
}

/**
 * 
 */
int
dvd_entry_elem_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_elem *elem)
{
    elem->type = DVD_ENTRY_TYPE_ELEM;
    return 0;
}

/**
 * 
 */
int
dvd_entry_fxbk_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_fxbk *fxbk)
{
    fxbk->type = DVD_ENTRY_TYPE_FXBK;
    return 0;
}

/**
 * 
 */
int
dvd_entry_msic_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_msic *msic)
{
    msic->type = DVD_ENTRY_TYPE_MSIC;
    return 0;
}

/**
 * 
 */
int
dvd_entry_snd_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_snd *snd)
{
    snd->type = DVD_ENTRY_TYPE_SND;
    return 0;
}

/**
 * 
 */
int
dvd_entry_pat_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_pat *pat)
{
    pat->type = DVD_ENTRY_TYPE_PAT;
    return 0;
}

/**
 * 
 */
int
dvd_entry_bond_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_bond *bond)
{
    bond->type = DVD_ENTRY_TYPE_BOND;
    return 0;
}

/**
 * 
 */
int
dvd_entry_mat_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_mat *mat)
{
    mat->type = DVD_ENTRY_TYPE_MAT;
    return 0;
}

/**
 * 
 */
int
dvd_entry_lift_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_lift *lift)
{
    lift->type = DVD_ENTRY_TYPE_LIFT;
    return 0;
}

/**
 * 
 */
int
dvd_entry_ai_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_ai *ai)
{
    ai->type = DVD_ENTRY_TYPE_AI;
    return 0;
}

/**
 * 
 */
int
dvd_entry_buil_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_buil *buil)
{
    buil->type = DVD_ENTRY_TYPE_BUIL;
    return 0;
}

/**
 * 
 */
int
dvd_entry_scrp_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_scrp *scrp)
{
    scrp->type = DVD_ENTRY_TYPE_SCRP;
    return 0;
}

/**
 * 
 */
int
dvd_entry_jump_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_jump *jump)
{
    jump->type = DVD_ENTRY_TYPE_JUMP;
    return 0;
}

/**
 * 
 */
int
dvd_entry_cart_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_cart *cart)
{
    cart->type = DVD_ENTRY_TYPE_CART;
    return 0;
}

/**
 * 
 */
int
dvd_entry_dlgs_init(struct dvd_file *file,
                    struct dvd_entry_header *header,
                    struct dvd_entry_dlgs *dlgs)
{
    dlgs->type = DVD_ENTRY_TYPE_DLGS;
    return 0;
}

/**
 * 
 */
int
dvd_entry_misc_cleanup(struct dvd_entry_misc *misc)
{
    return 0;
}

/**
 * 
 */
__SYM_EXPORT__ void
dvd_entry_done(union dvd_entry *entry)
{
    switch(entry->type) {
        case DVD_ENTRY_TYPE_MISC:
            dvd_entry_misc_cleanup(&entry->misc);
            break;
    }
}

/**
 * 
 */
__SYM_EXPORT__ int
dvd_file_get_next(struct dvd_file *file,
                  union dvd_entry *entry)
{
    int err = 0;

    struct dvd_entry_header *header =
      mmap_file_ptr_offset(file->file, file->offset, sizeof(*header));

    if (!header) {
        DEBUG_ERROR("file is malformed\n");
        err = EILSEQ;
        goto exit;
    }

    uint32_t type = DVD_ENTRY_TYPE(header->type[0],
                                   header->type[1],
                                   header->type[2],
                                   header->type[3]);
    switch (type) {
        case DVD_ENTRY_TYPE_MISC:
            err = dvd_entry_misc_init(file, header, &entry->misc);
            break;
        case DVD_ENTRY_TYPE_MOVE:
            err = dvd_entry_move_init(file, header, &entry->move);
            break;
        default:
            err = dvd_entry_unknown_init(file, header, &entry->unknown);
            break;
    }

    file->offset += header->size + sizeof(*header);

exit:
    return err;
}

/**
 * returns 1 if the file contains at least another entry, 0 otherwise
 */
__SYM_EXPORT__ int
dvd_file_has_next(struct dvd_file *file)
{
    struct dvd_entry_header *entry =
      mmap_file_ptr_offset(file->file,
                           file->offset,
                           sizeof(*entry));
    return entry != NULL;
}

/**
 * open a dvd file
 * 
 * returns a struct dvd_file on success, otherwise NULL and err gets set
 */
__SYM_EXPORT__ struct dvd_file *
dvd_file_open(char *file_name, int *err_out)
{
    int err = 0;
    struct dvd_file *file = malloc(sizeof(*file));
    if (!file) {
        DEBUG_ERROR("out of memory\n");
        err = ENOMEM;
        goto error;
    }
    memset(file, 0, sizeof(*file));

    file->file = mmap_file_open(file_name, &err);
    if (!file->file) {
        DEBUG_ERROR("cannot open file %s: %s (%d)\n",
                    file_name,
                    strerror(err),
                    err);
        goto error;
    }

    return file;

error:
    if (file)
        dvd_file_close(file);
    if (err_out)
        *err_out = err;
    return NULL;
}

/**
 * close a dvd file
 * the file can be closed at any moment
 *
 * returns 0 on success
 */
__SYM_EXPORT__ int
dvd_file_close(struct dvd_file *file)
{
    if (!file)
        return 0;

    if (file->file)
        mmap_file_close(file->file);

    free(file);

    return 0;
}


