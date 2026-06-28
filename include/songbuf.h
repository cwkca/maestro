#ifndef SONGBUF_H
#define SONGBUF_H

#include <stdint.h>

typedef struct
{
    int16_t *data, *curr, *end;
    int capacity;
} SongBuffer;

#define songbuf_size(b) ((int)((b)->end - (b)->data))
#define song_over(b) ((b)->curr >= (b)->end)

void songbuf_init(SongBuffer *, int capacity);
void songbuf_clear(SongBuffer *buf);
void songbuf_write(SongBuffer *buf, int16_t data);
void song_reset(SongBuffer *buf);
int16_t song_next(SongBuffer *);
void songbuf_cleanup(SongBuffer *);

#endif /* SONGBUF_H */
