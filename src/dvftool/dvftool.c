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

#include <stdio.h>
#include <errno.h>
#include <SDL.h>

#include "dvf.h"

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
                "error: cannot open file %s: %s (%d)\n",
                file_name,
                strerror(err),
                err);
        return err;
    }

    if ((err = dvf_file_init(file))) {
        fprintf(stderr,
                "error: cannot init file: %s (%d)\n",
                strerror(err),
                err);
        dvf_file_close(file);
        return err;
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *w = SDL_CreateWindow("DVF tool",
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     512,
                                     512,
                                     0);
    SDL_Renderer *renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);


    int i=0, j=0, k=0;
    struct dvf_object *obj = NULL;
    int num_objects = dvf_file_num_objects(file);

    for (i=0; i<num_objects; i++) {
        obj = dvf_file_get_object(file, i);

        SDL_Rect sprite_rect, screen_rect, obj_rect;

        int obj_width, obj_height;
        dvf_object_size(obj, &obj_width, &obj_height);
        //SDL_SetWindowSize(w, obj_width, obj_height);
        SDL_SetWindowTitle(w, dvf_object_name(obj));

        screen_rect.x = 0;
        screen_rect.y = 0;
        screen_rect.w = 512; //obj_width;
        screen_rect.h = 512; //obj_height;

        obj_rect.x = 0;
        obj_rect.y = 0;
        obj_rect.w = obj_width; //obj_width;
        obj_rect.h = obj_height; //obj_height;

        struct dvf_animation *anim = NULL;
        for (j=0; j<dvf_object_num_animations(obj); j++) {
            anim = dvf_object_get_animation(obj, j);;

            struct dvf_frame *frame = NULL;
            unsigned int width, height;
            void *pixmap;
            for (k=0; k<dvf_animation_num_frames(anim); k++) {
                frame = dvf_animation_get_frame(anim, k);
                    pixmap = dvf_frame_pixmap(frame, &width, &height);

                int wf, hf;
                dvf_frame_unknown(frame, NULL, NULL, &wf, &hf, NULL, NULL);

                SDL_Surface *surface =
                  SDL_CreateRGBSurface(0,
                                       width,
                                       height,
                                       32,
                                       0x00ff0000,
                                       0x0000ff00,
                                       0x000000ff,
                                       0xff000000);
                surface->pixels = pixmap;

                SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,
                                                                surface);
                SDL_FreeSurface(surface);

                sprite_rect.x = wf;
                sprite_rect.y = hf;
                sprite_rect.w = width;
                sprite_rect.h = height;

                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 205, 235, 255, 255);
                SDL_RenderFillRect(renderer, &screen_rect);
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &obj_rect);
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderDrawRect(renderer, &sprite_rect);
                SDL_RenderCopy(renderer, tex, NULL, &sprite_rect);
                SDL_RenderPresent(renderer);

                SDL_DestroyTexture(tex);

                SDL_Delay(50);

                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        goto quit;
                    }
                }
            }

        }
    }

quit:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(w);
    SDL_Quit();

    dvf_file_cleanup(file);
    dvf_file_close(file);

    return 0;
}
