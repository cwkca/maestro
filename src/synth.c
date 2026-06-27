#include "app.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h> /* rand() */
#include <time.h>

#include <SDL.h>
#include "buffer.h"

#define SAMPLE_RATE 24000
#define LIVE_SAMPLE_COUNT 64
#define PLAYBACK_SAMPLE_COUNT 1024
#define MAX_PERIOD 1500

SDL_AudioDeviceID audio = 0;
AudioBuffer note_buffer[2];
AudioBuffer *play_buffer, *synth_buffer;

/* Private function prototypes */
void init_buffers();
void init_audio();
void swap_buffers();
void synthesize(void *userdata, Uint8 *buffer, int len);
int report_usecs(struct timespec start, int frequency);

void init_synth()
{
    init_buffers();
    init_audio();
}

void play_note(int period)
{
    buffer_resize(synth_buffer, period);
    int16_t *sample = synth_buffer->start;
    int16_t *mid = sample + (buffer_size(synth_buffer) >> 1);
    while (sample < mid)
        *sample++ = INT16_MAX;
    while (sample < synth_buffer->end)
        *sample++ = INT16_MIN;

    swap_buffers();
}

void stop_note()
{
    buffer_clear(synth_buffer);
    swap_buffers();
}

void synth_cleanup()
{
    if (audio)
        SDL_CloseAudioDevice(audio);
    buffer_cleanup(note_buffer);
    buffer_cleanup(note_buffer + 1);
}

/*
 * Private functions
 */

void init_buffers()
{
    buffer_init(note_buffer, MAX_PERIOD);
    buffer_init(note_buffer + 1, MAX_PERIOD);
    play_buffer = note_buffer;
    synth_buffer = note_buffer + 1;
}

void init_audio()
{
    SDL_AudioSpec requested_spec, provided_spec;

    SDL_zero(requested_spec);
    requested_spec.freq = SAMPLE_RATE;
    requested_spec.format = AUDIO_S16;
    requested_spec.channels = 1;
    requested_spec.samples = LIVE_SAMPLE_COUNT;
    requested_spec.callback = synthesize;

    audio = SDL_OpenAudioDevice(NULL, 0, &requested_spec, &provided_spec, 0);

    /* Enforce required parameters */
    assert(provided_spec.format == AUDIO_S16);
    assert(provided_spec.channels == 1);
    assert(provided_spec.size == (LIVE_SAMPLE_COUNT << 1));

    /* Warn on others */
    if (provided_spec.freq != SAMPLE_RATE)
        printf("Unexpected sample rate: %d Hz\n", provided_spec.freq);
    if (provided_spec.samples != LIVE_SAMPLE_COUNT)
        printf("Unexpected buffer size: %d\n", provided_spec.samples);

    /* Unpause to start device */
    SDL_PauseAudioDevice(audio, 0);
}

void swap_buffers()
{
    AudioBuffer *old_buffer = play_buffer;

    SDL_LockAudioDevice(audio);
    play_buffer = synth_buffer;
    SDL_UnlockAudioDevice(audio);

    synth_buffer = old_buffer;
}

void synthesize(void *userdata, Uint8 *buffer, int byte_count)
{
    if (buffer_empty(play_buffer))
        memset(buffer, 0, byte_count);
    else
    {
        int16_t *sample = (int16_t *)buffer;
        int16_t *end = (int16_t *)(buffer + byte_count);
        while (sample < end)
            *(sample++) = buffer_get_circular(play_buffer);
    }
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
