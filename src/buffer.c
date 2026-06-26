#include "buffer.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void buffer_init(AudioBuffer *buf, int size)
{
    assert(size > 0);

    buf->data = malloc(size << 1);
    if (!buf->data)
    {
        printf("Failed to allocate %d bytes of memory\n", size << 1);
        exit(1);
    }

    buf->size = size;
    buf->end = buf->data + size;
    buf->start = buf->end;
}

void buffer_cleanup(AudioBuffer *buf)
{
    if (buf->data)
        free(buf->data);

    buf->data = buf->start = buf->end = NULL;
    buf->size = 0;
}

void buffer_clear(AudioBuffer *buf)
{
    buf->start = buf->end;
}

void buffer_zero(AudioBuffer *buf)
{
    memset(buf->data, 0, (buf->size << 1));
    buf->start = buf->data;
}

int16_t buffer_get(AudioBuffer *buf)
{
    return buffer_empty(buf) ? 0 : *(buf->start++);
}

int16_t buffer_get_circular(AudioBuffer *buf)
{
    int16_t byte = buffer_get(buf);

    if (buffer_empty(buf))
        buf->start = buf->data;

    return byte;
}
