#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include "chip8.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} SDL_Context;

const int keymap[16] = {
    SDL_SCANCODE_X,
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_W,
    SDL_SCANCODE_E,
    SDL_SCANCODE_A,
    SDL_SCANCODE_S,
    SDL_SCANCODE_D,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_C,
    SDL_SCANCODE_4,
    SDL_SCANCODE_R,
    SDL_SCANCODE_F,
    SDL_SCANCODE_V
};

SDL_Context *sdlInit() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL %s\n", SDL_GetError());
        exit(-1);
    }

    SDL_Context *context = malloc(sizeof(SDL_Context));

    if (context == NULL) {
        printf("Failed to allocate memory for SDL context\n");
        SDL_Quit();
        return NULL;
    }

    // Create window
    context->window = SDL_CreateWindow(
        "CHIP8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1024, 512,
        SDL_WINDOW_SHOWN
    );

    if (context->window == NULL) {
        printf("Window could not be created, SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }

    // Create renderer
    context->renderer = SDL_CreateRenderer(context->window, -1, SDL_RENDERER_ACCELERATED);
    
    if (context->renderer == NULL) {
        printf("Window could not be created, SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }

    context->texture = SDL_CreateTexture(context->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    return context;
}

void sdl_cleaning(SDL_Context *context) {
    SDL_DestroyWindow(context->window);
    SDL_DestroyRenderer(context->renderer);
    SDL_DestroyTexture(context->texture);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    char *filename;
    chip8 *myChip8 = malloc(sizeof(chip8));
    chip8_initialize(myChip8);

    if (myChip8 == NULL) {
        fprintf(stderr, "Failed to create a chip 8 struct");
        exit(-1);
    }

    SDL_Context *ct = sdlInit();

    if (argc < 2) {
        fprintf(stderr, "No ROM provided, please provide one\n");
        exit(-1);
    }
    else {
        filename = argv[1];
    }

    //load a rom
    loadRom(filename, myChip8);
    bool open = true;
    SDL_Event event;

    while(open) {
        //emulator cycle
        chip8_emulateCycle(myChip8);


        while(SDL_PollEvent(&event) > 0) {
            if (event.type == SDL_QUIT) {
                open = false;
            }
            if (event.type == SDL_KEYDOWN) {
                int i = 0;
                while (i < 16) {
                    if (event.key.keysym.scancode == keymap[i]) {
                        myChip8 -> key[i] = 1;
                    }
                    i += 1;
                }
            }
            if (event.type == SDL_KEYUP) {
                int i = 0;
                while (i < 16) {
                    if (event.key.keysym.scancode == keymap[i]) {
                        myChip8 -> key[i] = 0;
                    }
                    i += 1;
                }
            }
        }

        SDL_RenderClear(ct -> renderer);

        //build texture
        void *pix = NULL;
        uint32_t *pixel_data;
        int pitch = 0;

        SDL_LockTexture(ct -> texture, NULL, &pix, &pitch);
        pixel_data = (u_int32_t *)pix;

        for(int i = 0; i < 2048; i++) {
            if (myChip8 -> gfx[i] == 1) {
                pixel_data[i] = 0xFFFFFFFF;
            }
            else {
                pixel_data[i] = 0x000000FF;
            }
        }

        SDL_UnlockTexture(ct -> texture);
        SDL_RenderCopy(ct -> renderer, ct -> texture, NULL, NULL);
        SDL_RenderPresent(ct -> renderer);

        SDL_Delay(16);
    }


    sdl_cleaning(ct);

}