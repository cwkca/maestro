#include "songbuf.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void songbuf_init(SongBuffer *buf, int capacity)
{
    buf->data = malloc(capacity << 1);
    if (!buf->data)
    {
        printf("Failed to allocate %d bytes\n", capacity << 1);
        exit(1);
    }

    buf->capacity = capacity;
    songbuf_clear(buf);
}

void songbuf_clear(SongBuffer *buf)
{
    buf->curr = buf->end = buf->data;
}

void songbuf_write(SongBuffer *buf, int16_t data)
{
    /* Todo: resize as needed */
    assert(buf->curr - buf->data < buf->capacity);

    *buf->curr = data;
    buf->curr = ++buf->end;
}

void song_reset(SongBuffer *buf)
{
    buf->curr = buf->data;
}

int16_t song_next(SongBuffer *buf)
{
    assert(buf->curr < buf->end);
    return *buf->curr++;
}

void songbuf_cleanup(SongBuffer *buf)
{
    if (buf->data)
        free(buf->data);

    buf->data = NULL;
    buf->capacity = 0;
    songbuf_clear(buf);
}
