#include "app.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h> /* rand() */
#include <time.h>

#include <SDL.h>

#define SAMPLE_RATE 24000
#define LIVE_SAMPLE_COUNT 64
#define PLAYBACK_SAMPLE_COUNT 1024

SDL_Window *window = NULL;
SDL_AudioDeviceID audio = 0;

/* Private function prototypes */
int init_sdl();
void init_audio();
void play();
void quit();
void output_audio(void *userdata, Uint8 *buffer, int len);

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
    SDL_AudioSpec requested_spec, provided_spec;

    SDL_zero(requested_spec);
    requested_spec.freq = SAMPLE_RATE;
    requested_spec.format = AUDIO_S16;
    requested_spec.channels = 1;
    requested_spec.samples = 256; /* For testing */
    requested_spec.callback = output_audio;

    audio = SDL_OpenAudioDevice(NULL, 0, &requested_spec, &provided_spec, 0);

    /* Enforce required parameters */
    assert(provided_spec.format == AUDIO_S16);
    assert(provided_spec.channels == 1);

    /* Warn on others */
    if (provided_spec.freq != SAMPLE_RATE)
        printf("Unexpected sample rate: %d Hz\n", provided_spec.freq);
    if (provided_spec.samples != 256)
        printf("Unexpected buffer size: %d\n", provided_spec.samples);
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

void output_audio(void *userdata, Uint8 *buffer, int len)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int16_t *sample = (int16_t *)buffer;
    int16_t *sample_mid = (int16_t *)(buffer + (len >> 1));
    int16_t *sample_end = (int16_t *)(buffer + len);

    while (sample++ < sample_mid)
        *sample = INT16_MAX;
    while (sample++ < sample_end)
        *sample = INT16_MIN;

    clock_gettime(CLOCK_MONOTONIC, &end);
    int diff_sec = end.tv_sec - start.tv_sec;
    int diff_ns = end.tv_nsec - start.tv_nsec;
    int diff_us = (1e9 * diff_sec + diff_ns) / 1e3;

    if (!(rand() & 0x7f))
        printf("%d square wave samples synthesized in %dµs\n", len >> 1, diff_us);
}
