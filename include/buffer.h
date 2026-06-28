#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

typedef struct
{
    int16_t *data, *start, *end;
    int capacity;
} AudioBuffer;

void audiobuf_init(AudioBuffer *buffer, int capacity);
void audiobuf_cleanup(AudioBuffer *buffer);

void audiobuf_clear(AudioBuffer *buffer);
void audiobuf_resize(AudioBuffer *buf, int size);
void audiobuf_zero(AudioBuffer *buffer);

int16_t audiobuf_get(AudioBuffer *buffer);
int16_t audiobuf_get_circular(AudioBuffer *buffer);
void audiobuf_print(AudioBuffer *buffer);

#define audiobuf_size(b) ((unsigned int)((b)->end - (b)->start))
#define audiobuf_empty(b) ((b)->start == (b)->end)

#endif /* BUFFER_H */
