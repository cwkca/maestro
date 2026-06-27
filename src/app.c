#include "app.h"

#include <stdio.h>
#include <unistd.h>

#include <SDL.h>

#include "synth.h"

SDL_Window *window = NULL;
signed char keys_down = 0, octave = 4;
#define INVALID_NOTE 50

signed char KEY_NOTES[] = {
    -4, 4, 0, INVALID_NOTE, INVALID_NOTE, 1, 3, INVALID_NOTE, INVALID_NOTE,
    6, 8, 10, 7, 5, INVALID_NOTE, INVALID_NOTE, INVALID_NOTE, INVALID_NOTE,
    -2, INVALID_NOTE, INVALID_NOTE, 2, INVALID_NOTE, -1, INVALID_NOTE, -3};

/* Private function prototypes */
int init_sdl();
void quit();
signed char get_note_for_key(SDL_KeyCode key);

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
    else if (event.type == SDL_KEYDOWN && !event.key.repeat)
    {
        keys_down++;
        SDL_KeyCode key = event.key.keysym.sym;
        if (key == SDLK_ESCAPE)
            quit();
        else if (isdigit(key))
        {
            octave = key - '0';
        }
        else
        {
            signed char note = get_note_for_key(key);
            if (note != INVALID_NOTE)
                play_note(12 + 12 * octave + note);
        }
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

signed char get_note_for_key(SDL_KeyCode key)
{
    switch (key)
    {
    case SDLK_LSHIFT:
        return -5;
    case ',':
        return 9;
    case '.':
        return 11;
    case '/':
        return 12;
    case '\'':
        return 13;
    case SDLK_RSHIFT:
        return 14;
    default:
        return isalpha(key) ? KEY_NOTES[tolower(key) - 'a'] : INVALID_NOTE;
    }
}