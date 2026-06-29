#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>

#include "notebuf.h"

int init_player();
int read_song(FILE *file, NoteBuffer *songbuf);
void play_song(NoteBuffer *songbuf);
void player_cleanup();

#endif /* PLAYER_H */
