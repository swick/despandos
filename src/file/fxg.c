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
 * *.fxg file format
 * =================
 */



#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <endian.h>

#include "file.h"
#include "fxg.h"

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

__PACKED__ struct fxg_file_header {
    uint8_t  magic[4];
    uint32_t unknown0;
    uint32_t unknown1;
    uint32_t unknown2;
    uint32_t unknown3;
    uint32_t unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
};

__PACKED__ struct fxg_file_entry {
    uint32_t type;
    uint16_t id;
    uint8_t  padding[4];
};

__PACKED__ struct fxg_file_entry_d1 {
    uint16_t unknown0;
    uint16_t unknown1;
    uint16_t unknown2;
};

__PACKED__ struct fxg_file_entry_d2 {
    uint16_t length;
    /* uint8_t name[length] */
};

