
#include <stdio.h>
#include <SDL.h>
#include <dvm.h>
#include <dvd.h>

int
main(int argc, char **argv)
{
    if(argc < 2)
        return 1;

    char dvd_filename[MAX_PATH];
    char dvm_filename[MAX_PATH];
    snprintf(dvd_filename, MAX_PATH-1, "%s.dvd", argv[1]);
    snprintf(dvm_filename, MAX_PATH-1, "%s.dvm", argv[1]);

    int err = 0;
    struct dvd_file *file = dvd_file_open(dvd_filename, &err);
    if (!file) {
        fprintf(stderr, "dvd_file_open failed\n");
        return -1;
    }

    union dvd_entry entry;
    while (dvd_file_has_next(file)) {
        if ((err = dvd_file_get_next(file, &entry))) {
            fprintf(stderr, "dvd_file_get_next failed\n");
            return -2;
        }

        switch (entry.type) {
            case DVD_ENTRY_TYPE_MISC:
                printf("misc entry: size %d\n", entry.misc.size);
                break;
            case DVD_ENTRY_TYPE_UNKN:
            default:
                break;
        }

        dvd_entry_done(&entry);
    }

    dvd_file_close(file);

    return 0;

    unsigned int width, height;
    void *pixmap = dvm_file_get_pixmap(dvm_filename, &width, &height, NULL);

    if (!pixmap) {
        fprintf(stderr, "failed getting pixmap\n");
        return -1;
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *w = SDL_CreateWindow("DVF tool",
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     -1,
                                     -1,
                                     SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Renderer *renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface *surface = SDL_CreateRGBSurface(0,
                           width,
                           height,
                           16, 0, 0, 0, 0);
    surface->pixels = pixmap;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,
                                                    surface);
    SDL_FreeSurface(surface);

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = width;
    rect.h = height;

    int move = 0, mouse_x, mouse_y, diff_x = 0, diff_y = 0;
    int offset_x, offset_y;
    SDL_Event event;
    while (1) {
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                goto exit;
            }
            switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                move = 1;
                mouse_x = event.button.x;
                mouse_y = event.button.y;
                break;
            case SDL_MOUSEBUTTONUP:
                move = 0;
                diff_x = 0;
                diff_y = 0;
                break;
            case SDL_MOUSEMOTION:
                if (move) {
                    rect.x -= diff_x;
                    rect.y -= diff_y;
                    diff_x = event.motion.x - mouse_x;
                    diff_y = event.motion.y - mouse_y;
                    rect.x += diff_x;
                    rect.y += diff_y;
                }
                break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, tex, NULL, &rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

exit:
    SDL_DestroyTexture(tex);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(w);
    SDL_Quit();

    return 0;
}

