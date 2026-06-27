#ifndef SYNTH_H
#define SYNTH_H

#include "buffer.h"

void init_synth();
void play_note(char note); /* MIDI note number */
void stop_note();
void synth_cleanup();

#endif /* SYNTH_H */
