#include "buffer.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void buffer_init(AudioBuffer *buf, int capacity)
{
    assert(capacity > 0);

    buf->data = malloc(capacity << 1);
    if (!buf->data)
    {
        printf("Failed to allocate %d bytes of memory\n", capacity << 1);
        exit(1);
    }
    buf->capacity = capacity;
    buffer_clear(buf);
}

void buffer_cleanup(AudioBuffer *buf)
{
    if (buf->data)
        free(buf->data);

    buf->start = buf->end = buf->data = NULL;
    buf->size = buf->capacity = 0;
}

void buffer_clear(AudioBuffer *buf)
{
    buffer_resize(buf, 0);
}

void buffer_resize(AudioBuffer *buf, int size)
{
    assert(size <= buf->capacity);
    buf->size = size;
    buf->start = buf->data;
    buf->end = buf->data + size;
}

void buffer_zero(AudioBuffer *buf)
{
    memset(buf->data, 0, (buf->size << 1));
}

int16_t buffer_get(AudioBuffer *buf)
{
    assert(buf->start < buf->end);
    return buffer_empty(buf) ? 0 : *(buf->start++);
}

int16_t buffer_get_circular(AudioBuffer *buf)
{
    int16_t byte = buffer_get(buf);

    if (buffer_empty(buf))
        buf->start = buf->data;

    return byte;
}
