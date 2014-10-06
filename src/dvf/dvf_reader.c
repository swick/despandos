/*
 * Copyright (C) 2014 Sebastian Wick <sebastian@sebastianwick.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * *.dvf file format
 * =================
 *
 * struct dvf_file_header file_header;
 * struct dvf_file_sprites_header sprites_header;
 * for(sprites_header.num_sprites) {
 *   struct dvf_file_sprite_header sprite_header;
 *   binary sprite[sprite_header.size];
 * }
 * struct dvf_file_objects_header objects_header;
 * for(objects_header.num_objects) {
 *   struct dvf_file_object object;
 *   for(object.num_animations) {
 *     for(object.num_perspectives) {
 *       struct dvf_file_object_animation animation;
 *       for(animation.num_frames) {
 *         struct dvf_file_object_animation_frame frame;
 *       }
 *     }
 *   }
 * }
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

#include <dvf_reader.h>

#define __PACKED__ __attribute__ ((__packed__))

#define DEBUG 1
#if DEBUG
  #define DEBUG_LOG(...) do { fprintf(stdout,  __VA_ARGS__ ); } while(0)
  #define DEBUG_ERROR(...) do { fprintf(stderr,  "error: " __VA_ARGS__ ); } while(0)
#else
  #define DEBUG_LOG(...)
  #define DEBUG_ERROR(...)
#endif

/**
 * dvf file
 * read only
 */
struct dvf_file {
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
 * dvf sprite
 * - pixel format R8G8B8A8
 */
struct dvf_sprite {
    unsigned int width;
    unsigned int height;
    unsigned int size;
    void *data;
};

/**
 *
 */
struct dvf_object {
    
};

/**
 *
 */
struct dvf_container {
    struct dvf_sprite *sprites;
    unsigned int num_sprites;
    struct dvf_object *objects;
    unsigned int num_objects;
};

/**
 * dvf file header
 */
struct __PACKED__ dvf_file_header {
    /* first two bytes are probably a magic identifier and should be 00 02 */
    uint16_t magic;
};
#define DVF_FILE_MAGIC 512

/**
 * dvf sprites header
 */
struct __PACKED__ dvf_file_sprites_header {
    /* number of sprites in the file */
    uint32_t num_sprites;
    /*  */
    uint16_t max_width;
    /*  */
    uint16_t max_height;
    /* padding */
    uint8_t padding0[20];
};

/**
 * dvf file sprite header
 */
struct __PACKED__ dvf_file_sprite_header {
    /* size of sprite in bytes (= offset to next sprite) */
    uint32_t size;
    /* width of sprite */
    uint16_t width;
    /* height of sprite */
    uint16_t height;
    /* unknown, default: 01 00 */
    uint16_t unknown1;
};

/**
 * dvf file objects header
 */
struct __PACKED__ dvf_file_objects_header {
    /* number of objects */
    uint16_t num_objects;
};

/**
 * dvf file object
 */
struct __PACKED__ dvf_file_object {
    /* object name */
    uint8_t name[32];
    /* number of perspectives */
    uint16_t num_perspectives;
    /* padding */
    uint8_t padding0[32];
    /* number of amimations */
    uint16_t num_animations;
    /* padding */
    uint8_t padding1[16];
    /* max width of all frames, possibly simply the size of the object */
    uint16_t max_width;
    /* height */
    uint16_t max_height;
    /* unknown; 70 in most files, other known values are 0, 160, 180 */
    uint32_t unknown0;
    /* unknown; like unknown0 but 70 is 71 */
    uint32_t unknown1;
    /* padding */
    uint8_t padding2[20];
};

struct __PACKED__ dvf_file_object_animation {
    /* padding */
    uint8_t padding0[4];
    /* number of frames */
    uint16_t num_frames;
    /* unknown; usually nbrOfFrames-1 */
    uint16_t unknown0;
    /* unknown */
    uint16_t unknown1;
    /* same as unknown0 from struct dvf_file_object */
    uint32_t unknown2;
    /* same as unknown1 from struct dvf_file_object */
    uint32_t unknown3;
    /* perspective id (0-n) */
    uint16_t perspective_id;
    /* animation id (0-n) */
    uint16_t animation_id;
    /* animation name */
    uint8_t name[32];
};

struct __PACKED__ dvf_file_object_animation_frame {
    /* sprite id */
    uint16_t sprite_id;
    uint16_t unknown0;
    uint16_t unknown1;
    uint16_t unknown2;
    uint16_t unknown3;
    uint16_t unknown4;
    uint16_t unknown5;
};

/**
 * open a dvf file
 * 
 * returns a struct dvf_file on success, otherwise NULL and err gets set
 */
struct dvf_file *
dvf_file_open(char *file_name, int *err)
{
    *err = 0;
    struct dvf_file *file = malloc(sizeof(*file));
    if (!file) {
        DEBUG_ERROR("out of memory\n");
        *err = ENOMEM;
        return NULL;
    }
    memset(file, 0, sizeof(*file));

    file->fd = open(file_name, O_RDONLY);
    if (file->fd < 0) {
        DEBUG_ERROR("open failed: %s (%d)\n", strerror(errno), errno);
        *err = errno;
        return NULL;
    }

    struct stat file_stat;
    if (fstat(file->fd, &file_stat) < 0) {
        DEBUG_ERROR("fstat failed: %s (%d)\n", strerror(errno), errno);
        close(file->fd);
        *err = errno;
        return NULL;
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
        *err = errno;
        return NULL;
    }

    file->name = strdup(file_name);
    if (!file->name) {
        DEBUG_ERROR("out of memory\n");
        *err = ENOMEM;
        return NULL;
    }

    return file;
}

/**
 * close a dvf file
 * the file can be closed at any moment
 *
 * returns 0 on success
 */
int
dvf_file_close(struct dvf_file *file)
{
    assert(file &&
           file->name && 
           file->mapping != MAP_FAILED &&
           file->fd >= 0 &&
           file->size > 0);

    munmap(file->mapping, file->size);
    close(file->fd);
    free(file->name);
    free(file);

    return 0;
}

/**
 * create a dvf container from a file
 * 
 * returns a struct dvf_container on success, otherwise NULL and err gets set
 *
 * FIXME
 *   1. broken or invalid files should not raise a segfault
 *       -> check the mapping boundaries
 */
struct dvf_container *
dvf_container_create(struct dvf_file *file, int *err)
{
    *err = 0;
    int i = 0, j = 0, k = 0;
    unsigned long offset = 0;

    struct dvf_container *container = malloc(sizeof(*container));
    if (!container) {
        DEBUG_ERROR("out of memory\n");
        *err = ENOMEM;
        goto error;
    }
    memset(container, 0, sizeof(*container));

    struct dvf_file_header *file_header = file->mapping;
    offset += sizeof(*file_header);
    if (le16toh(file_header->magic) != DVF_FILE_MAGIC) {
        DEBUG_ERROR("file magic is %d, expected %d\n",
                    le16toh(file_header->magic),
                    DVF_FILE_MAGIC);
        *err = EILSEQ;
        goto error;
    }

    struct dvf_file_sprites_header *sprites_header = file->mapping + offset;
    offset += sizeof(*sprites_header);
    container->num_sprites = sprites_header->num_sprites;
    container->sprites = malloc(sizeof(*container->sprites) *
                         le32toh(sprites_header->num_sprites));
    if (!container->sprites) {
        DEBUG_ERROR("out of memory\n");
        *err = ENOMEM;
        goto error;
    }

    struct dvf_file_sprite_header *sprite = NULL;
    for (i=0; i<le32toh(sprites_header->num_sprites); i++) {
        sprite = file->mapping + offset;
        offset += sizeof(*sprite) + le32toh(sprite->size);

        //container->sprites[i] = dvf_sprite_create();
    }

    struct dvf_file_objects_header *objects_header = file->mapping + offset;
    offset += sizeof(*objects_header);

    struct dvf_file_object *object = NULL;
    for (i=0; i<le16toh(objects_header->num_objects); i++) {
        object = file->mapping + offset;
        offset += sizeof(*object);

        DEBUG_LOG("Object %s\n", object->name);
        DEBUG_LOG(" num_perspectives: %d\n", le16toh(object->num_perspectives));
        DEBUG_LOG(" num_animations:   %d\n", le16toh(object->num_animations));
        DEBUG_LOG(" width:            %d\n", le16toh(object->max_width));
        DEBUG_LOG(" height:           %d\n", le16toh(object->max_height));
        DEBUG_LOG(" unknown0:         %d\n", le32toh(object->unknown0));
        DEBUG_LOG(" unknown1:         %d\n", le32toh(object->unknown0));

        struct dvf_file_object_animation *animation = NULL;
        for(j=0; j<le16toh(object->num_animations) * le16toh(object->num_perspectives); j++) {
            animation = file->mapping + offset;
            offset += sizeof(*animation);

            DEBUG_LOG("  Animation %s\n", animation->name);
            DEBUG_LOG("    num_frames:     %d\n", le16toh(animation->num_frames));
            DEBUG_LOG("    perspective_id: %d\n", le16toh(animation->perspective_id));
            DEBUG_LOG("    animation_id:   %d\n", le16toh(animation->animation_id));
            DEBUG_LOG("    unknown0:       %d\n", le16toh(animation->unknown0));
            DEBUG_LOG("    unknown1:       %d\n", le16toh(animation->unknown1));
            DEBUG_LOG("    unknown2:       %d\n", le32toh(animation->unknown2));
            DEBUG_LOG("    unknown3:       %d\n", le32toh(animation->unknown3));

            struct dvf_file_object_animation_frame *frame = NULL;
            for(k=0; k<le16toh(animation->num_frames); k++) {
                frame = file->mapping + offset;
                offset += sizeof(*frame);

                DEBUG_LOG("    frame %d\n", k);
                DEBUG_LOG("      sprite_id: %d\n", le16toh(frame->sprite_id));
                DEBUG_LOG("      unknown0:  %d\n", le16toh(frame->unknown0));
                DEBUG_LOG("      unknown1:  %d\n", le16toh(frame->unknown1));
                DEBUG_LOG("      unknown2:  %d\n", le16toh(frame->unknown2));
                DEBUG_LOG("      unknown3:  %d\n", le16toh(frame->unknown3));
                DEBUG_LOG("      unknown4:  %d\n", le16toh(frame->unknown4));
                DEBUG_LOG("      unknown5:  %d\n", le16toh(frame->unknown5));
            }
        }
    }

    return container;

error:
    if (container)
        dvf_container_destroy(container);

    return NULL;
}

/**
 * destroys a dvf container
 * 
 * returns 0 on success
 */
int
dvf_container_destroy(struct dvf_container *container)
{
    if (!container)
        return 0;
    if (container->sprites)
        free(container->sprites);
    free(container);

    return 0;
}

