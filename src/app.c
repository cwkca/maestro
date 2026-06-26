#include "app.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h> /* rand() */
#include <time.h>

#include <SDL.h>
#include "buffer.h"

#define SAMPLE_RATE 24000
#define LIVE_SAMPLE_COUNT 64
#define TEST_SAMPLE_COUNT 256
#define PLAYBACK_SAMPLE_COUNT 1024

SDL_Window *window = NULL;
SDL_AudioDeviceID audio = 0;
AudioBuffer audio_buffer;
SDL_Event synthesis_event;

/* Private function prototypes */
int init_sdl();
void init_audio();
void init_buffers();
void play();
void quit();
void synthesize_audio();
void output_audio(void *userdata, Uint8 *buffer, int len);
int report_usecs(struct timespec start, int frequency);

int app_init()
{
    if (init_sdl())
        return 1;
    init_audio();
    init_buffers();
    return 0;
}

int app_start()
{
    play();
    return 0;
}

void handle_event(SDL_Event event)
{
    if (event.type == synthesis_event.type)
        synthesize_audio();
    else if (event.type == SDL_KEYDOWN)
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

    SDL_zero(synthesis_event);
    synthesis_event.type = SDL_RegisterEvents(1);
    assert(synthesis_event.type < SDL_LASTEVENT);

    return 0;
}

void init_audio()
{
    SDL_AudioSpec requested_spec, provided_spec;

    SDL_zero(requested_spec);
    requested_spec.freq = SAMPLE_RATE;
    requested_spec.format = AUDIO_S16;
    requested_spec.channels = 1;
    requested_spec.samples = TEST_SAMPLE_COUNT;
    requested_spec.callback = output_audio;

    audio = SDL_OpenAudioDevice(NULL, 0, &requested_spec, &provided_spec, 0);

    /* Enforce required parameters */
    assert(provided_spec.format == AUDIO_S16);
    assert(provided_spec.channels == 1);
    assert(provided_spec.size == (TEST_SAMPLE_COUNT << 1));

    /* Warn on others */
    if (provided_spec.freq != SAMPLE_RATE)
        printf("Unexpected sample rate: %d Hz\n", provided_spec.freq);
    if (provided_spec.samples != TEST_SAMPLE_COUNT)
        printf("Unexpected buffer size: %d\n", provided_spec.samples);
}

void init_buffers()
{
    buffer_init(&audio_buffer, TEST_SAMPLE_COUNT);

    /* Temporary static square wave */
    buffer_resize(&audio_buffer, TEST_SAMPLE_COUNT);
    int16_t *sample = audio_buffer.data;
    int16_t *mid = sample + (TEST_SAMPLE_COUNT >> 1);
    while (sample++ < mid)
        *sample = INT16_MAX;
    while (sample++ < audio_buffer.end)
        *sample = INT16_MIN;
}

void play()
{
    /* Unpause to start playing */
    SDL_PauseAudioDevice(audio, 0);
}

void quit()
{
    SDL_Event quit_event;
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);
}

void synthesize_audio()
{
}

void output_audio(void *userdata, Uint8 *buffer, int byte_count)
{
    assert(byte_count == (audio_buffer.size << 1));
    memcpy(buffer, audio_buffer.data, byte_count);
    SDL_PushEvent(&synthesis_event);
}

/* Populate start with clock_gettime(CLOCK_MONOTONIC, &start) */
int report_usecs(struct timespec start, int frequency)
{
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);

    if (frequency > 1 && rand() % frequency)
        return 0;

    int diff_sec = end.tv_sec - start.tv_sec;
    int diff_ns = end.tv_nsec - start.tv_nsec;
    return (1e9 * diff_sec + diff_ns) / 1e3;
}
