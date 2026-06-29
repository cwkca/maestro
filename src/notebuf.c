#include "notebuf.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void notebuf_init(NoteBuffer *buf, int capacity)
{
    buf->data = malloc(capacity << 1);
    if (!buf->data)
    {
        printf("Failed to allocate %d bytes\n", capacity << 1);
        exit(1);
    }

    buf->capacity = capacity;
    notebuf_clear(buf);
}

void notebuf_clear(NoteBuffer *buf)
{
    buf->curr = buf->end = buf->data;
}

void notebuf_write(NoteBuffer *buf, const Note *note)
{
    /* Todo: resize as needed */
    assert(buf->curr - buf->data < buf->capacity);

    *buf->curr = *note;
    buf->curr = ++buf->end;
}

void notebuf_reset(NoteBuffer *buf)
{
    buf->curr = buf->data;
}

const Note *notebuf_next(NoteBuffer *buf)
{
    assert(buf->curr < buf->end);
    return buf->curr++;
}

void notebuf_cleanup(NoteBuffer *buf)
{
    if (buf->data)
        free(buf->data);

    buf->data = NULL;
    buf->capacity = 0;
    notebuf_clear(buf);
}
