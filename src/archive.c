/* Functions that aren't currently used, but may be a useful reference. */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "synth.h"

#define delay_ns 1e7
#define TICKS_PER_QUARTER 96
#define TIMING_LAG_NS 2e6

void sleep_test()
{
    int i, sec;
    long nsec, interval, error_ns;
    struct timespec start, end, delay = {0};
    delay.tv_nsec = delay_ns;

    for (i = 0; i < 100; i++)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        nanosleep(&delay, NULL);
        clock_gettime(CLOCK_MONOTONIC, &end);

        sec = end.tv_sec - start.tv_sec;
        nsec = end.tv_nsec - start.tv_nsec;
        interval = 1e9 * sec + nsec;

        error_ns = interval - delay_ns;
        delay.tv_nsec -= error_ns >> 2;
        printf("Overslept by %ld usec. New delay %ld usec.\n",
               error_ns / 1000, delay.tv_nsec / 1000);
    }
}

/* Placeholder functions */
void midibuf_write(uint16_t *song, uint16_t data) {}
char midibuf_over(uint16_t *song) { return 0; }
uint16_t midibuf_next(uint16_t *song) { return 0; }

void store_midi(signed char note_number, uint16_t duration)
{
    /* Placeholder vars */
    uint16_t *curr_song;
    signed char curr_voice, curr_octave;
    int tempo;

    assert((curr_voice & 0xF0) == 0);
    assert(duration < 0x8000);

    if (note_number < 0) /* Rest */
    {
        midibuf_write(curr_song, duration);
        return;
    }

    /* Todo: support staccato, legato, ties */
    int gap = duration >> 3;

    /* Todo: define macros? */
    /* Todo: interleave multiple voices */

    /* Note on */
    unsigned char status_byte = 0x90 + curr_voice;
    midibuf_write(curr_song, note_number | status_byte << 8);
    midibuf_write(curr_song, duration - gap);

    /* Note off */
    status_byte = 0x80 + curr_voice;
    midibuf_write(curr_song, note_number | status_byte << 8);
    midibuf_write(curr_song, gap);
}

void play_midi(uint16_t *songbuf)
{
    /* Placeholder var */
    int tempo;

    int16_t data;
    unsigned char note, voice;
    struct timespec delay;
    long ns_delay, ns_per_tick = 6e10 / (tempo * TICKS_PER_QUARTER);

    while (!midibuf_over(songbuf))
    {
        data = midibuf_next(songbuf);
        assert(data); /* Zero bytes not allowed */
        /* printf("Playing value 0x%04x\n", (uint16_t)data); */

        if (data > 0)
        {
            ns_delay = data * ns_per_tick - TIMING_LAG_NS;
            delay.tv_nsec = ns_delay % (long)1e9;
            delay.tv_sec = ns_delay / 1e9;
            nanosleep(&delay, NULL);
        }
        else
        {
            note = data & 0xFF;
            voice = 15 & data >> 8;

            if (1 & data >> 12)
                play_note(voice, note);
            else
                stop_note(voice);
        }
    }
}
