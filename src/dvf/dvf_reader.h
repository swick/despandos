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

#ifndef __DVF_READER_H__
#define __DVF_READER_H__

struct dvf_file;
struct dvf_object;
struct dvf_animation;
struct dvf_frame;

struct dvf_file *
dvf_file_open(char *file_name, int *err);

int
dvf_file_close(struct dvf_file *file);

int
dvf_file_init(struct dvf_file *file);

int
dvf_file_cleanup(struct dvf_file *file);


unsigned int
dvf_file_num_objects(struct dvf_file *file);

struct dvf_object *
dvf_file_get_object(struct dvf_file *file, unsigned int index);

const char *
dvf_object_name(struct dvf_object *obj);

unsigned int
dvf_object_num_perspectives(struct dvf_object *obj);

void
dvf_object_size(struct dvf_object *obj,
                unsigned int *width,
                unsigned int *height);

unsigned int
dvf_object_num_animations(struct dvf_object *obj);

struct dvf_animation *
dvf_object_get_animation(struct dvf_object *obj, unsigned int index);

void
dvf_animation_unknown(struct dvf_animation *anim,
                      unsigned int *u0,
                      unsigned int *u1,
                      unsigned int *u2,
                      unsigned int *u3);

unsigned int
dvf_animation_id(struct dvf_animation *anim);

const char *
dvf_animation_name(struct dvf_animation *anim);

unsigned int
dvf_animation_perspective(struct dvf_animation *anim);

unsigned int
dvf_animation_num_frames(struct dvf_animation *anim);

struct dvf_frame *
dvf_animation_get_frame(struct dvf_animation *anim, unsigned int index);

void
dvf_frame_unknown(struct dvf_frame *frame,
                  unsigned int *u0,
                  unsigned int *u1,
                  unsigned int *u2,
                  unsigned int *u3,
                  unsigned int *u4,
                  unsigned int *u5);

void *
dvf_frame_pixmap(struct dvf_frame *frame,
                 unsigned int *width,
                 unsigned int *height);

#endif /* __DVF_READER_H__ */
