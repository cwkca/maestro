#ifndef NOTEBUF_H
#define NOTEBUF_H

#include <stdint.h>

typedef struct
{
    unsigned char voice, duration, velocity;
    signed char number;
} Note;

typedef struct
{
    Note *data, *curr, *end;
    int capacity;
} NoteBuffer;

#define notebuf_size(b) ((int)((b)->end - (b)->data))
#define notebuf_empty(b) ((b)->curr >= (b)->end)

void notebuf_init(NoteBuffer *, int capacity);
void notebuf_clear(NoteBuffer *buf);
void notebuf_write(NoteBuffer *buf, const Note *note);
void notebuf_reset(NoteBuffer *buf);
const Note *notebuf_next(NoteBuffer *);
void notebuf_cleanup(NoteBuffer *);

#endif /* NOTEBUF_H */
