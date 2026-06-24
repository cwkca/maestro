#include "app.h"

#include <stdio.h>
#include <SDL.h>

SDL_Window *window = NULL;
SDL_AudioDeviceID audio = 0;

/* Private function prototypes */
int init_sdl();
void init_audio();
void play();
void quit();
void AudioCallback(void *userdata, Uint8 *buffer, int len);

int app_init()
{
    if (init_sdl())
        return 1;
    init_audio();
    return 0;
}

int app_start()
{
    play();
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
    if (audio)
        SDL_CloseAudioDevice(audio);
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

void init_audio()
{
    SDL_AudioSpec want, have;

    SDL_zero(want);
    want.freq = 24000;
    want.format = AUDIO_S16;
    want.channels = 1;
    want.samples = 256;
    want.callback = AudioCallback;

    audio = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
}

void play()
{
    SDL_PauseAudioDevice(audio, 0); // Unpause to start playing
}

void quit()
{
    SDL_Event quit_event;
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);
}

void AudioCallback(void *userdata, Uint8 *buffer, int len)
{
    int16_t *sample = (int16_t *)buffer;
    int16_t *sample_mid = (int16_t *)(buffer + (len >> 1));
    int16_t *sample_end = (int16_t *)(buffer + len);

    while (sample++ < sample_mid)
        *sample = INT16_MAX;
    while (sample++ < sample_end)
        *sample = INT16_MIN;
}
