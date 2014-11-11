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
#include <assert.h>
#include <stdint.h>
#include <endian.h>

#include "file.h"
#include "dvf.h"

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

#define DVF_SPRITE_TYPE_AT_OFFSET(SPRITE, TYPE, OFFSET) \
    (*((TYPE *)((char *)SPRITE + OFFSET)))

struct dvf_frame {
    struct dvf_file_object_animation_frame *frame;
    struct dvf_file_sprite_header *sprite;
};

struct dvf_animation {
    struct dvf_file_object_animation *animation;
    unsigned int num_frames;
    struct dvf_frame **frames;
};

struct dvf_object {
    struct dvf_file_object *object;
    unsigned int num_animations;
    struct dvf_animation **animations;
};

/**
 * dvf file
 * read only
 */
struct dvf_file {
    /* */
    struct mmap_file *file;
    /* objects lookup table */
    unsigned int num_objects;
    struct dvf_object **objects;
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

__SYM_EXPORT__ unsigned int
dvf_file_num_objects(struct dvf_file *file)
{
    assert(file);
    return file->num_objects;
}

__SYM_EXPORT__ struct dvf_object *
dvf_file_get_object(struct dvf_file *file, unsigned int index)
{
    assert(index >= 0 && index < file->num_objects);
    return file->objects[index];
}

/**
 * returns a reference to the name of the object
 * the reference becomes invalid when dvf_file_cleanup gets called
 */
__SYM_EXPORT__ const char *
dvf_object_name(struct dvf_object *obj)
{
    return obj->object->name;
}

/**
 * returns the number of perspectives the object can be displayed in
 */
__SYM_EXPORT__ unsigned int
dvf_object_num_perspectives(struct dvf_object *obj)
{
    return obj->object->num_perspectives;
}

/**
 * returns the number of perspectives the object can be displayed in
 */
__SYM_EXPORT__ void
dvf_object_size(struct dvf_object *obj,
                unsigned int *width,
                unsigned int *height)
{
    *width = obj->object->max_width;
    *height = obj->object->max_height;
}

__SYM_EXPORT__ unsigned int
dvf_object_num_animations(struct dvf_object *obj)
{
    assert(obj);
    return obj->num_animations;
}

__SYM_EXPORT__ struct dvf_animation *
dvf_object_get_animation(struct dvf_object *obj, unsigned int index)
{
    assert(index >= 0 && index < obj->num_animations);
    return obj->animations[index];
}

__SYM_EXPORT__ void
dvf_animation_unknown(struct dvf_animation *anim,
                      unsigned int *u0,
                      unsigned int *u1,
                      unsigned int *u2,
                      unsigned int *u3)
{
    if(u0)
        *u0 = anim->animation->unknown0;
    if(u1)
        *u1 = anim->animation->unknown1;
    if(u2)
        *u2 = anim->animation->unknown2;
    if(u3)
        *u3 = anim->animation->unknown3;
}

/**
 * returns a reference to the name of the animation
 * the reference becomes invalid when dvf_file_cleanup gets called
 */
__SYM_EXPORT__ unsigned int
dvf_animation_id(struct dvf_animation *anim)
{
    return anim->animation->animation_id;
}

/**
 * returns a reference to the name of the animation
 */
__SYM_EXPORT__ const char *
dvf_animation_name(struct dvf_animation *anim)
{
    return anim->animation->name;
}

/**
 * returns the perspective of the animation
 */
__SYM_EXPORT__ unsigned int
dvf_animation_perspective(struct dvf_animation *anim)
{
    return anim->animation->perspective_id;
}

__SYM_EXPORT__ unsigned int
dvf_animation_num_frames(struct dvf_animation *anim)
{
    assert(anim);
    return anim->num_frames;
}

__SYM_EXPORT__ struct dvf_frame *
dvf_animation_get_frame(struct dvf_animation *anim, unsigned int index)
{
    assert(index >= 0 && index < anim->num_frames);
    return anim->frames[index];
}

__SYM_EXPORT__ void
dvf_frame_unknown(struct dvf_frame *frame, 
                     unsigned int *u0,
                     unsigned int *u1,
                     unsigned int *u2,
                     unsigned int *u3,
                     unsigned int *u4,
                     unsigned int *u5)
{
    if(u0)
        *u0 = frame->frame->unknown0;
    if(u1)
        *u1 = frame->frame->unknown1;
    if(u2)
        *u2 = frame->frame->unknown2;
    if(u3)
        *u3 = frame->frame->unknown3;
    if(u4)
        *u4 = frame->frame->unknown4;
    if(u5)
        *u5 = frame->frame->unknown5;
}

/**
 * returns a B8G8R8A8 pixmap of the frame or NULL if an error occured
 * the caller has to free the buffer
 */
__SYM_EXPORT__ void *
dvf_frame_pixmap(struct dvf_frame *frame,
                 unsigned int *width,
                 unsigned int *height)
{
    struct dvf_file_sprite_header *sprite = frame->sprite;

    uint8_t *image = malloc(le16toh(sprite->width) *
                            le16toh(sprite->height) * 4);

    if (!image)
        return NULL;

    unsigned int offset = sizeof(*sprite), i = 0, j = 0;
    int16_t num_transparent_pixels;
    int16_t num_total_pixels;
    uint16_t color;

    for(i=0; i<le16toh(sprite->height); i++) {
        num_transparent_pixels = DVF_SPRITE_TYPE_AT_OFFSET(sprite,
                                                           int16_t,
                                                           offset);
        offset += 2;
        num_total_pixels = DVF_SPRITE_TYPE_AT_OFFSET(sprite,
                                                     int16_t,
                                                     offset) + 1;
        offset += 2;

        /* if num_total_pixels equals -1, the complete line is transparent */
        if (num_total_pixels == -1) {
            for (j=0; j<le16toh(sprite->width); j++) {
                image[le16toh(sprite->width) * i * 4 + j * 4 + 0] = 0;
                image[le16toh(sprite->width) * i * 4 + j * 4 + 1] = 0;
                image[le16toh(sprite->width) * i * 4 + j * 4 + 2] = 0;
                image[le16toh(sprite->width) * i * 4 + j * 4 + 3] = 0;
            }
        }
        else {
            for (j=0; j<num_transparent_pixels; j++) {
                image[le16toh(sprite->width) * i * 4 + j * 4 + 0] = 0;
                image[le16toh(sprite->width) * i * 4 + j * 4 + 1] = 0;
                image[le16toh(sprite->width) * i * 4 + j * 4 + 2] = 0;
                image[le16toh(sprite->width) * i * 4 + j * 4 + 3] = 0;
            }
            for (j=0; j < num_total_pixels - num_transparent_pixels; j++) {
                color = DVF_SPRITE_TYPE_AT_OFFSET(sprite, uint16_t, offset);
                offset += 2;
                if (color == 0x1f) {
                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 0] = 0;
                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 1] = 0;
                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 2] = 0;
                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 3] = 127;
                } else if (color == 0x7C0) {
                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 0] = 0;
                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 1] = 0;
                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 2] = 0;
                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 3] = 0;
                } else {
                    int bits[4];
                    bits[2] = ((color) % 16);
                    bits[3] = ((color/0x10) % 16);
                    bits[0] = ((color/0x100) % 16);
                    bits[1] = ((color/0x1000) % 16);

                    int colormap[3];
                    colormap[0] = bits[0] > 7 ? 1 : 0;
	                colormap[0] += bits[1] * 2;
	                colormap[1] = (bits[0] % 8) * 8;
	                colormap[1] += bits[3]/2;
	                colormap[2] = bits[3] % 2 == 1 ? bits[2] + 16 : bits[2];
	                colormap[0] = colormap[0] * 8 + (colormap[0]/4);
	                colormap[1] = colormap[1] * 4 + (colormap[1]/16);
	                colormap[2] = colormap[2] * 8 + (colormap[2]/4);

                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 0] = colormap[2];
                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 1] = colormap[1];
                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 2] = colormap[0];
                    image[le16toh(sprite->width) * i * 4 + num_transparent_pixels * 4 + j * 4 + 3] = 255;
                }
            }
            for (j=0; j < le16toh(sprite->width) - num_total_pixels; j++) {
                image[le16toh(sprite->width) * i * 4 + num_total_pixels * 4 + j * 4 + 0] = 0;
                image[le16toh(sprite->width) * i * 4 + num_total_pixels * 4 + j * 4 + 1] = 0;
                image[le16toh(sprite->width) * i * 4 + num_total_pixels * 4 + j * 4 + 2] = 0;
                image[le16toh(sprite->width) * i * 4 + num_total_pixels * 4 + j * 4 + 3] = 0;
            }
        }
    }

    *width = le16toh(sprite->width);
    *height = le16toh(sprite->height);

    return image;
}

__SYM_EXPORT__ int
dvf_file_init(struct dvf_file *file)
{
    int err = 0;
    int i = 0, j = 0, k = 0;
    unsigned long offset = 0;

    struct dvf_file_header *file_header =
      mmap_file_ptr_offset(file->file, 0, sizeof(*file_header));
    if (!file_header) {
        DEBUG_ERROR("file is malformed\n");
        err = EILSEQ;
        goto error;
    }
    offset += sizeof(*file_header);

    if (le16toh(file_header->magic) != DVF_FILE_MAGIC) {
        DEBUG_ERROR("file magic is %d, expected %d\n",
                    le16toh(file_header->magic),
                    DVF_FILE_MAGIC);
        err = EILSEQ;
        goto error;
    }

    struct dvf_file_sprites_header *sprites_header =
      mmap_file_ptr_offset(file->file, offset, sizeof(*sprites_header));
    if (!file_header) {
        DEBUG_ERROR("file is malformed\n");
        err = EILSEQ;
        goto error;
    }
    offset += sizeof(*sprites_header);

    {
    int num_sprites = le32toh(sprites_header->num_sprites);
    struct dvf_file_sprite_header *sprites[num_sprites];

    struct dvf_file_sprite_header *sprite = NULL;
    for (i=0; i<le32toh(sprites_header->num_sprites); i++) {
        sprite = mmap_file_ptr_offset(file->file, offset, sizeof(*sprite));
        if (!sprite) {
            DEBUG_ERROR("file is malformed\n");
            err = EILSEQ;
            goto error;
        }
        offset += sizeof(*sprite) + le32toh(sprite->size);

        sprites[i] = sprite;
    }

    struct dvf_file_objects_header *objects_header =
      mmap_file_ptr_offset(file->file, offset, sizeof(*objects_header));
    if (!objects_header) {
        DEBUG_ERROR("file is malformed\n");
        err = EILSEQ;
        goto error;
    }
    offset += sizeof(*objects_header);

    file->num_objects = le16toh(objects_header->num_objects);
    file->objects = malloc(sizeof(*file->objects) * file->num_objects);
    if (!file->objects) {
        DEBUG_ERROR("out of memory\n");
        err = ENOMEM;
        goto error;
    }
    memset(file->objects, 0, sizeof(*file->objects));

    struct dvf_file_object *object = NULL;
    for (i=0; i<le16toh(objects_header->num_objects); i++) {
        object = mmap_file_ptr_offset(file->file, offset, sizeof(*object));
        if (!object) {
            DEBUG_ERROR("file is malformed\n");
            err = EILSEQ;
            goto error;
        }
        offset += sizeof(*object);

        struct dvf_object *obj = malloc(sizeof(*obj));
        if (!obj) {
            DEBUG_ERROR("out of memory\n");
            err = ENOMEM;
            goto error;
        }
        memset(obj, 0, sizeof(*obj));

        file->objects[i] = obj;
        obj->object = object;
        obj->num_animations = le16toh(object->num_animations) *
                              le16toh(object->num_perspectives);
        obj->animations = malloc(sizeof(*obj->animations) *
                                 obj->num_animations);
        if (!obj->animations) {
            DEBUG_ERROR("out of memory\n");
            err = ENOMEM;
            goto error;
        }

        DEBUG_LOG("Object %s\n", object->name);
        DEBUG_LOG(" num_perspectives: %d\n", le16toh(object->num_perspectives));
        DEBUG_LOG(" num_animations:   %d\n", le16toh(object->num_animations));
        DEBUG_LOG(" width:            %d\n", le16toh(object->max_width));
        DEBUG_LOG(" height:           %d\n", le16toh(object->max_height));
        DEBUG_LOG(" unknown0:         %d\n", le32toh(object->unknown0));
        DEBUG_LOG(" unknown1:         %d\n", le32toh(object->unknown0));

        struct dvf_file_object_animation *animation = NULL;
        for(j=0; j<le16toh(object->num_animations) * le16toh(object->num_perspectives); j++) {
            animation = mmap_file_ptr_offset(file->file,
                                             offset,
                                             sizeof(*animation));
            if (!animation) {
                DEBUG_ERROR("file is malformed\n");
                err = EILSEQ;
                goto error;
            }
            offset += sizeof(*animation);

            struct dvf_animation *anim = malloc(sizeof(*anim));
            if (!anim) {
                DEBUG_ERROR("out of memory\n");
                err = ENOMEM;
                goto error;
            }
            memset(anim, 0, sizeof(*anim));

            obj->animations[j] = anim;
            anim->animation = animation;
            anim->num_frames = le16toh(animation->num_frames);
            anim->frames = malloc(sizeof(*anim->frames) * anim->num_frames);
            if (!anim->frames) {
                DEBUG_ERROR("out of memory\n");
                err = ENOMEM;
                goto error;
            }

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
                frame = mmap_file_ptr_offset(file->file,
                                             offset,
                                             sizeof(*frame));
                if (!frame) {
                    DEBUG_ERROR("file is malformed\n");
                    err = EILSEQ;
                    goto error;
                }
                offset += sizeof(*frame);

                struct dvf_frame *dvf_frame = malloc(sizeof(*dvf_frame));
                if (!dvf_frame) {
                    DEBUG_ERROR("out of memory\n");
                    err = ENOMEM;
                    goto error;
                }
                memset(dvf_frame, 0, sizeof(*dvf_frame));

                anim->frames[k] = dvf_frame;
                dvf_frame->frame = frame;
                if (le16toh(frame->sprite_id) >= num_sprites ||
                    le16toh(frame->sprite_id) < 0)
                {
                    DEBUG_ERROR("file is malformed\n");
                    err = EILSEQ;
                    goto error;
                }
                dvf_frame->sprite = sprites[le16toh(frame->sprite_id)];

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
    }

    return 0;

error:
    dvf_file_cleanup(file);
    return err;
}

__SYM_EXPORT__ int
dvf_file_cleanup(struct dvf_file *file)
{
    if (!file || !file->objects)
        return 0;

    int i = 0, j = 0, k = 0;

    struct dvf_object *object;
    struct dvf_animation *animation;
    struct dvf_frame *frame;

    for (i=0; i < file->num_objects; i++) {
        object = file->objects[i];
        if (!object)
            break;

        for (j=0; j < object->num_animations; j++) {
            animation = object->animations[j];
            if (!animation)
                break;

            for (k=0; k < animation->num_frames; k++) {
                frame = animation->frames[k];
                if (!frame)
                    break;

                free(frame);
            }

            free(animation->frames);
            free(animation);
        }

        free(object->animations);
        free(object);
    }

    free(file->objects);
}

/**
 * open a dvf file
 * 
 * returns a struct dvf_file on success, otherwise NULL and err gets set
 */
__SYM_EXPORT__ struct dvf_file *
dvf_file_open(char *file_name, int *err_out)
{
    int err = 0;
    struct dvf_file *file = malloc(sizeof(*file));
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
        dvf_file_close(file);
    if (err_out)
        *err_out = err;
    return NULL;
}

/**
 * close a dvf file
 * the file can be closed at any moment
 *
 * returns 0 on success
 */
__SYM_EXPORT__ int
dvf_file_close(struct dvf_file *file)
{
    if (!file)
        return 0;

    if (file->file)
        mmap_file_close(file->file);

    free(file);

    return 0;
}


