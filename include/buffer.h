#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

typedef struct
{
    int16_t *data, *start, *end;
    int size, capacity;
} AudioBuffer;

void buffer_init(AudioBuffer *buffer, int capacity);
void buffer_cleanup(AudioBuffer *buffer);

void buffer_clear(AudioBuffer *buffer);
void buffer_resize(AudioBuffer *buf, int size);
void buffer_zero(AudioBuffer *buffer);

int16_t buffer_get(AudioBuffer *buffer);
int16_t buffer_get_circular(AudioBuffer *buffer);
#define buffer_len(b) ((b)->end - (b)->start)
#define buffer_empty(b) ((b)->start == (b)->end)

#endif /* BUFFER_H */
