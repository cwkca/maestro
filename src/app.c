#include "app.h"

#include <stdio.h>

SDL_Window *window = NULL;

/* Private functions */
void quit();

int app_init()
{
    if (SDL_Init(SDL_INIT_EVENTS))
        return 1;

    window = SDL_CreateWindow(NULL, 0, 0, 0, 0, 0);
    if (!window)
        return 1;

    return 0;
}

int app_start()
{
    printf("Hello world!\n");
    return 0;
}

void handle_key(SDL_Keysym keysym)
{
    quit();
}

void app_cleanup()
{
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

void quit()
{
    SDL_Event quit_event;
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);
}
