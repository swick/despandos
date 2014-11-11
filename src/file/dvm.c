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
 * *.dvm file format
 * =================
 * struct dvm_file_header file_header;
 * bzip2 bitmap[file_length]; // is it really file_length?
 *
 * bitmap
 * ======
 * pixel format: R6G5B6
 * size:         file_header.map_width*file_header.map_height*file_header.bpp
 */



#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <endian.h>

#include <bzlib.h>
#include <zlib.h>

#include "file.h"
#include "dvm.h"

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

__PACKED__ struct dvm_file_header {
    uint16_t map_width;
    uint16_t map_height;
    uint32_t bpp;
    uint32_t file_length;
};

__SYM_EXPORT__ void *
dvm_file_get_pixmap(const char *file_name,
                    unsigned int *width,
                    unsigned int *height,
                    int *err_out)
{
    int err = 0, decompress_res = 0;
    char *dest_buf = NULL;

    struct mmap_file *file = mmap_file_open(file_name, &err);
    if (!file) {
        DEBUG_ERROR("cannot opem file\n");
        goto error;
    }

    struct dvm_file_header *header = mmap_file_ptr_offset(file,
                                                          0,
                                                          sizeof(*header));
    if (!header) {
        DEBUG_ERROR("file is malformed\n");
        err = EILSEQ;
        goto error;
    }


    unsigned int map_width = le16toh(header->map_width);
    unsigned int map_height = le16toh(header->map_height);
    unsigned int map_bpp = 2; //e32toh(header->bpp);
    unsigned int map_size = le32toh(header->file_length);

    unsigned int dest_len = map_width * map_height * map_bpp;
    dest_buf = malloc(dest_len);
    if (!dest_buf) {
        DEBUG_ERROR("out of memory\n");
        err = ENOMEM;
        goto error;
    }

    unsigned int source_len = map_size;
    char *source_buf = mmap_file_ptr_offset(file, sizeof(*header), source_len);

    if (!source_buf) {
        DEBUG_ERROR("file is malformed\n");
        err = EILSEQ;
        goto error;
    }

    /* check for bzip2 magick */
    if (source_buf[0] == 0x42 && source_buf[1] == 0x5A) {
        decompress_res = BZ2_bzBuffToBuffDecompress(dest_buf,
                                                    &dest_len,
                                                    source_buf,
                                                    source_len,
                                                    0,
                                                    0);
        if (decompress_res != BZ_OK) {
            DEBUG_ERROR("decompression using bzip2 failed %d\n",
                        decompress_res);
            err = EILSEQ;
            goto error;
        }
    }
    /* check for zlib magick */
    else if (source_buf[0] == 0x78) {
        z_stream strm  = {0};
        strm.total_in  = strm.avail_in  = source_len;
        strm.total_out = strm.avail_out = dest_len;
        strm.next_in   = (Bytef *) source_buf;
        strm.next_out  = (Bytef *) dest_buf;

        strm.zalloc = Z_NULL;
        strm.zfree  = Z_NULL;
        strm.opaque = Z_NULL;

        decompress_res = inflateInit2(&strm, (15 + 32));
        if (decompress_res == Z_OK) {
            decompress_res = inflate(&strm, Z_FINISH);
            if (decompress_res != Z_STREAM_END) {
                DEBUG_ERROR("inflate failed: %d\n", decompress_res);
                inflateEnd(&strm);
                err = EILSEQ;
                goto error;
            }
        }
        else {
            inflateEnd(&strm);
            err = EILSEQ;
            goto error;
        }

        inflateEnd(&strm);
    }
    /* unknown compression */
    else {
        DEBUG_ERROR("unknown compression\n");
        err = EILSEQ;
        goto error;
    }

    *width = map_width;
    *height = map_height;
    return dest_buf;

error:
    if (file)
        mmap_file_close(file);
    if (dest_buf)
        free(dest_buf);
    if (err_out)
        *err_out = err;
    return NULL;
}

