#ifndef SONGBUF_H
#define SONGBUF_H

#include <stdint.h>

typedef struct
{
    int16_t *data, *curr, *end;
    int capacity;
} SongBuffer;

void songbuf_init(SongBuffer *, int capacity);
void songbuf_clear(SongBuffer *buf);
void songbuf_write(SongBuffer *buf, int16_t data);
void songbuf_reset(SongBuffer *buf);
int16_t songbuf_next(SongBuffer *);
void songbuf_cleanup(SongBuffer *);

#endif /* SONGBUF_H */
