#ifndef SYNTH_H
#define SYNTH_H

#include "buffer.h"

typedef enum
{
    NO_INSTRUMENT = 0,
    SQUARE_WAVE = 81,
    SAWTOOTH_WAVE = 82,
    TRIANGLE_WAVE = 83
} Instrument;

void init_synth();
void set_instrument(char voice, Instrument instrument);
void play_note(char voice, char note); /* MIDI note number */
void stop_note(char voice);
void synth_cleanup();

#endif /* SYNTH_H */
