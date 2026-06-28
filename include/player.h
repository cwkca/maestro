#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>

#include "songbuf.h"

int read_song(FILE *file, SongBuffer *songbuf);
void play_song(SongBuffer *songbuf);

#endif /* PLAYER_H */
