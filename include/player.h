#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>

#include "songbuf.h"

int init_player();
int read_song(FILE *file, SongBuffer *songbuf);
void play_song(SongBuffer *songbuf);
void player_cleanup();

#endif /* PLAYER_H */
