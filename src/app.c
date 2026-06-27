#include "app.h"

#include <stdio.h>
#include <SDL.h>

#include "synth.h"

SDL_Window *window = NULL;
char keys_down = 0;

/* Private function prototypes */
int init_sdl();
void quit();

int app_init()
{
    if (init_sdl())
        return 1;
    init_synth();
    return 0;
}

int app_start()
{
    return 0;
}

void handle_event(SDL_Event event)
{
    if (event.type == SDL_KEYUP)
    {
        if (keys_down > 0)
            keys_down--;
        if (!keys_down)
            stop_note();
    }
    else if (event.type == SDL_KEYDOWN)
    {
        keys_down++;
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_ESCAPE)
            quit();
        else
            play_note(100);
    }
}

void app_cleanup()
{
    synth_cleanup();
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

/*
 * Private functions
 */

int init_sdl()
{
    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_AUDIO))
        return 1;

    window = SDL_CreateWindow(NULL, 0, 0, 0, 0, 0);
    if (!window)
        return 1;

    return 0;
}

void quit()
{
    SDL_Event quit_event;
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);
}
