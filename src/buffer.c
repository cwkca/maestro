#include "buffer.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void audiobuf_init(AudioBuffer *buf, int capacity)
{
    assert(capacity > 0);

    buf->data = malloc(capacity << 1);
    if (!buf->data)
    {
        printf("Failed to allocate %d bytes of memory\n", capacity << 1);
        exit(1);
    }
    buf->capacity = capacity;
    audiobuf_clear(buf);
}

void audiobuf_cleanup(AudioBuffer *buf)
{
    if (buf->data)
        free(buf->data);

    buf->start = buf->end = buf->data = NULL;
    buf->capacity = 0;
}

void audiobuf_clear(AudioBuffer *buf)
{
    audiobuf_resize(buf, 0);
}

void audiobuf_resize(AudioBuffer *buf, int size)
{
    assert(size <= buf->capacity);
    buf->start = buf->data;
    buf->end = buf->data + size;
}

void audiobuf_zero(AudioBuffer *buf)
{
    memset(buf->data, 0, (audiobuf_size(buf) << 1));
}

int16_t audiobuf_get(AudioBuffer *buf)
{
    assert(buf->start < buf->end);
    return audiobuf_empty(buf) ? 0 : *(buf->start++);
}

int16_t audiobuf_get_circular(AudioBuffer *buf)
{
    int16_t sample = audiobuf_get(buf);

    if (audiobuf_empty(buf))
        buf->start = buf->data;

    return sample;
}

void audiobuf_print(AudioBuffer *buf)
{
    int16_t *sample;
    printf("Buffer size %d, values: ", audiobuf_size(buf));
    for (sample = buf->start; sample < buf->end; sample++)
        printf("%d ", *sample);
    printf("\n");
}
