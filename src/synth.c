#include "app.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h> /* rand() */
#include <time.h>

#include <SDL.h>
#include "buffer.h"

#define SAMPLE_RATE 24000
#define LIVE_SAMPLE_COUNT 64
#define WEB_SAMPLE_COUNT 256
#define PLAYBACK_SAMPLE_COUNT 1024
#define BUFFER_SIZE 1600

const AMPLITUDE = INT16_MAX >> 3;
SDL_AudioDeviceID audio = 0;
AudioBuffer note_buffers[9];
AudioBuffer *synth_buffer, *play_buffers[8];

int TONE_PERIODS[] = {
    1468,
    1385,
    1308,
    1234,
    1165,
    1100,
    1038,
    980,
    925,
    873,
    824,
    778};

/* Private function prototypes */
void init_buffers();
void init_audio(int buffer_size);
void swap_in_buffer(char voice);
void synthesize(void *userdata, Uint8 *buffer, int len);
int get_note_period(char note);
int report_usecs(struct timespec start, int frequency);

void init_synth()
{
    init_buffers();
#ifdef __EMSCRIPTEN__
    init_audio(WEB_SAMPLE_COUNT);
#else
    init_audio(LIVE_SAMPLE_COUNT);
#endif
}

void play_note(char voice, char note)
{
    int period = get_note_period(note);
    if (!period)
        return;

    buffer_resize(synth_buffer, period);
    int16_t *sample = synth_buffer->start;
    int16_t *mid = sample + (period >> 1);
    while (sample < mid)
        *sample++ = AMPLITUDE;
    while (sample < synth_buffer->end)
        *sample++ = -AMPLITUDE;

    swap_in_buffer(voice);
}

void stop_note(char voice)
{
    buffer_clear(synth_buffer);
    swap_in_buffer(voice);
}

void synth_cleanup()
{
    if (audio)
        SDL_CloseAudioDevice(audio);

    int i;
    for (i = 0; i < 9; i++)
        buffer_cleanup(note_buffers + i);
}

/*
 * Private functions
 */

void init_buffers()
{
    char i;
    for (i = 0; i < 8; i++)
    {
        buffer_init(note_buffers + i, BUFFER_SIZE);
        play_buffers[i] = note_buffers + i;
    }

    buffer_init(note_buffers + i, BUFFER_SIZE);
    synth_buffer = note_buffers + i;
}

void init_audio(int buffer_size)
{
    SDL_AudioSpec requested_spec, provided_spec;

    SDL_zero(requested_spec);
    requested_spec.freq = SAMPLE_RATE;
    requested_spec.format = AUDIO_S16;
    requested_spec.channels = 1;
    requested_spec.samples = buffer_size;
    requested_spec.callback = synthesize;

    audio = SDL_OpenAudioDevice(NULL, 0, &requested_spec, &provided_spec, 0);

    /* Enforce required parameters */
    assert(provided_spec.format == AUDIO_S16);
    assert(provided_spec.channels == 1);
    assert(provided_spec.size == (buffer_size << 1));

    /* Warn on others */
    if (provided_spec.freq != SAMPLE_RATE)
        printf("Unexpected sample rate: %d Hz\n", provided_spec.freq);
    if (provided_spec.samples != buffer_size)
        printf("Unexpected buffer size: %d\n", provided_spec.samples);

    /* Unpause to start device */
    SDL_PauseAudioDevice(audio, 0);
}

void swap_in_buffer(char voice)
{
    AudioBuffer *old_buffer = play_buffers[voice];

    SDL_LockAudioDevice(audio);
    play_buffers[voice] = synth_buffer;
    SDL_UnlockAudioDevice(audio);

    synth_buffer = old_buffer;
}

void synthesize(void *userdata, Uint8 *buffer, int byte_count)
{
    char i;
    AudioBuffer *active_buffers[9];
    AudioBuffer **abuf = active_buffers;
    for (i = 0; i < 8; i++)
        if (!buffer_empty(play_buffers[i]))
            *(abuf++) = play_buffers[i];
    *abuf = NULL;

    int16_t value;
    int16_t *sample = (int16_t *)buffer;
    int16_t *end = (int16_t *)(buffer + byte_count);
    while (sample < end)
    {
        value = 0;
        for (abuf = active_buffers; *abuf; abuf++)
            value += buffer_get_circular(*abuf);
        *(sample++) = value;
    }
}

int get_note_period(char note)
{
    char octave = note / 12 - 1;
    if (note < 0 || octave < 0)
    {
        printf("Unsupported note %d\n", note);
        return 0;
    }

    return TONE_PERIODS[note % 12] >> octave;
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
